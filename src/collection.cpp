#include <algorithm>
#include <stdexcept>
#include <sstream>
#include "collection.h"

////////////////////////////////////////////////////////////////////////////////
// Collection class implementation section
////////////////////////////////////////////////////////////////////////////////
Key parse_key(std::string raw, int pos);

Collection::Collection(const std::string& file_name) : file_name(file_name) {
  this->init_file();
  this->init_keys();
  this->init_index();
}

Collection::~Collection() {
  delete this->index;
  if (input.is_open()) {
    this->input.close();
  }
}

void Collection::init_file() {
  input.open(file_name.c_str());
  if (!input.is_open()) {
    std::stringstream ss;
    ss << "File " << file_name.c_str() << " doesn't exist";
    throw std::runtime_error(ss.str());
  }
}

void Collection::init_keys() {
  std::string line;
  getline(input, line);

  int key_count = 0;
  int cp = 0;  // alias to checkpoint
  for (size_t i = 0; i < line.length(); i += 1) {
    bool is_delim = line[i] == DELIM;
    bool is_end = i == (line.length() - 1);
    if (is_delim || is_end) {
      int pos = cp;
      int offset = i - cp + (is_end ? 1 : 0);
      std::string raw_key = line.substr(pos, offset);
      key_count += 1;
      Key key = parse_key(raw_key, key_count);
      this->keys_.insert(key_pair_t(key.name, key));
      cp = i + 1;
    }
  }
}

void Collection::init_index() {
  this->index = Index::from_csv(this->file_name);
}

Collection::Record Collection::operator[](size_t row) {
  // if (!(0 < row && row <= this->size())) {
  if ( row >= this->size()) {
    std::stringstream ss;
    ss << "Record " << row << " doesn't exist.";
    throw std::out_of_range(ss.str());
  }
  size_t rrn = this->index->get(row);
  return Collection::Record(*this, rrn);
}

size_t Collection::size() const {
  return this->index->size();
}

size_t Collection::cur_pos() {
  return this->input.tellg();
}

void Collection::seek(size_t pos) {
  this->input.seekg(pos);
}

Key parse_key(std::string raw, int pos) {
  for (size_t i = 0; i < raw.length() - 1; i += 1) {
    if (raw[i] == ':') {
      std::string key_name = raw.substr(0, i);
      std::string type_string = raw.substr(i + 1);

      // caps don't matter
      std::transform(type_string.begin(), type_string.end(),
                     type_string.begin(), ::toupper);

      Key::Type type = Key::STRING;
      if (type_string == "STR" || type_string == "STRING") {
        type = Key::STRING;
      } else if (type_string == "INT" || type_string == "INTEGER") {
        type = Key::INTEGER;
      }

      return Key(pos, key_name, type);
    }
  }
  std::stringstream ss;
  ss << "\"" << raw << "\"" << " is not a valid syntax";
  throw std::runtime_error(ss.str());
}

////////////////////////////////////////////////////////////////////////////////
// Record class implementation section
////////////////////////////////////////////////////////////////////////////////
Collection::Record::Record(Collection& collection, size_t offset)
  : offset(offset),
    collection(collection) {}

Field Collection::Record::get(std::string key_name) {
  Key key = this->collection.keys_.find(key_name)->second;
  return Collection::Record::get(key);
}

Field Collection::Record::get(Key key) {
  { // Seek record
    this->collection.seek(this->offset);
  }

  size_t begin = this->offset;
  size_t end = this->offset;
  { // seek position of field within record
    char ch;
    size_t count = 0;

    while (this->collection.input.get(ch)) {
      if (ch == DELIM || ch == '\n') {
        count += 1;
        int pos = this->collection.input.tellg();

        begin = end;
        end = pos;
        if (count == key.pos) {
          end -= 1;
          break;
        }
      }
    }
  }
  // std::cout << begin << " " << end << std::endl;

  std::string raw_string;
  {  // read raw value of field
    this->collection.seek(begin);
    std::ostringstream ss;
    char ch;
    while (this->collection.input.get(ch)) {
      if (ch == DELIM || ch == '\n') break;
      else ss << ch;
    }
    raw_string = ss.str();
  }
  // std::cout << raw_string << std::endl;

  Field::Data data;
  {
    switch (key.type) {
    case Key::STRING: {
      // char* name = name = raw_string.c_str();
      std::string* str = new std::string(raw_string.c_str());
      Field::Data d = { .string=str };
      data = d;
      break;
    }
    case Key::INTEGER:
      std::stringstream ss;
      ss << raw_string;
      int x = 0;
      ss >> x;
      Field::Data d = {.integer=x};
      data = d;
      break;
    }
  }
  Field field(data, key);
  return field;
}

Field Collection::Record::operator[](std::string key_name) {
  return this->get(key_name);
}

Field Collection::Record::operator[](Key key) {
  return this->get(key);
}

////////////////////////////////////////////////////////////////////////////////
// Field class implementation section
////////////////////////////////////////////////////////////////////////////////
std::string Field::str() const {
  switch (this->key.type) {
  case Key::INTEGER: {
    std::ostringstream convert;
    convert << this->data.integer;
    return convert.str();
   break;
  }
  case Key::STRING: {
    return std::string(this->data.string->c_str());
    break;
  }
  default:
    return std::string("This is impossible");
    break;
  }
  return std::string("This is impossible");
};

Field::~Field() {
  if (this->key.type == Key::STRING) {
    delete this->data.string;
  }
}

void panic_on_bad_compare(const Field& f1, const Field& f2) {
  if (f1.key.type != f2.key.type) {
    std::stringstream ss;
    ss << "Fields " << f1.key.name << " and " << f2.key.name
      << " Cannot be logically compared for order";
    throw std::logic_error(ss.str());
  }
}

bool Field::operator<(const Field& other) const {
  panic_on_bad_compare(*this, other);
  switch (this->key.type) {
  case Key::INTEGER: {
    return this->data.integer < other.data.integer;
    break;
  }
  case Key::STRING: {
    return *(this->data.string) < *(other.data.string);
    break;
  }
  }
  throw std::logic_error("This should be impossible.");
}

bool Field::operator>(const Field& other) const {
  panic_on_bad_compare(*this, other);

  switch (this->key.type) {
  case Key::INTEGER: {
    return this->data.integer > other.data.integer;
    break;
  }
  case Key::STRING: {
    return *(this->data.string) > *(other.data.string);
    break;
  }
  }
  throw std::logic_error("This should be impossible.");
}

bool Field::operator<=(const Field& other) const {
  return !(*this > other);
}

bool Field::operator>=(const Field& other) const {
  return !(*this < other);
}

bool Field::operator==(const Field& other) const {
  panic_on_bad_compare(*this, other);
  if (this->key.type == other.key.type) {
    switch (this->key.type) {
    case Key::INTEGER: {
      return this->data.integer == other.data.integer;
      break;
    }
    case Key::STRING: {
      return *(this->data.string) == *(other.data.string);
      break;
    }
    }
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Index class implementation section
////////////////////////////////////////////////////////////////////////////////
template<typename Word>
std::ostream& write_raw(std::ostream& outs, Word value);

template<typename Word>
static std::istream& read_raw(std::istream& ins, Word& value);

Index* Index::from_csv(std::string file_name) {
  std::string output_file_name; {
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
      if (ch == DELIM) {
        key_count += 1;
      }
    }
    key_count += 1;
  }

  // save a spot for size;
  output.seekp(sizeof(Word));

  size_t size = 0;  // keep track of count of records
  { // fill output with offsets
    char ch;
    size_t cur = input.tellg();
    size_t count = 0;

    while (input.get(ch)) {
      if (ch == DELIM) {
        count += 1;
      }
      if (ch == '\n' && count >= key_count - 1) {
        size += 1;
        write_raw<Word>(output, cur);
        cur = input.tellg();
        count = 0;
      }
    }
  }

  { // write number of records at first byte
    output.seekp(0);
    write_raw<Word>(output, size);
  }

  output.sync();
  return new Index(output_file_name);
}

Index::Index(std::string file_name) : file_name(file_name) {
  stream.open(file_name.c_str(), stream.binary | stream.in);

  { // get record count from beginning of file
    size_t size = 0;
    this->stream.seekg(0);
    read_raw<Word>(this->stream, size);
    this->size_ = size;
  }
};

Index::~Index() {
  if (this->stream.is_open()) {
    this->stream.close();
  }
}

size_t Index::get(size_t i) {
  int count_offset = sizeof(Word);
  this->stream.seekg(i * sizeof(Word) + count_offset);
  Word offset;
  read_raw<Word>(this->stream, offset);
  return offset;
}

size_t Index::operator[](size_t i) {
  return this->get(i);
}

size_t Index::size() const {
  return this->size_;
}

template<typename Word>
std::ostream& write_raw(std::ostream& outs, Word value) {
  for (size_t size = sizeof(Word); size; --size, value >>= 8) {
    outs.put((char)(value & 0xFF));
  }
  return outs;
}

template<typename Word>
std::istream& read_raw(std::istream& ins, Word& value) {
  value = 0;
  size_t length = sizeof(Word);
  for (size_t i = 0; i < length; i++) {
    int byte = ins.get() << (8 * i);
    value |= byte;
  }
  return ins;
}
