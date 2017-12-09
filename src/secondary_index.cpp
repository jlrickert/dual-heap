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

SecondaryIndex::SecondaryIndex(Collection& coll, string key)
    : key_(key),
      coll_(coll),
      stream_(this->open_index_file(key)),
      header_(read_header()) {}

SecondaryIndex::~SecondaryIndex() { delete this->stream_; }

void SecondaryIndex::rebuild() {
  // *this->stream_ << this->header_ << endl;
  this->update_header(this->header_);
}

Header SecondaryIndex::read_header() {
  this->stream_->seekg(0);
  Header h;
  if (this->stream_->peek() != EOF) {
    cout << "Reading header for \"" << this->key_ << "\" index" << endl;
    *this->stream_ >> h;
  } else {
    this->stream_->seekg(0);
    cout << "Creating header for \"" << this->key_ << "\" index" << endl;
    h.next_free = sizeof(Header) + sizeof(Block);
    h.root = sizeof(Header);
    this->update_header(h);
  }
  return h;
}

void SecondaryIndex::update_header(Header header) {
  this->stream_->seekg(0);
  *this->stream_ << header;
  this->stream_->flush();
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
    cout << "DEBUG: Created new " << key_name << " index" << endl << flush;
  }
  return stream;
}

ostream& operator<<(ostream& stream, const Block& block) {
  stream << block.allocated;
  stream << (char)block.type;
  for (size_t i = 0; i < DEGREE; i += 1) {
    for (size_t j = 0; j < BLOCK_KEY_LEN; j += 1) {
      stream << block.keys[i][j];
    }
  }
  stream << block.next;
  switch (block.type) {
    case NODE: {
      break;
    }
    case LEAF: {
      break;
    }
    case UNALLOCATED: {
    }
  }
  return stream;
}

istream& operator>>(istream& stream, Block& block) {
  stream >> block.allocated;
  char b_type;
  stream >> b_type;
  block.type = (BlockType)b_type;
  for (size_t i = 0; i < DEGREE; i += 1) {
    for (size_t j = 0; j < DEGREE; j += 1) {
      stream >> block.keys[i][j];
    }
  }
  stream >> block.next;
  for (size_t i = 0; i < DEGREE; i += 1) {
    if (block.type == NODE) {
      stream >> block.value.blocks[i];
    } else if (block.type == LEAF) {
      stream >> block.value.rows[i];
    }
  }
  return stream;
}

ostream& operator<<(ostream& stream, const Header& header) {
  Util::write_raw<p_block_t>(stream, header.root);
  // stream << header.root;
  Util::write_raw<p_block_t>(stream, header.next_free);
  // stream << header.next_free;
  return stream;
}

istream& operator>>(istream& stream, Header& header) {
  Util::read_raw<p_block_t>(stream, header.root);
  Util::read_raw<p_block_t>(stream, header.next_free);
  return stream;
}
