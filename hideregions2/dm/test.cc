#include <iostream>
#include <vector>
#include <typeinfo>
#include <cmath>
#include "dataset.hh"
#include "image.hh"
#include "exception.hh"
#include "general.hh"
#include "memimage.hh"

int main()
{
  try {
    dm::dataset ds("cen_image_b2.fits", dm::open);

    dm::image* im = ds.get_image();

    dm::memimage<double>* mem;
    im->create_memimage(&mem);
    dm::memimage<double>& m = *mem;

    std::cout << m(19, 19) << '\n';

    delete mem;
    delete im;
  }
  catch( dm::exception& e ) {
    std::cout << "Exception: "
	      << e() << '\n';
  }

  return 0;
}
