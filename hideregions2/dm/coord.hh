#ifndef DM_COORD_HH
#define DM_COORD_HH

#include <ascdm.h>

namespace dm
{

  class coord
  {
  public:
  protected:
    coord(dmDescriptor* descr);
    ~coord();

  private:
    dmDescriptor* _descr;
  };


}

#endif
