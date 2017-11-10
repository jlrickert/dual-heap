#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "simple.h"

Key parse_key(std::string raw_str);

std::string Key::type() const {
  switch (this->t) {
  case INTEGER: {
    return "INTEGER";
    break;
  }
  case STRING: {
    return "STRING";
    break;
  }
  }
};

std::string Field::str() {
  switch (this->type) {
  case INTEGER: {
    std::string result;
    std::ostringstream convert;
    convert << this->data.integer;
    return convert.str();
    break;
  }
  case STRING: {
    return std::string(this->data.string);
    break;
  }
  }
}

Record::Row::Row(const Record& rec, size_t offset)
  : rec(rec),
    offset(offset) {}

Field Record::Row::fetch(Key key) const {
  std::cout << "rawr";
  // size_t pos = rec.keys_index.find(key);
  // std::cout << pos;
}

Field Record::Row::fetch(std::string key_name) const {
  Key key = rec.lookup_key(key_name);
  return this->fetch(key);
}

Record::Record(const std::string& file_name)
  : file_name(file_name) {
  this->init_fstream();
  this->init_keys();
  this->init_index();
};

Record::~Record() {
  std::cout << "File " << this->file_name << " closed." << std::endl;
  file.close();
}

Record::Row Record::fetch(int id) const {
  return Row(*this, this->index[id]);
}

size_t Record::row_count() const {
  return this->index.size();
}

std::vector<Key> Record::keys() const {
  std::vector<Key> keys;
  for (std::vector<Key>::const_iterator it = this->keys_.begin();
       it != this->keys_.end(); ++it) {
    keys.push_back(*it);
  }
  return keys;
}

Key Record::lookup_key(std::string key_name) const {
  for (size_t i = 0; i < keys_.size(); i += 1) {
    if (key_name == keys_[i].name) {
      return keys_[i];
    }
  }
  throw KEY_ERROR;
}

void Record::init_fstream() {
  this->file.open(this->file_name.c_str());
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
      Key key = parse_key(raw_key);
      this->keys_index.insert(std::pair<Key, size_t>(key, this->keys_.size()));
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
      // this->index.push_back(pos);
      this->rows.push_back(Record::Row(*this, pos));
    }
  } while (file.get(ch));
}

Key parse_key(std::string str) {
  for (size_t i = 0; i < str.length() - 1; i += 1) {
    if (str[i] == ':') {
      std::string key;
      FieldType type;
      key = str.substr(0, i);
      str = str.substr(i + 1);
      if (str == "str" || str == "STR" || str == "string" || str == "STRING") {
        type = STRING;
      } else if (str == "int" || str == "INT" || str == "integer" || "INTEGER") {
        type = INTEGER;
      } else {
        throw INVALID_FIELD_TYPE;
      }
      return Key(key, type);
    }
  }
  throw PARSE_ERROR;
}
