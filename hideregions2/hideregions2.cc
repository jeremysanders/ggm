// New version of hide regions, which iterates over input regions,
// replacing contents of regions with random choices from the pixels
// in a slightly expanded region

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <dm/dm.hh>

#define EXPANDSIZE 1

// standard CAIO region files header
extern "C"
{
# include <cxcregion.h>
}

class Region
{
public:
  Region(const std::string& str)
  {
    reg = regParse(const_cast<char*>(str.c_str()));
    if(reg == 0)
      throw std::string("Invalid region: ") + str;
  }

  ~Region()
  {
    regFree(reg);
  }

  bool inside(double x, double y)
  {
    return regInsideRegion(reg, x, y);
  }

private:
  regRegion* reg;
};

struct Transform
{
  Transform(dm::image* im)
  {
    dmDescriptor* imdesc = im->get_descriptor();
    dmDescriptor* phys = dmArrayGetAxisGroup(imdesc, 1);
    dmCoordGetTransform_d(phys, pcrpix, pcrval, pcdlt, 2);
  }

  double x2phys(unsigned x) const
  {
    return (x + 1 - pcrpix[0])*pcdlt[0] + pcrval[0];
  }

  double y2phys(unsigned y) const
  {
    return (y + 1 - pcrpix[1])*pcdlt[1] + pcrval[1];
  }

  double pcrpix[2], pcrval[2], pcdlt[2];
};

void fillRegion(Region* reg, Region* enlarge, const Transform& trans,
                const dm::memimage<float>* inimage,
                dm::memimage<float>* outimage)
{
  std::vector<float> vals;

  unsigned minx=outimage->xw(), maxx=0;
  unsigned miny=outimage->yw(), maxy=0;

  for(unsigned y = 0; y < inimage->yw(); ++y)
    for(unsigned x = 0; x < inimage->xw(); ++x)
      {
        double px = trans.x2phys(x);
        double py = trans.y2phys(y);

        if(enlarge->inside(px, py) && !reg->inside(px, py))
          {
            minx=std::min(minx, x);
            miny=std::min(miny, y);
            maxx=std::max(maxx, x);
            maxy=std::max(maxy, y);

            vals.push_back((*inimage)(x, y));
          }
      }

  if(vals.empty())
    return;

  for(unsigned y = miny; y <= maxy; ++y)
    for(unsigned x = minx; x <= maxx; ++x)
      {
        double px = trans.x2phys(x);
        double py = trans.y2phys(y);

        if(reg->inside(px, py))
          {
            (*outimage)(x, y) =
              vals[unsigned(rand()*(1./RAND_MAX)*vals.size())];
          }
      }
}

std::string enlargeRegion(const std::string& str)
{
  boost::char_separator<char> sep("(,)");
  boost::tokenizer< boost::char_separator<char> > tokens(str, sep);
  //BOOST_FOREACH (const std::string& t, tokens) {
  //  std::cout << t << "." << std::endl;
  // }

  boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin();

  std::string name(*it++);
  std::string out(name + "(");
  if(name == "ellipse")
    {
      out += *it++;
      out += ",";
      out += *it++;
      out += ",";

      double rad1(boost::lexical_cast<double>(*it++));
      out += boost::lexical_cast<std::string>(rad1+EXPANDSIZE);
      out += ",";
      double rad2(boost::lexical_cast<double>(*it++));
      out += boost::lexical_cast<std::string>(rad2+EXPANDSIZE);
      out += ",";

      out += *it++;
    }
  else if(name == "circle")
    {
      out += *it++;
      out += ",";
      out += *it++;
      out += ",";

      double rad(boost::lexical_cast<double>(*it++));
      out += boost::lexical_cast<std::string>(rad+EXPANDSIZE);
    }
  else
    {
      throw std::string("Cannot interpret region");
    }

  out += ")";
  return out;
}

void run(const std::string& infile,
         const std::string& regfile,
         const std::string& outfile)
{
  // load in image
  dm::dataset ds(infile);
  dm::image* im = ds.get_image();

  Transform trans(im);
  
  // load the input image into memory
  dm::memimage<float>* inimage;
  im->create_memimage(&inimage);

  dm::memimage<float> outimage(inimage->xw(), inimage->yw());
  for(unsigned y=0; y<outimage.yw(); ++y)
    for(unsigned x=0; x<outimage.xw(); ++x)
      outimage(x, y) = (*inimage)(x, y);

  std::ifstream inreg(regfile.c_str());
  if(!inreg)
    {
      throw std::string("Cannot open region file ") + regfile;
    }

  std::string line;
  while(std::getline(inreg, line))
    {
      if(line.length() >= 1 && line[0] == '#')
        continue;

      std::cout << "Region: " << line << '\n';
      Region reg(line);

      Region enlarge( enlargeRegion(line) );

      fillRegion(&reg, &enlarge, trans, inimage, &outimage);
    }

  dm::dataset ds_im_out(outfile, dm::create_over);
  dm::image *im_im_out = ds_im_out.create_image("IMAGE", dmFLOAT,
                                                outimage.xw(),
						outimage.yw());
  im_im_out->write_from_memimage(outimage);
}

int main(int argc, char* argv[])
{
  if( argc != 4 )
    {
      std::cerr << "Usage: "
		<< argv[0]
		<< " infile.fits region.reg outfile.fits\n";
      return 1;
    }

  try
    {
      run(argv[1], argv[2], argv[3]);
    }
  catch(std::string s)
    {
      std::cerr << s << '\n';
      return 1;
    }

  return 0;
}
