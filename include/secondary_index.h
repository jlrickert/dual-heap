#ifndef SecondaryIndex_H
#define SecondaryIndex_H

#include <stdexcept>

#include "collection.h"
#include "record.h"
#include "types.h"

const size_t DEGREE = 6;
class SecondaryIndex;

typedef short int p_block_t;

enum BlockType {
  NODE,
  LEAF,
  UNALLOCATED,
};

struct Header {
  p_block_t root;
  p_block_t next_free;
};

const size_t BLOCK_KEY_LEN = 3;

struct Block {
  bool allocated;
  BlockType type;
  char keys[DEGREE][BLOCK_KEY_LEN];
  p_block_t next;
  union {
    p_block_t blocks[DEGREE];
    row_t rows[DEGREE];
  } value;
};

class SecondaryIndex {
 public:
  class KeyError : public std::runtime_error {
   public:
    KeyError(std::string item);
   private:
    std::string create_error_msg(std::string& str);
  };

  SecondaryIndex(Collection& collection, std::string key);
  ~SecondaryIndex();

  Record get(std::string key);
  void insert(size_t row);
  void remove(std::string key);
  void rebuild();
 private:
  std::string key_;
  Collection& coll_;
  std::fstream* stream_;
  Header header_;
  Block root_;

  void insert(Record& rec);
  std::fstream* open_index_file(std::string file_name);

  Header read_header();
  void update_header();
};

std::ostream& operator<<(std::ostream& stream, const Block& block);
std::istream& operator>>(std::istream& stream, Block& block);
std::ostream& operator<<(std::ostream& stream, const Header& block);
std::istream& operator>>(std::istream& stream, Header& block);

#endif
