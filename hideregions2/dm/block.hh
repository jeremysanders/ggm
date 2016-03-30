#ifndef DM_BLOCK_HH
#define DM_BLOCK_HH

#include <string>
#include <ascdm.h>

namespace dm {

  class block
  {
  public:
    virtual ~block();

    // read a double key with name, return false if not found
    bool read_key(const std::string& name, double* ret);

  protected:
    block(dmBlock *init) { m_block = init; }

    friend class dataset;

  private:
    block(const block& other);  // disallow copy usage
    block& operator=(const block& other); // disallow =

  protected:
    dmBlock* m_block;
  };


}

#endif
