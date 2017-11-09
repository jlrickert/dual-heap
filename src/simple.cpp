#include <iostream>
#include <fstream>
#include <string>
#include "simple.h"

Record::Key parse_key(std::string str);

Record::Record(const std::string& file_name)
  : file_name(file_name.c_str()) {
  this->init_fstream();
  this->init_keys();
  this->init_index();
};

Record::~Record() {
  file.close();
}


size_t Record::row_count() {
  return 0;
}

std::vector<std::string> Record::keys() const {
  std::vector<std::string> keys;
  for (std::vector<Key>::const_iterator it = this->keys_.begin(); it != this->keys_.end(); ++it) {
    keys.push_back(it->first);
  }
  return keys;
}

void Record::init_fstream() {
  this->file.open(this->file_name);
  if (!file.good()) {
    throw FILE_DOESNT_EXIST_ERROR;
  }
}

void Record::init_keys() {
  std::string line;
  getline(file, line);

  int cp = 0; // alias to checkpoint
  for (size_t i = 0; i < line.length(); i += 1) {
    bool is_delim = line[i] == HEADER_DELIM;
    bool is_end = i == (line.length() - 1);
    if (is_delim || is_end) {
      int pos = cp;
      int offset = i - cp + (is_end ? 1 : 0);
      std::string raw_key = line.substr(pos, offset);
      Record::Key key = parse_key(raw_key);
      this->keys_.push_back(key);
      cp = i + 1;
    };
  }
}

void Record::init_index() {
  file.seekg(0);
  char ch;

  file.get(ch);

  // skip header
  while (file.get(ch) && ch != '\n');

  do {
    if (ch == '\n') {
      size_t pos = file.tellg();
      this->index.push_back(pos);
    }
  } while (file.get(ch));
}

Record::Key parse_key(std::string str) {
  for (size_t i = 0; i < str.length() - 1; i += 1) {
    if (str[i] == ':') {
      std::string key;
      FieldType type;
      key = str.substr(0, i);
      str = str.substr(i + 1);
      if (str == "str" || str == "string") {
        type = STRING;
      } else if (str == "int" || str == "integer") {
        type = INTEGER;
      } else {
        throw INVALID_FIELD_TYPE;
      }
      return Record::Key(key, type);
    }
  }
  throw PARSE_ERROR;
}
