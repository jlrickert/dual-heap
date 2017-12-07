#ifndef SecondaryIndex_H
#define SecondaryIndex_H

#include <stdexcept>

#include "collection.h"
#include "record.h"
#include "types.h"

class SecondaryIndex {
 public:
  static const size_t DEGREE = 6;

  class KeyError : public std::runtime_error {
   public:
    KeyError(std::string item);
   private:
    std::string create_error_msg(std::string& str);
  };

  /// build secondary index from a previously cached value;
  static SecondaryIndex from_cache(std::fstream& col);

  SecondaryIndex(Collection& collection, std::string key);
  ~SecondaryIndex();

  void insert(size_t row);
  void remove(std::string key);
  std::fstream& cache(std::fstream& stream);

 private:
  typedef short int p_block_t;
  enum BlockType {
    NODE,
    DATA,
    UNALLOCATED,
  };

  struct Block {
    bool allocated;
    BlockType type;
    char* keys[SecondaryIndex::DEGREE];
    p_block_t next;
    union {
      p_block_t blocks[DEGREE];
      row_t rows[DEGREE];
    } value;
  };

  struct Header {
    p_block_t root;
    p_block_t next_free;
  };

  std::string key_;
  Collection& coll_;
  std::fstream* stream_;
  Header header_;
  Block root_;

  void insert(Record& rec);
  std::fstream* open_index_file(std::string file_name);
};

#endif
