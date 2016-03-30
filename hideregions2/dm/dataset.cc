#include <cstring>
#include <unistd.h>
#include <memory>
#include "dataset.hh"
#include "exception.hh"

//////////////////////////////////////////////////////////////////
// Constructors / destructors

dm::dataset::dataset(const std::string& filename, open_mode mode)
  : m_delete_on_finish(false)
{
  strlike bfr(filename);

  switch( mode ) {
  case open:
    m_dataset = dmDatasetOpen( bfr() );
    if( m_dataset == 0 ) {
      except_unable_to_open e;
      e.set_descr(std::string("Unable to open file ") + filename);
      throw e;
    }
    break;
  case openrw:
    m_dataset = dmDatasetOpenUpdate( bfr() );
    if( m_dataset == 0 ) {
      except_unable_to_open e;
      e.set_descr(std::string("Unable to open file ") + filename +
		  " (r/w access)");
      throw e;
    }
    break;
  case create_over:
    unlink(filename.c_str());
  case create:
    m_dataset = dmDatasetCreate( bfr() );
    if( m_dataset == 0 ) {
      except_unable_to_create e;
      e.set_descr(std::string("Unable to create file ") + filename);
      throw e;
    }
    break;
  }
}

dm::dataset::~dataset()
{
  if( m_delete_on_finish )
    dmDatasetDelete( m_dataset );
  else
    dmDatasetClose( m_dataset );
}


///////////////////////////////////////////////////////////////
// Block functions

int dm::dataset::get_current_block()
{
  return dmDatasetGetCurrentBlockNo(m_dataset);
}

int dm::dataset::get_no_blocks()
{
  return dmDatasetGetNoBlocks(m_dataset);
}

dm::block* dm::dataset::get_block(int blockno)
{
  return new block( _get_block(blockno) );
}

std::string dm::dataset::get_block_name(int blockno)
{
  char buffer[256];
  dmErrCode ret = dmDatasetGetBlockName(m_dataset, blockno, buffer, 255);
  if( ret != dmSUCCESS ) {
    except_invalid_param e;
    e.set_descr(std::string("Invalid block number ") +
		to_str(blockno));
    throw e;
  }
  return buffer;
}

dmBlockType dm::dataset::get_block_type(int blockno)
{
  return dmDatasetGetBlockType(m_dataset, blockno);
}

dm::block* dm::dataset::copy_block(const std::string& name,
				   const dm::block* parent,
				   bool copydata)
{
  strlike buffer(name);

  dmBlock* b = dmBlockCreateCopy(m_dataset, buffer(), parent->m_block,
				 copydata);

  if( b == 0 ) {
    except_copy_fail e;
    e.set_descr("Unable to copy block");
    throw e;
  }

  return new block(b);
}

//////////////////////////////////////////////////////////////
// Image functions

dm::image* dm::dataset::create_image(const std::string& name,
				     dmDataType datatype,
				     const std::vector<int>& axes_lengths)
{
  strlike buffer(name);

  const unsigned noaxes = axes_lengths.size();
  long* lengths = new long[noaxes];
  for(unsigned i=0; i<noaxes; ++i)
    lengths[i] = axes_lengths[i];

  dmBlock* b = dmDatasetCreateImage(m_dataset, buffer(),
				    datatype, lengths,
				    noaxes);
  delete[] lengths;

  if( b == 0 ) {
    except_block_create_fail e;
    e.set_descr("Unable to create image block");
    throw e;
  }
  return new image(b);
}

dm::image* dm::dataset::create_image(const std::string& name,
				     dmDataType datatype,
				     unsigned nlen, const int* lengths)
{
  strlike buffer(name);

  long* tlengths = new long[nlen];
  for(unsigned i=0; i<nlen; ++i)
    tlengths[i] = lengths[i];

  dmBlock* b = dmDatasetCreateImage(m_dataset, buffer(),
				    datatype, tlengths, nlen);

  delete[] tlengths;

  if( b == 0 ) {
    except_block_create_fail e;
    e.set_descr("Unable to create image block");
    throw e;
  }
  return new image(b);
}

dm::image* dm::dataset::create_image(const std::string& name,
				     dmDataType datatype,
				     int xw, int yw)
{
  int l[2] = {xw, yw};
  return create_image(name, datatype, 2, l);
}

dm::image* dm::dataset::get_image(int blockno)
{
  if( get_block_type(blockno) != dmIMAGE ) {
    except_invalid_param e;
    e.set_descr("move_to_image: block is not an image block");
    throw e;
  }

  dmBlock* b = _get_block(blockno);

  return new image(b);
}

///////////////////////////////////////////////////////////
// private functions

dmBlock* dm::dataset::_get_block(int blockno)
{
  dmBlock* b = dmDatasetMoveToBlock(m_dataset, blockno);
  if( b == 0 ) {
    except_invalid_param e;
    e.set_descr(std::string("Invalid block number ") +
		to_str(blockno));
    throw e;
  }

  return b;
}

///////////////////////////////////////////////////////////
// Static functions

void dm::dataset::delete_on_disk(const std::string& filename)
{
  strlike bfr(filename);
  dmDatasetDestroy( bfr() );
}
