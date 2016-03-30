#ifndef DM_DESCRIPTOR_HH
#define DM_DESCRIPTOR_HH

#include <ascdm.h>
#include <vector>

#include "general.hh"

namespace dm
{

  class descriptor
  {
  public:
    ~descriptor() {};
    descriptor(dmDescriptor *d);

    // allowed T classes are: long, short, float, double, unsigned char,
    //  unsigned short, unsigned long
    template<class T> void set_pixel(const pix_vec& pos, T val);
    template<class T> void get_pixel(const pix_vec& pos, T* val);
    // get_subarray allocates the necessary space, returning *val
    template<class T> void get_subarray(const pix_vec& lowerbounds,
					const pix_vec& upperbounds,
					T** val);
    template<class T> void set_subarray(const pix_vec& lowerbounds,
					const pix_vec& upperbounds,
					const T* val);
 
    unsigned get_dimensionality();
    void get_dimensions(pix_vec* retn);

    dmDataType get_data_type() { return m_dtype; }
    dmDescriptor* get_descriptor() { return m_descriptor; }

  private:
    descriptor(const descriptor& other); // disallow copy
    descriptor& operator=(const descriptor& other);

  private:
    dmDescriptor* m_descriptor;
    dmDataType m_dtype;
  };

}

#endif
