#ifndef DM_EXCEPTION_HH
#define DM_EXCEPTION_HH

#include <string>

namespace dm
{
  class exception
  {
  public:
    exception() {}
    void set_descr(const std::string& t) { m_descr = t; }
    const std::string& operator() () { return m_descr; }
  private:
    std::string m_descr;
  };

  class except_unable_to_open : public exception {};
  class except_unable_to_create : public exception {};
  class except_invalid_param : public exception {};
  class except_copy_fail : public exception {};
  class except_block_create_fail : public exception {};

  class except_unknown : public exception {};
}

#endif
