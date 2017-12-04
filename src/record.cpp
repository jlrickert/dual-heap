#include "record.h"
#include "collection.h"

#include <iostream>

using namespace std;

Record::Record(Collection& collection, size_t offset)
  : offset(offset),
    collection(collection) {}

Record::Record(const Record& other)
  : offset(other.offset),
    collection(other.collection) {}

Field Record::get(string key_name) {
  key_pair_t key = this->collection.key_index.find(key_name)->second;
  return this->get(key);
}

Field Record::get(key_pair_t key) {
  size_t pos = this->collection.key_pos_index.find(key.first)->second;

  // Seek record
  this->collection.input->seekg(this->offset);

  size_t begin = this->offset;
  size_t end = this->offset;
  { // seek position of field within record
    char ch;
    size_t count = 0; // current field position

    while (this->collection.input->get(ch)) {
      if (ch == CSV_DELIM || ch == '\n') {
        count += 1;
        int cur = this->collection.input->tellg();
        begin = end;
        end = cur;
        if (count == (pos + 1)) {
          end -= 1;
          break;
        }
      }
    }
  }

  string raw_string;
  {  // read raw value of field
    this->collection.input->seekg(begin);
    ostringstream ss;
    char ch;
    while (this->collection.input->get(ch)) {
      if (ch == CSV_DELIM || ch == '\n') break;
      else ss << ch;
    }
    raw_string = ss.str();
  }

  { // parse field data
    switch (key.second) {
    case STRING: {
      return Field(raw_string, key);
      break;
    }
    case INTEGER: {
      stringstream ss;
      ss << raw_string;
      int x = 0;
      ss >> x;
      return Field(x, key);
      break;
    }
    default: {
      break;
    }
    }
  }
}

Field Record::operator[](string key_name) {
  return this->get(key_name);
}

Field Record::operator[](key_pair_t key) {
  return this->get(key);
}

string Record::str() {
  ostringstream ss;
  ss << "Record(";
  vector<string> keys = this->collection.keys();
  for (vector<string>::const_iterator it = keys.begin();
       it != keys.end(); ++it) {
    Field field = this->get(*it);
    ss << *it << "=" << field.str();
  }
  return ss.str();
}

bool Record::lt(Record& other, vector<string> keys) {
  for (vector<string>::iterator it = keys.begin(); it != keys.end(); ++it) {
    if (this->get(*it) < other.get(*it)) {
      return true;
    } else if (this->get(*it) == other.get(*it)) {
      continue;
    } else {
      return false;
    }
  }
  return false;
}

bool Record::lte(Record& other, vector<string> keys) {
  return !this->gt(other, keys);
}

bool Record::gt(Record& other, vector<string> keys) {
  for (vector<string>::iterator it = keys.begin(); it != keys.end(); ++it) {
    if (this->get(*it) > other.get(*it)) {
      return true;
    } else if (this->get(*it) == other.get(*it)) {
      continue;
    } else {
      return false;
    }
  }
  return false;
}

bool Record::gte(Record& other, vector<string> keys) {
  return !this->lt(other, keys);
}

bool Record::eq(Record& other, std::vector<std::string> keys) {
  for (vector<string>::iterator it = keys.begin(); it != keys.end(); ++it) {
    if (!(this->get(*it) == other.get(*it))) {
      return false;
    }
  }
  return true;
}

bool Record::neq(Record& other, std::vector<std::string> keys) {
  return !(this->eq(other, keys));
}

fstream& Record::write(fstream& stream) {
  vector<string> keys = this->collection.keys();
  for (size_t i = 0; i < keys.size(); i += 1) {
    Field field = this->get(keys[i]);
    stream << field.str();
    if (i < keys.size() - 1) {
      stream << ',';
    } else {
      stream << '\n';
    }
  }
  return stream;
}

fstream& operator<<(fstream& stream, Record& rec) {
  vector<string> keys = rec.collection.keys();
  for (size_t i = 0; i < keys.size(); i += 1) {
    stream << rec[keys[i]].str();
    if (i < keys.size() - 1) {
      stream << ',';
    } else {
      stream << '\n';
    }
  }
  return stream;
}
