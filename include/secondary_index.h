#ifndef SecondaryIndex_H
#define SecondaryIndex_H

#include <stdexcept>

#include "collection.h"
#include "record.h"
#include "types.h"

const size_t DEGREE = 6;
class SecondaryIndex;

typedef unsigned short p_block_t;
typedef char meta_t;

enum BlockType {
  NODE,
  LEAF,
  UNALLOCATED,
};

struct Header {
  p_block_t root;
  p_block_t next_free;
};

const size_t BLOCK_KEY_LEN = 10;

class Block {
 public:
  static Block new_node(std::vector<std::string> keys, p_block_t parent,
                        std::vector<p_block_t> blocks);
  static Block new_leaf(std::vector<std::string> keys, std::vector<row_t> rows,
                        p_block_t parent, p_block_t next);
  static Block new_unallocated(p_block_t next);

  Block& operator=(const Block& other);

  meta_t meta;
  p_block_t parent;
  p_block_t next;
  char items;
  std::string keys[DEGREE];
  union {
    p_block_t blocks[DEGREE];
    row_t rows[DEGREE];
  } value;

  BlockType type() const;
  bool allocated() const;

 private:
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

  Record get(std::string key) throw();
  void insert(Record rec);
  void remove(std::string key);
  Header rebuild();

 private:
  std::string key_;
  Collection& coll_;
  std::fstream* stream_;
  Header header_;
  Block root_;

  std::fstream* open_index_file(std::string key_name);
  Block traverse(Block root, std::string key);

  Header read_header();
  void update_header();
  Block read_block(p_block_t offset);
  p_block_t update_block(p_block_t offset, const Block& block);
};

std::ostream& operator<<(std::ostream& stream, const Block& block);
std::ostream& operator<<(std::ostream& stream, const BlockType& type);
std::ostream& operator<<(std::ostream& stream, const Header& block);

#endif
