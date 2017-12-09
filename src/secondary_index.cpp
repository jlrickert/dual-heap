#include <fstream>
#include <iostream>
#include <sstream>

#include "secondary_index.h"
#include "utilities.h"

using namespace std;

SecondaryIndex::KeyError::KeyError(string key)
    : runtime_error(create_error_msg(key)) {}

string SecondaryIndex::KeyError::create_error_msg(string& key) {
  std::ostringstream ss;
  ss << "No entry for key "
     << "\"" << key << "\""
     << " found";
  return ss.str().c_str();
}

Block Block::new_node(vector<string> keys, p_block_t parent,
                      vector<p_block_t> blocks) {
  Block block;
  block.meta = 0x0;
  block.meta |= 1 << (sizeof(meta_t) * 8 - 1);  // set most significant bit to 1
  block.meta |= NODE;

  block.parent = parent;
  block.next = -1;
  block.items = (char)keys.size();
  for (size_t i = 0; i < DEGREE; i += 1) {
    for (size_t j = 0; j < BLOCK_KEY_LEN; j += 1) {
      if (i < keys.size() && j < keys[i].size()) {
        block.keys[i][j] = keys[i][j];
      } else {
        block.keys[i][j] = ' ';
      }
    }

    if (i < blocks.size()) {
      block.value.blocks[i] = blocks[i];
    } else {
      block.value.blocks[i] = -1;
    }
  }

  return block;
}

Block Block::new_leaf(vector<string> keys, vector<row_t> rows, p_block_t parent,
                      p_block_t next) {
  Block block;
  block.meta = 0x0;
  block.meta |= 1 << (sizeof(meta_t) * 8 - 1);  // set most significant bit to 1
  block.meta |= LEAF;

  block.parent = parent;
  block.next = next;
  block.items = (char)keys.size();

  for (size_t i = 0; i < DEGREE; i += 1) {
    // write out values to keys
    for (size_t j = 0; j < BLOCK_KEY_LEN; j += 1) {
      if (i < keys.size() && j < keys[i].size()) {
        block.keys[i][j] = keys[i][j];
      } else {
        block.keys[i][j] = ' ';
      }
    }

    // write out value to rows
    if (i < rows.size()) {
      block.value.rows[i] = rows[i];
    } else {
      block.value.rows[i] = -1;
    }
  }

  return block;
}

Block Block::new_unallocated(p_block_t next) {
  Block block;
  block.meta = 0x0;
  block.meta |= UNALLOCATED;

  block.next = next;
  return block;
}

Block& Block::operator=(const Block& other) {
  if (this != &other) {
    this->meta = other.meta;
    this->next = other.next;
    this->items = other.items;
    for (size_t i = 0; i < (size_t)other.items; i += 1) {
      this->keys[i] = other.keys[i];
      if (other.type() == LEAF) {
        this->value.rows[i] = other.value.rows[i];
      } else {
        this->value.blocks[i] = other.value.blocks[i];
      }
    }
  }
  return *this;
}

BlockType Block::type() const { return (BlockType)(this->meta & 0xF); }

bool Block::allocated() const {
  size_t bit_position = 1 << ((sizeof(meta_t)) * 8 - 1);
  return (this->meta & bit_position);
}

SecondaryIndex::SecondaryIndex(Collection& coll, string key)
    : key_(key),
      coll_(coll),
      stream_(this->open_index_file(key)),
      header_(read_header()) {}

SecondaryIndex::~SecondaryIndex() { delete this->stream_; }

Record SecondaryIndex::get(string key) throw() {
  Block leaf = this->traverse(this->root_, key);

  bool ok = true;
  while (ok) {
    for (size_t i = 0; i < (size_t)leaf.items; i += 1) {
      if (leaf.keys[i] == key) {
        continue;
      } else if (leaf.keys[i] == key) {
        row_t row = leaf.value.rows[i];
        return this->coll_.get(row);
      } else {
        ok = false;
        break;
      }
    }
    if (leaf.next != (typeof(leaf.next))(-1)) {
      leaf = read_block(leaf.next);
    } else {
      ok = false;
    }
  }
  throw KeyError(key);
}

void SecondaryIndex::insert(Record rec) {
  row_t row = rec.row();
  string key = rec.get(this->key_).str();

  cout << "Inserting record " << row << " with key " << key << " into \""
       << this->key_ << "\" index" << endl;

  Block leaf = this->traverse(this->root_, key_);
  for (size_t i = 0; i < DEGREE; i += 1) {
    if (i < (size_t)leaf.items && key < leaf.keys[i]) {
      if (i < (size_t)leaf.items) {
        // insert between node
      } else if (i == (DEGREE - 1)) {
        // need to split
      } else {
        string tmp = leaf.keys[i];
        leaf.keys[i] = key;
        leaf.keys[i + 1] = tmp;
      }
    }
  }
}

void SecondaryIndex::remove(string key) {}

Header SecondaryIndex::rebuild() {
  cout << "Rebuilding index" << endl;

  this->stream_->seekg(0);

  cout << "Creating header for \"" << this->key_ << "\" index" << endl;
  this->header_.root = sizeof(Header);

  cout << "DEBUG: Creating new root node" << endl;
  vector<string> keys;
  vector<row_t> rows;
  this->root_ = Block::new_leaf(keys, rows, -1, -1);
  p_block_t next = update_block(this->header_.root, this->root_);
  this->header_.next_free = next;
  this->update_header();

  // for (row_t i = 0; i < this->coll_.size(); i += 1) {
  //   Record rec = this->coll_.get(i);
  //   this->insert(rec);
  // }

  return this->header_;
}

Header SecondaryIndex::read_header() {
  Header h;
  if (this->stream_->peek() != EOF) {
    cout << "Reading header for \"" << this->key_ << "\" index" << endl;
    this->stream_->seekg(0);
    Util::read_raw<p_block_t>(*this->stream_, h.root);
    Util::read_raw<p_block_t>(*this->stream_, h.next_free);
  } else {
    cout << "DEBUG: No header found" << endl;
    this->rebuild();
    h = this->header_;
  }
  return h;
}

void SecondaryIndex::update_header() {
  cout << "DEBUG: Writing " << this->header_ << endl;
  this->stream_->seekp(0);
  Util::write_raw<p_block_t>(*this->stream_, this->header_.root);
  Util::write_raw<p_block_t>(*this->stream_, this->header_.next_free);
  this->stream_->flush();
}

Block SecondaryIndex::read_block(p_block_t offset) {
  cout << "DEBUG: Reading Block at " << offset << endl;

  Block block;
  this->stream_->seekg(offset);
  Util::read_raw<char>(*this->stream_, block.meta);
  BlockType type = (BlockType)(block.meta & 0xF);
  Util::read_raw<p_block_t>(*this->stream_, block.parent);
  Util::read_raw<p_block_t>(*this->stream_, block.next);
  Util::read_raw<char>(*this->stream_, block.items);
  for (size_t i = 0; i < DEGREE; i += 1) {
    if (sizeof(p_block_t) < sizeof(row_t)) {
      if (type == NODE) {
        row_t item;
        Util::read_raw<row_t>(*this->stream_, item);
        block.value.blocks[i] = item;
      } else {
        p_block_t item;
        Util::read_raw<p_block_t>(*this->stream_, item);
        block.value.blocks[i] = item;
      }
    }
  }
  this->stream_->flush();
  return block;
}

p_block_t SecondaryIndex::update_block(p_block_t offset, const Block& block) {
  BlockType type = (BlockType)(block.meta & 0xF);

  this->stream_->seekp(offset);
  Util::write_raw<char>(*this->stream_, block.meta);
  Util::write_raw<p_block_t>(*this->stream_, block.parent);
  Util::write_raw<p_block_t>(*this->stream_, block.next);
  Util::write_raw<char>(*this->stream_, block.items);
  for (size_t i = 0; i < DEGREE; i += 1) {
    if (sizeof(p_block_t) < sizeof(row_t)) {
      if (type == NODE) {
        Util::write_raw<row_t>(*this->stream_, (row_t)block.value.blocks[i]);
      } else {
        Util::write_raw<p_block_t>(*this->stream_,
                                   (p_block_t)block.value.blocks[i]);
      }
    }
  }
  this->stream_->flush();
  return this->stream_->tellp();
}

fstream* SecondaryIndex::open_index_file(string key_name) {
  string index_file_name;
  {
    ostringstream ss;
    ss << "tmp/index_";
    for (size_t i = 0; i < key_name.size(); i += 1) {
      if (key_name[i] != ' ') {
        ss << key_name[i];
      }
    }
    index_file_name = ss.str();
  }

  fstream* stream = new fstream();
  stream->exceptions(fstream::badbit | fstream::failbit);
  try {
    stream->open(index_file_name.c_str(),
                 fstream::binary | fstream::in | fstream::out);
    cout << "Opened \"" << key_name << "\" index" << endl << flush;
  } catch (fstream::failure e) {
    stream->open(index_file_name.c_str(),
                 fstream::binary | fstream::in | fstream::out | fstream::trunc);
    cout << "Creating new " << key_name << " index" << endl << flush;
  }
  return stream;
}

Block SecondaryIndex::traverse(Block root, std::string key) {
  if (root.type() == LEAF) {
    return root;
  }
  for (size_t i = 0; i < (size_t)root.items; i += 1) {
    if (key <= root.keys[i]) {
      p_block_t offset = root.value.blocks[i];
      return read_block(offset);
    }
  }
  throw runtime_error("A node must have a value");
}

ostream& operator<<(ostream& stream, const BlockType& type) {
  switch (type) {
    case NODE: {
      stream << "NODE";
      break;
    }
    case LEAF: {
      stream << "LEAF";
      break;
    }
    case UNALLOCATED: {
      stream << "UNALLOCATED";
      break;
    }
  }
  return stream;
}

ostream& operator<<(ostream& stream, const Block& block) {
  BlockType type = (BlockType)(block.meta & 0xF);
  stream << "Block(";
  stream << "block_type=" << type << ", ";

  if (type != NODE) {
    stream << "next_block=";
    if (block.next == (typeof(block.next))(-1)) {
      stream << "nil";
    } else {
      stream << block.next;
    }
    stream << ", ";
  }

  {  // write out keys keys
    stream << "keys=[";
    for (size_t i = 0; i < (size_t)block.items; i += 1) {
      for (size_t j = 0; j < BLOCK_KEY_LEN; j += 1) {
        stream << block.keys[i][j];
      }
      if (i < (size_t)block.items - 1) {
        stream << ", ";
      }
    }
    stream << "] ";
  }

  {  // write out block specific values
    if (type != UNALLOCATED) {
      if (type == LEAF) {
        stream << "rows=[";
      } else {
        stream << "blocks=[";
      }
      if (size_t(block.items) != 0) {
        for (size_t i = 0; i < (size_t)block.items; i += 1) {
          if (type == LEAF) {
            row_t row = block.value.rows[i];
            stream << row;
          } else {
            p_block_t b = block.value.blocks[i];
            stream << b;
          }
          if (i < (size_t)block.items - 1) {
            stream << ", ";
          }
        }
      }
      stream << "]";
    }
  }
  stream << ")";

  return stream;
}

ostream& operator<<(ostream& stream, const Header& header) {
  stream << "Header(";
  stream << "root=" << header.root << ", ";
  stream << "next_free=" << header.next_free << ")";
  return stream;
}
