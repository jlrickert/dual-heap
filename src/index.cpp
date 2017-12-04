#include <sstream>

#include "types.h"
#include "index.h"
#include "utilities.h"
#include "types.h"


Index* Index::from_csv(std::string file_name) {
  std::string output_file_name;
  { // create temporary file name
    std::stringstream ss;
    ss << "tmp/index";
    output_file_name = ss.str();
  }

  std::fstream input, output;
  output.open(output_file_name.c_str(),
              output.binary | output.trunc | output.out);
  input.open(file_name.c_str(), input.in);

  size_t key_count = 0;
  { // get the number of keys
    input.seekg(0);
    char ch;
    while (input.get(ch) && ch != '\n') {
      if (ch == CSV_DELIM) {
        key_count += 1;
      }
    }
    key_count += 1;
  }

  // save a spot for size;
  output.seekp(sizeof(size_t));

  size_t size = 0;  // keep track of count of records
  { // fill output with offsets
    char ch;
    size_t cur = input.tellg();
    size_t count = 0;

    while (input.get(ch)) {
      if (ch == CSV_DELIM) {
        count += 1;
      }
      if (ch == '\n' && count >= key_count - 1) {
        size += 1;
        Util::write_raw<size_t>(output, cur);
        cur = input.tellg();
        count = 0;
      }
    }
  }

  { // write number of records at first byte
    output.seekp(0);
    Util::write_raw<size_t>(output, size);
  }

  output.sync();
  return new Index(output_file_name);
}

Index::Index(std::string file_name) : file_name(file_name) {
  stream.open(file_name.c_str(), stream.binary | stream.in);

  { // get record count from beginning of file
    size_t size = 0;
    this->stream.seekg(0);
    Util::read_raw<size_t>(this->stream, size);
    this->size_ = size;
  }
};

Index::~Index() {
  if (this->stream.is_open()) {
    this->stream.close();
  }
}

size_t Index::get(size_t i) {
  int count_offset = sizeof(size_t);
  this->stream.seekg(i * sizeof(size_t) + count_offset);
  size_t offset;
  Util::read_raw<size_t>(this->stream, offset);
  return offset;
}

size_t Index::operator[](size_t i) {
  return this->get(i);
}

size_t Index::size() const {
  return this->size_;
}
