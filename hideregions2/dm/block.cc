#include <cassert>
#include "block.hh"

dm::block::~block()
{
  if ( m_block != 0 )
    dmBlockClose(m_block);
}

bool dm::block::read_key(const std::string& name, double* ret)
{
  assert( m_block != 0 );

  dmDescriptor* desc = dmKeyRead_d(m_block, const_cast<char*>(name.c_str()),
				   ret);

  return( desc != 0 );
}
