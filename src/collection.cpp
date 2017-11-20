#include <algorithm>
#include <stdexcept>
#include <sstream>
#include "collection.h"

using namespace std;

template<typename Word>
ostream& write_raw(ostream& outs, Word value);

template<typename Word>
static istream& read_raw(istream& ins, Word& value);


////////////////////////////////////////////////////////////////////////////////
// Collection class implementation section
////////////////////////////////////////////////////////////////////////////////
Key parse_key(string raw, int pos);

Collection::Collection(const string& file_name) : file_name(file_name) {
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
    stringstream ss;
    ss << "File " << file_name.c_str() << " doesn't exist";
    throw runtime_error(ss.str());
  }
}

void Collection::init_keys() {
  string line;
  getline(input, line);

  int key_count = 0;
  int cp = 0;  // alias to checkpoint
  for (size_t i = 0; i < line.length(); i += 1) {
    bool is_delim = line[i] == DELIM;
    bool is_end = i == (line.length() - 1);
    if (is_delim || is_end) {
      int pos = cp;
      int offset = i - cp + (is_end ? 1 : 0);
      string raw_key = line.substr(pos, offset);
      key_count += 1;
      Key key = parse_key(raw_key, key_count);
      this->keys_.push_back(key.name);
      this->key_type_map.insert(key_pair_t(key.name, key));
      cp = i + 1;
    }
  }
}

void Collection::init_index() {
  this->index = Index::from_csv(this->file_name);
}

const vector<string>& Collection::keys() const {
  return this->keys_;
}

Collection::Record Collection::operator[](size_t row) {
  if (row >= this->size()) {
    stringstream ss;
    ss << "Record " << row << " doesn't exist.";
    throw out_of_range(ss.str());
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

Key parse_key(string raw, int pos) {
  for (size_t i = 0; i < raw.length() - 1; i += 1) {
    if (raw[i] == ':') {
      string key_name = raw.substr(0, i);
      string type_string = raw.substr(i + 1);

      // don't care about case
      transform(type_string.begin(), type_string.end(),
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
  stringstream ss;
  ss << "\"" << raw << "\"" << " is not a valid syntax";
  throw runtime_error(ss.str());
}

////////////////////////////////////////////////////////////////////////////////
// Record class implementation section
////////////////////////////////////////////////////////////////////////////////
Collection::Record::Record(Collection& collection, size_t offset)
  : offset(offset),
    collection(collection) {}

Field Collection::Record::get(string key_name) {
  Key key = this->collection.key_type_map.find(key_name)->second;
  return Collection::Record::get(key);
}

Field Collection::Record::get(Key key) {
  // Seek record
  this->collection.seek(this->offset);

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

  string raw_string;
  {  // read raw value of field
    this->collection.seek(begin);
    ostringstream ss;
    char ch;
    while (this->collection.input.get(ch)) {
      if (ch == DELIM || ch == '\n') break;
      else ss << ch;
    }
    raw_string = ss.str();
  }

  Field::Data data;
  { // parse field data
    switch (key.type) {
    case Key::STRING: {
      string* str = new string(raw_string.c_str());
      Field::Data d;
      d.string = str;
      data = d;
      break;
    }
    case Key::INTEGER:
      stringstream ss;
      ss << raw_string;
      int x = 0;
      ss >> x;
      Field::Data d;
      d.integer = x;
      data = d;
      break;
    }
  }
  Field field(data, key);
  return field;
}

Field Collection::Record::operator[](string key_name) {
  return this->get(key_name);
}

Field Collection::Record::operator[](Key key) {
  return this->get(key);
}

bool Collection::Record::lt(Collection::Record& other, vector<Key> keys) {
  for (vector<Key>::iterator it = keys.begin(); it != keys.end(); ++it) {
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

bool Collection::Record::lte(Collection::Record& other, vector<Key> keys) {
  return !this->gt(other, keys);
}

bool Collection::Record::gt(Collection::Record& other, vector<Key> keys) {
  for (vector<Key>::iterator it = keys.begin(); it != keys.end(); ++it) {
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

bool Collection::Record::gte(Collection::Record& other, vector<Key> keys) {
  return !this->lt(other, keys);
}

string Collection::Record::str() {
  ostringstream ss;
  ss << "Record(";
  for (vector<string>::const_iterator it = this->collection.keys().begin();
       it != this->collection.keys().end(); ++it) {
    Field field = this->get(*it);
    ss << *it << "=" << field.str();
  }
  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////
// Field class implementation section
////////////////////////////////////////////////////////////////////////////////
string Field::str() const {
  switch (this->key.type) {
  case Key::INTEGER: {
    ostringstream convert;
    convert << this->data.integer;
    return convert.str();
   break;
  }
  case Key::STRING: {
    return string(this->data.string->c_str());
    break;
  }
  default:
    return string("This is impossible");
    break;
  }
  return string("This is impossible");
};

Field::~Field() {
  if (this->key.type == Key::STRING) {
    delete this->data.string;
  }
}

void panic_on_bad_compare(const Field& f1, const Field& f2) {
  if (f1.key.type != f2.key.type) {
    stringstream ss;
    ss << "Fields " << f1.key.name << " and " << f2.key.name
       << " Cannot be logically compared for order";
    throw logic_error(ss.str());
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
  throw logic_error("This should be impossible.");
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
  throw logic_error("This should be impossible.");
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

bool compare(Collection& collection, size_t a, size_t b,
             vector<Key> keys);
void heapify(Collection& collection, size_t heap[], size_t pending,
             vector<Key> keys);

// TODO: move to a utility file
template <typename T>
string stringify(T array[], size_t size);
template <typename T>
string stringify(T array[], size_t begin, size_t size);

void Collection::sort(string output_file, vector<string> keys) {
  vector<Key> k;
  for (vector<string>::iterator it = keys.begin(); it != keys.end(); it++) {
    Key key = this->key_type_map.find(*it)->second;
    k.push_back(key);
  }
  return this->sort(output_file, k);
}

void Collection::sort(string output_file, vector<Key> keys) {
  fstream buffer;

  string buffer_file_name = "tmp/buffer";
  buffer.open(buffer_file_name.c_str(),
              buffer.binary | buffer.trunc | buffer.out | buffer.in);
  if (!buffer.good()) {
    ostringstream ss;
    ss << "Could not open file " << buffer_file_name;
    throw runtime_error(ss.str());
  }

  vector<BucketSize_t> bucket_sizes;

  fstream output;
  output.open(output_file.c_str(), output.trunc | output.out);
  if (!output.good()) {
    ostringstream ss;
    ss << "Could not open file " << output_file;
    throw runtime_error(ss.str());
  }

  this->replacement_selection_sort(buffer, bucket_sizes, keys);
  { // write header to output
    vector<string> keys = this->keys();
    for (size_t i = 0; i < keys.size(); i += 1) {
      Key key = this->key_type_map.find(keys[i])->second;
      string type;
      switch (key.type) {
      case Key::INTEGER: {
        type = "integer";
        break;
      }
      case Key::STRING:
        type = "string";
        break;
      }
      output << keys[i] << ":" << type;
      if (i == keys.size() - 1) {
        output << '\n';
      } else {
        output << ',';
      }
    }
  }
  this->kway_merge(buffer, output, bucket_sizes, keys);
}

fstream& Collection::replacement_selection_sort(
    fstream& buffer, vector<BucketSize_t>& bucket_sizes,
    vector<Key> keys) {
  size_t heap[Collection::HEAP_SIZE];
  buffer.seekg(0);

  size_t size = 0;       // current count of elements in heap;
  size_t pending = 0;    // artificial size of heap
  size_t count = 0;      // number of elements in current bucket
  size_t item;           // last item to be put into buffer
  for (size_t row = 0; row < this->size(); row += 1) {
    if (count == 0) {
      cout << "New bucket." << endl;
    }

    { // initially fill the heap
      if (size < Collection::HEAP_SIZE) {
        heap[size] = row;
        size += 1;
        pending += 1;
        heapify(*this, heap, pending, keys);
        continue;
      }
    }

    { // Log current pass
      cout << "Pending=" << pending << ", Bucket count=" << count << ", Next=" << row << endl;
      cout << "Initial: " << stringify<size_t>(heap, size) << endl;
    }

    { // heapify
      heapify(*this, heap, pending, keys);
      cout << "Heap: " << stringify<size_t>(heap, pending) << endl;
    }

    { // select value to put in buffer
      item = heap[0];
      count += 1;
      cout << "Select: (" << heap[0] << ") " << stringify<size_t>(heap, 1, size) << endl;
      write_raw<size_t>(buffer, item);
    }

    { // Swap in new incoming value
      heap[0] = heap[pending - 1];  // move last item in heap into
      heap[pending - 1] = row;      // move in new item to the end
      cout << "Swap: " << stringify<size_t>(heap, size) << endl;
    }

    { // Mark pending positions if necessary
      if (compare(*this, row, item, keys)) { // check if incoming record is
                                           // smaller than last value pushed to
                                           // buffer.
        // artificially reduce heap size
        pending -= 1;
      }

      if (pending == 0) {  // check if new bucket is needed
        cout << "End of bucket. Writing out bucket size to " << count
                  << "." << endl;
        bucket_sizes.push_back(count);
        count = 0;
        pending = size;
      }
    }
    cout << endl;
  }

  // write out remaining values
  while (size > 0) {
    if (count == 0) {
      cout << "New bucket." << endl;
    }

    { // log current pass
      cout << "Pending=" << pending << endl;
      cout << "Initial: " << stringify<size_t>(heap, size) << endl;
    }

    { // heapify
      heapify(*this, heap, pending, keys);
      cout << "Heap: " << stringify<size_t>(heap, size) << endl;
    }

    size_t item;
    { // select value to put in buffer
      item = heap[0];
      count += 1;
      cout << "Select: (" << heap[0] << ") " << stringify<size_t>(heap, 1, size) << endl;
      write_raw<size_t>(buffer, item);
    }

    { // Swap in new incoming value
      heap[0] = heap[pending - 1];
      for (size_t i = pending; i < size; i += 1) {
        heap[i - 1] = heap[i];
      }
      heap[pending - 1] = heap[size - 1]; //
      size -= 1;
      pending -= 1;
      cout << "Swap: " << stringify(heap, size) << endl;
    }

    {
      if (!pending) {  // reset pending heap size and write out offset
        cout << "End of bucket. Writing out bucket size to " << count << "." << endl;
        bucket_sizes.push_back(count);
        count = 0;
        pending = size;
      }
    }
    cout << endl;
  }

  buffer.flush();
  return buffer;
}

fstream& Collection::kway_merge_pass(fstream& buffer, fstream& output,
                                    vector<size_t>& bucket_offsets,
                                    vector<BucketSize_t>& bucket_sizes,
                                    vector<Key> keys) {
  size_t count = 0;
  return output;
}

vector<size_t> Collection::find_offsets(
    const vector<BucketSize_t>& bucket_sizes) {
  vector<size_t> bucket_offsets;
  if (bucket_sizes.size() > 0) bucket_offsets.push_back(0);
  BucketSize_t count = 0;
  for (size_t i = 0; i < bucket_sizes.size() - 1; i += 1) {
    count += bucket_sizes[i];
    bucket_offsets.push_back(count);
  }
  return bucket_offsets;
}

fstream& Collection::kway_merge(fstream& buffer, fstream& output,
                                vector<BucketSize_t>& bucket_sizes,
                                vector<Key> keys) {
  buffer.seekg(0);
  size_t active_buckets = 0;
  int buckets[Collection::HEAP_SIZE]; // holds smallest value of each active
  vector<size_t> bucket_offsets = find_offsets(bucket_sizes);

  if (bucket_sizes.size() <= Collection::HEAP_SIZE) {
    // do final merge
    return kway_merge_pass(buffer, output, bucket_offsets, bucket_sizes, keys);
  } else { // more passes
    fstream new_buffer;
    { // create a new buffer
      ostringstream ss;
      ss << "tmp/buffer" << bucket_sizes.size();
      string buffer_file_name = ss.str();
      buffer.open(buffer_file_name.c_str(),
                  buffer.binary | buffer.trunc | buffer.out | buffer.in);
      if (!new_buffer.good()) {
        ostringstream ss;
        ss << "Could not open file " << buffer_file_name;
        throw runtime_error(ss.str());
      }
    }
    { // do pass and fill new counts
      size_t passes = bucket_sizes.size() / Collection::HEAP_SIZE;
      size_t begin = 0;
      size_t end = Collection::HEAP_SIZE;
      if ((bucket_sizes.size() % Collection::HEAP_SIZE) != 0) {
        passes += 1;
      }

      for (size_t i = 0; i < passes; i += 1) {
        vector<BucketSize_t> sizes = vector<BucketSize_t>(
            bucket_sizes.begin() + begin, bucket_sizes.begin() + end);
        vector<size_t> offsets = vector<size_t>(bucket_sizes.begin() + begin,
                                                bucket_sizes.begin() + end);
        kway_merge_pass(buffer, new_buffer, offsets, sizes, keys);
        begin = end;
        end += (bucket_sizes.size() % Collection::HEAP_SIZE);
      }
    }
    // vector<size_t> bucket_offsets = find_offsets(bucket_sizes);
    // return kway_merge(next_buffer, output, bucket_offsets, bucket_sizes, keys)
  }

  return output;

  // fstream next_buffer;
  // if (Collection::HEAP_SIZE == bucket_sizes.size()) {
  //   return this->complete_way_merge(buffer, output, bucket_sizes, keys);
  // } else {
  //   ostringstream ss;
  //   ss << "tmp/buffer" << bucket_sizes.size();
  //   string buffer_file_name = ss.str();
  //   buffer.open(buffer_file_name.c_str(),
  //               buffer.binary | buffer.trunc | buffer.out | buffer.in);
  //   if (!next_buffer.good()) {
  //     ostringstream ss;
  //     ss << "Could not open file " << buffer_file_name;
  //     throw runtime_error(ss.str());
  //   }
  // }

  // size_t rows = 0;                    // count of rows written out
  // size_t bucket_cur = 0;              // points last active bucket
  // int buckets[Collection::HEAP_SIZE]; // holds smallest value of each active
  //                                     // bucket
  // size_t size = 0;                    // count of elements in buckets
  // vector<size_t> bucket_offsets;      // location of bucket in file

  // { // fill bucket offsets
  //   if (bucket_sizes.size() > 0) bucket_offsets.push_back(0);
  //   BucketSize_t count = 0;
  //   for (size_t i = 0; i < bucket_sizes.size() - 1; i += 1) {
  //     count += bucket_sizes[i];
  //     bucket_offsets.push_back(count);
  //   }
  // }

  // while (rows < this->size()) {
  //   { // activate bucket
  //     if (size < Collection::HEAP_SIZE && bucket_cur < bucket_sizes.size()) {
  //       size += 1;
  //       bucket_cur += 1;
  //     }
  //   }

  //   {

  //   }

  //   rows += 1;
  // }

  // return this->complete_way_merge(next_buffer, output, bucket_sizes, keys);
}

bool compare(Collection& collection, size_t a, size_t b,
             vector<Key> keys) {
  Collection::Record rec_a = collection[a];
  Collection::Record rec_b = collection[b];
  return rec_a.lt(rec_b, keys);
}

void heapify(Collection& collection, size_t heap[], size_t size, vector<Key> keys) {
  if (size <= 1) {
    return;
  } else if (size <= 2 && compare(collection, heap[1], heap[0], keys)) {
    size_t temp = heap[0];
    heap[0] = heap[1];
    heap[1] = temp;
    return;
  }
  for (size_t i = size/2; i >= 1; i -=1) {
    size_t parent = heap[i - 1];
    size_t left = heap[i * 2 - 1];

    if (i * 2 >= size) {
      if (compare(collection, left, parent, keys)) {
        heap[i - 1] = left;
        heap[i * 2 - 1] = parent;
      }
    } else {
      size_t right = heap[i * 2];
      if (compare(collection, left, right, keys) && compare(collection, left, parent, keys)) {
        heap[i - 1] = left;
        heap[i * 2 - 1] = parent;
      } else if (compare(collection, right, left, keys) && compare(collection, right, parent, keys)) {
        heap[i - 1] = right;
        heap[i * 2] = parent;
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
// Index class implementation section
////////////////////////////////////////////////////////////////////////////////
Index* Index::from_csv(string file_name) {
  string output_file_name; {
    stringstream ss;
    ss << "tmp/index";
    output_file_name = ss.str();
  }

  fstream input, output;
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

Index::Index(string file_name) : file_name(file_name) {
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

////////////////////////////////////////////////////////////////////////////////
// Utility section
////////////////////////////////////////////////////////////////////////////////
template<typename Word>
ostream& write_raw(ostream& outs, Word value) {
  for (size_t size = sizeof(Word); size; --size, value >>= 8) {
    outs.put((char)(value & 0xFF));
  }
  return outs;
}

template<typename Word>
istream& read_raw(istream& ins, Word& value) {
  value = 0;
  size_t length = sizeof(Word);
  for (size_t i = 0; i < length; i++) {
    int byte = ins.get() << (8 * i);
    value |= byte;
  }
  return ins;
}

template <typename T>
string stringify(T array[], size_t size) {
  return stringify<T>(array, 0, size);
}

template <typename T>
string stringify(T array[], size_t begin, size_t size) {
  ostringstream ss;
  for (size_t i = begin; i < size; i += 1) {
    ss << array[i];
    if (i < size - 1) {
      ss << " ";
    }
  }
  return ss.str();
}
