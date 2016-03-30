#include "exception.hh"
#include "image.hh"

dm::image::image(dmBlock *init)
  : block(init),
    descriptor( dmImageGetDataDescriptor( m_block ) )
{
}

dm::image::~image()
{
  if( m_block != 0 )
    dmBlockClose(m_block);

  // don't do anything with descriptors (I think)
}

template<class T> void dm::image::create_memimage(memimage<T> **im)
{
  pix_vec dims;
  get_dimensions( &dims );

  if( dims.size() != 2 ) {
    except_invalid_param e;
    e.set_descr("Invalid number of dimensions in dm::image::create_memimage");
    throw e;
  }

  // lower range of coords
  pix_vec lower;
  lower.push_back(1); lower.push_back(1);

  // get data for range
  T* data;
  get_subarray(lower, dims, &data);

  // create image
  *im = new memimage<T>(dims[0], dims[1], data);

  delete[] data;
}

template<class T> void dm::image::write_from_memimage(const memimage<T>& im)
{
  pix_vec dims;
  get_dimensions( &dims );

  if( dims.size() != 2 ) {
    except_invalid_param e;
    e.set_descr("Invalid number of dimensions in dm::image::write_from_memimage");
    throw e;
  }

  if( dims[0] != im.xw() || dims[1] != im.yw() ) {
    except_invalid_param e;
    e.set_descr("Dimensions of image and data block not matched");
    throw e;
  }

  // lower range of coords
  pix_vec lower;
  lower.push_back(1); lower.push_back(1);

  // copy data
  const std::valarray<T>& data = im.data();
  const int size = data.size();
  T* copy = new T[size];
  for(int i=0; i<size; i++)
    copy[i] = data[i];

  set_subarray(lower, dims, copy);

  delete[] copy;
}

#define DM_DEFINE_TEMPL(TYPE) \
 template void \
  dm::image::create_memimage(memimage<TYPE> **im); \
 template void \
  dm::image::write_from_memimage(const memimage<TYPE>& im);

DM_DEFINE_TEMPL(short)
DM_DEFINE_TEMPL(long)
DM_DEFINE_TEMPL(float)
DM_DEFINE_TEMPL(double)
DM_DEFINE_TEMPL(unsigned char)
DM_DEFINE_TEMPL(unsigned short)
DM_DEFINE_TEMPL(unsigned long)

#undef DM_DEFINE_TEMPL
