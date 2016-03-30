#include <cstring>
#include <cstdio>
#include "general.hh"

dm::strlike::strlike(const std::string& str)
{
  m_cpy = new char[str.size() + 1];
  std::strcpy( m_cpy, str.c_str());
}

dm::arraycopy::arraycopy(const pix_vec& array)
{
  const unsigned no = array.size();
  m_copy = new long[no];
  for(unsigned i=0; i<no; ++i)
    m_copy[i] = array[i];
}

dm::arraycopy::arraycopy(unsigned no, const unsigned* list)
{
  m_copy = new long[no];
  for(unsigned i=0; i<no; ++i)
    m_copy[i] = list[i];
}

std::string dm::to_str(int i)
{
  char buf[32];
  std::sprintf(buf, "%i", i);
  return std::string(buf);
}

