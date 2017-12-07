#include <fstream>
#include <sstream>

#include "secondary_index.h"

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
    : key_(key), coll_(coll), stream_(this->open_index_file(coll.file_name())) {
  root_ = Block();
  // root_type_ = LEAF;
}

SecondaryIndex::~SecondaryIndex() { delete this->stream_; }

fstream* SecondaryIndex::open_index_file(string file_name) {
  fstream* stream = new fstream();
  this->stream_->open(file_name.c_str());
  return stream;
}
