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
      stream_(this->open_index_file(coll.file_name())),
      header_(read_header()) {
  this->update_header();
}

SecondaryIndex::~SecondaryIndex() { delete this->stream_; }

void SecondaryIndex::rebuild() {
  *this->stream_ << this->header_ << endl;
}

Header SecondaryIndex::read_header() {
  this->stream_->seekg(0);
  Header h;
  if (this->stream_->peek() != EOF) {
    *this->stream_ >> h;
    return h;
  } else {
    h.next_free = sizeof(Header) + sizeof(Block);
    h.root = sizeof(Header);
    return h;
  }
}

void SecondaryIndex::update_header() {
  this->stream_->seekg(0);
  *this->stream_ >> this->header_;
}

fstream* SecondaryIndex::open_index_file(string collection_file_name) {
  fstream* stream = new fstream();
  stream->open("tmp/key_index", fstream::binary | fstream::out);  // this needs to be both read and write
  *stream << "wassup";
  cout << stream->good() << endl;
  return stream;
}

ostream& operator<<(ostream& stream, const Block& block) {
  stream << block.allocated;
  stream << block.type;
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
  stream >> block;
  return stream;
}

ostream& operator<<(ostream& stream, const Header& header) {
  stream << header.root;
  stream << header.next_free;
  return stream;
}

istream& operator>>(istream& stream, Header& header) {
  stream >> header.root;
  stream >> header.next_free;
  return stream;
}
