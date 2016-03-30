#include <cstdlib>
#include <typeinfo>
#include "descriptor.hh"
#include "general.hh"
#include "exception.hh"

dm::descriptor::descriptor(dmDescriptor* d)
{
  m_descriptor = d;
  m_dtype = dmGetDataType(d);
}

template<class T> void
dm::descriptor::set_pixel(const pix_vec& pos, T val)
{
  arraycopy a( pos );

  int r;
  switch( get_data_type() ) {
  case dmSHORT:
    r = dmImageDataSetPixel_s(m_descriptor, a(), short(val) ); break;
  case dmLONG:
    r = dmImageDataSetPixel_l(m_descriptor, a(), long(val) ); break;
  case dmFLOAT:
    r = dmImageDataSetPixel_f(m_descriptor, a(), float(val) ); break;
  case dmDOUBLE:
    r = dmImageDataSetPixel_d(m_descriptor, a(), double(val) ); break;
  case dmBYTE:
    r = dmImageDataSetPixel_ub(m_descriptor, a(), (unsigned char)(val) ); break;
  case dmUSHORT:
    r = dmImageDataSetPixel_us(m_descriptor, a(), (unsigned short)(val) ); break;
  case dmULONG:
    r = dmImageDataSetPixel_ul(m_descriptor, a(), (unsigned long)(val) ); break;
  default:
    except_unknown e;
    e.set_descr("Invalid data type in dm::descriptor::set_pixel()");
    throw e;
  }

  if( r == dmFAILURE ) {
    except_invalid_param e;
    e.set_descr("Invalid return from dmImageDataSetPixel_x");
    throw e;
  }
}

template<class T> void
dm::descriptor::get_pixel(const pix_vec& pos, T* val)
{
  arraycopy a( pos );

  const long dim = pos.size();

  switch( get_data_type() ) {
  case dmSHORT:
    *val = T( dmImageDataGetPixel_s(m_descriptor, a(), dim ) ); break;
  case dmLONG:
    *val = T( dmImageDataGetPixel_l(m_descriptor, a(), dim ) ); break;
  case dmFLOAT:
    *val = T( dmImageDataGetPixel_f(m_descriptor, a(), dim ) ); break;
  case dmDOUBLE:
    *val = T( dmImageDataGetPixel_d(m_descriptor, a(), dim ) ); break;
  case dmBYTE:
    *val = T( dmImageDataGetPixel_ub(m_descriptor, a(), dim ) ); break;
  case dmUSHORT:
    *val = T( dmImageDataGetPixel_us(m_descriptor, a(), dim ) ); break;
  case dmULONG:
    *val = T( dmImageDataGetPixel_ul(m_descriptor, a(), dim ) ); break;
  default:
    except_unknown e;
    e.set_descr("Invalid data type in dm::descriptor::get_pixel()");
    throw e;
  }
}

namespace dm{
  // copies T2[] to T1 (size items)
  template<class T1, class T2> void _translate_array(T1* a, T2* b, unsigned size)
  {
    for(unsigned i=0; i<size; ++i) {
      *a = T1(*b);
      a++; b++;
    }
  }

  unsigned get_total_size(const pix_vec& lowerbounds,
			  const pix_vec& upperbounds)
  {
    const unsigned no = lowerbounds.size();
    if( no != upperbounds.size() ) {
      except_invalid_param e;
      e.set_descr("Mismatching subarray bound sizes in dm::descriptor::get_total_size()");
      throw e;
    }

    unsigned mult = 1;
    for(unsigned i=0; i<no; ++i)
      mult *= (upperbounds[i] - lowerbounds[i] + 1);

    return mult;
  }

}

// macro simplifies template below a lot
#define DM_DESCRIPTOR_GETSUBARRAY(TYPE, EXTEN) \
{\
    TYPE *x = new TYPE[size];\
    r = dmImageDataGetSubArray ## EXTEN (m_descriptor, l(), u(), x);\
    _translate_array(*val, x, size);\
    delete[] x; break;\
}

template<class T> void
dm::descriptor::get_subarray(const pix_vec& lowerbounds,
			     const pix_vec& upperbounds,
			     T** val)
{
  const int size = get_total_size(lowerbounds, upperbounds);
  arraycopy l( lowerbounds), u ( upperbounds );
  *val = new T[size];

  int r;
  switch( get_data_type() ) {
  case dmSHORT: DM_DESCRIPTOR_GETSUBARRAY(short, _s);
  case dmLONG: DM_DESCRIPTOR_GETSUBARRAY(long, _l);
  case dmFLOAT: DM_DESCRIPTOR_GETSUBARRAY(float, _f);
  case dmDOUBLE: DM_DESCRIPTOR_GETSUBARRAY(double, _d);
  case dmBYTE: DM_DESCRIPTOR_GETSUBARRAY(unsigned char, _ub);
  case dmUSHORT: DM_DESCRIPTOR_GETSUBARRAY(unsigned short, _us);
  case dmULONG: DM_DESCRIPTOR_GETSUBARRAY(unsigned long, _ul);
  default:
    except_unknown e;
    e.set_descr("Invalid data type in dm::descriptor::get_subarray()");
    throw e;
  }

  if( r != dmSUCCESS ) {
    except_invalid_param e;
    e.set_descr("Invalid return from dmImageDataGetSubArray_x");
    throw e;
  }
}

#undef DM_DESCRIPTOR_GETSUBARRAY

#define DM_DESCRIPTOR_SETSUBARRAY(TYPE, EXTEN) \
{\
    TYPE *x = new TYPE[size];\
    _translate_array(x, val, size);\
    r = dmImageDataSetSubArray ## EXTEN (m_descriptor, l(), u(), x);\
    delete[] x; break;\
}

template<class T> void
dm::descriptor::set_subarray(const pix_vec& lowerbounds,
			     const pix_vec& upperbounds,
			     const T* val)
{
  const int size = get_total_size(lowerbounds, upperbounds);
  arraycopy l( lowerbounds), u ( upperbounds );

  int r;
  switch( get_data_type() ) {
  case dmSHORT: DM_DESCRIPTOR_SETSUBARRAY(short, _s);
  case dmLONG: DM_DESCRIPTOR_SETSUBARRAY(long, _l);
  case dmFLOAT: DM_DESCRIPTOR_SETSUBARRAY(float, _f);
  case dmDOUBLE: DM_DESCRIPTOR_SETSUBARRAY(double, _d);
  case dmBYTE: DM_DESCRIPTOR_SETSUBARRAY(unsigned char, _ub);
  case dmUSHORT: DM_DESCRIPTOR_SETSUBARRAY(unsigned short, _us);
  case dmULONG: DM_DESCRIPTOR_SETSUBARRAY(unsigned long, _ul);
  default:
    except_unknown e;
    e.set_descr("Invalid data type in dm::descriptor::set_subarray()");
    throw e;
  }

  if( r != dmSUCCESS ) {
    except_invalid_param e;
    e.set_descr("Invalid return from dmImageDataSetSubArray_x");
    throw e;
  }
}

#undef DM_DESCRIPTOR_SETSUBARRAY

///////////////////////////////////////////////////////////////////

unsigned dm::descriptor::get_dimensionality()
{
  return dmGetArrayDim( m_descriptor );
}

void dm::descriptor::get_dimensions(pix_vec* retn)
{
  retn->clear();

  long *axeslengths = 0;

  const int no = dmGetArrayDimensions(m_descriptor, &axeslengths);

  for(int i=0; i<no; ++i)
    retn->push_back( unsigned(axeslengths[i]) );

  std::free( axeslengths );
}

/////////////////////////////////////////////////////////////////////////
// put these into the object output
#define DM_DEFINE_SET_PIXEL(TYPE) \
 template void \
  descriptor::set_pixel(const pix_vec&, TYPE);\
 template void \
  descriptor::get_pixel(const pix_vec&, TYPE*);\
 template void \
  descriptor::get_subarray(const pix_vec&, const pix_vec&, TYPE**);\
 template void \
  descriptor::set_subarray(const pix_vec&, const pix_vec&, const TYPE*);


namespace dm {
DM_DEFINE_SET_PIXEL(short)
DM_DEFINE_SET_PIXEL(long)
DM_DEFINE_SET_PIXEL(float)
DM_DEFINE_SET_PIXEL(double)
DM_DEFINE_SET_PIXEL(unsigned char)
DM_DEFINE_SET_PIXEL(unsigned short)
DM_DEFINE_SET_PIXEL(unsigned long)
  }

#undef DM_DEFINE_SET_PIXEL
