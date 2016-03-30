#ifndef DM_DATASET_HH
#define DM_DATASET_HH

#include <string>
#include <vector>
#include <ascdm.h>

#include "general.hh"
#include "block.hh"
#include "image.hh"

namespace dm
{

  class dataset
  {
  public:
    // open the dataset (or create)
    dataset(const std::string& filename, open_mode mode = open);
    // close the dataset
    ~dataset();

    // BLOCK FUNCTIONS
    //////////////////
    // get current block no.
    int get_current_block();
    // get number of blocks
    int get_no_blocks();
    // move to block
    block* get_block(int blockno);
    // get the name of the numbered block
    std::string get_block_name(int blockno);
    // get type of the block
    dmBlockType get_block_type(int blockno);
    // copy block
    block* copy_block(const std::string& name, const block* parent,
		      bool copydata);

    // IMAGE FUNCTIONS
    //////////////////
    image* create_image(const std::string& name,
			dmDataType datatype,
			const std::vector<int>& axes_lengths);
    image* create_image(const std::string& name,
			dmDataType datatype,
			unsigned nlen, const int* lengths);
    image* create_image(const std::string& name,
			dmDataType datatype, int xw, int yw);
    image* get_image(int block_no = 1);

    // delete the dataset when we close (default false)
    void delete_when_finished(bool b=true) 
     { m_delete_on_finish = b; }

  public:
    // static functions
    static void delete_on_disk(const std::string& filename);

  private:
    dataset(const dataset& other); // disallow copy
    dataset& operator=(const dataset& other); // disallow =

    dmBlock* _get_block(int blockno);

  private:
    dmDataset* m_dataset;
    bool m_delete_on_finish;
  };



}


#endif
