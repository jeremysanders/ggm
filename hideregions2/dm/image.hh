#ifndef DM_IMAGE_HH
#define DM_IMAGE_HH

#include "block.hh"
#include "descriptor.hh"
#include "memimage.hh"

namespace dm
{
  class image : public block, public descriptor
  {
  public:
    virtual ~image();

    // create an in-memory image from the file
    template<class T> void create_memimage(memimage<T> **im);
    // write memory image to disk (note - must be same size!)
    template<class T> void write_from_memimage(const memimage<T>& im);

  protected:
    image(dmBlock *init);  // protected constructor
    
    friend class dataset;

  private:
    image(const image& other);  // disallow copy usage
    image& operator=(const image& other); // disallow =

  private:
    dmDescriptor* m_descriptor;
  };


}


#endif
