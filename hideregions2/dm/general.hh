#ifndef DM_GENERAL_HH
#define DM_GENERAL_HH

#include <string>
#include <vector>

namespace dm
{
  // method to use when opening file
  //  create won't overwrite new files, create_over will
  enum open_mode { open, openrw, create, create_over };

  typedef std::vector<unsigned> pix_vec;

  // little class to make copies of strings
  // acts like a kind of 'smart-pointer'
  // needed because DM routines use char* rather than const char*
  class strlike 
  {
  public:
    strlike(const std::string& str);
    ~strlike() { delete[] m_cpy; }
    char* operator() () { return m_cpy; }
  private:
    char* m_cpy;
  };

  // arraycopy
  // class to make a copy of a vector or unsigned* array
  class arraycopy
  {
  public:
    arraycopy(const pix_vec& array);
    arraycopy(unsigned no, const unsigned* list);
    ~arraycopy() { delete[] m_copy; }

    long* operator() () { return m_copy; }
  private:
    long* m_copy;
  };

  std::string to_str(int i);
}

#endif
