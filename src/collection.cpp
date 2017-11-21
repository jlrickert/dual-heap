#include <iostream>
#include <map>
#include <algorithm>

#include "collection.h"
#include "record.h"
#include "utilities.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
/// Construct collection
////////////////////////////////////////////////////////////////////////////////
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

key_pair_t parse_key(string raw);

void Collection::init_keys() {
  string line;
  getline(input, line);

  int key_count = 0;
  int cp = 0;  // alias to checkpoint
  for (size_t i = 0; i < line.length(); i += 1) {
    bool is_delim = line[i] == CSV_DELIM;
    bool is_end = i == (line.length() - 1);
    if (is_delim || is_end) {
      int pos = cp;
      int offset = i - cp + (is_end ? 1 : 0);
      string raw_key = line.substr(pos, offset);
      key_pair_t key = parse_key(raw_key);
      this->keys_.push_back(key);
      this->key_index.insert(pair<string, key_pair_t>(key.first, key));
      this->key_pos_index.insert(pair<string, size_t>(key.first, key_count));
      key_count += 1;
      cp = i + 1;
    }
  }
}

void Collection::init_index() {
  this->index = Index::from_csv(this->file_name);
}

key_pair_t parse_key(string raw) {
  for (size_t i = 0; i < raw.length() - 1; i += 1) {
    if (raw[i] == ':') {
      string key_name = raw.substr(0, i);
      string type_string = raw.substr(i + 1);

      // caps don't matter
      transform(type_string.begin(), type_string.end(),
                type_string.begin(), ::toupper);

      key_pair_t key;
      if (type_string == "STR" || type_string == "STRING") {
        key = key_pair_t(key_name, STRING);
      } else if (type_string == "INT" || type_string == "INTEGER") {
        key = key_pair_t(key_name, INTEGER);
      }

      return key;
    }
  }
  stringstream ss;
  ss << "\"" << raw << "\"" << " is not a valid syntax";
  throw runtime_error(ss.str());
}

////////////////////////////////////////////////////////////////////////////////
/// utilities section
////////////////////////////////////////////////////////////////////////////////
vector<string> Collection::keys() const {
  vector<string> elements;
  for (vector<key_pair_t>::const_iterator it = this->keys_.begin();
       it != this->keys_.end(); ++it) {
    elements.push_back(it->first);
  }
  return elements;
}

Record Collection::operator[](size_t row) {
  if (row >= this->size()) {
    stringstream ss;
    ss << "Record " << row << " doesn't exist.";
    throw out_of_range(ss.str());
  }
  size_t rrn = this->index->get(row);
  return Record(*this, rrn);
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

////////////////////////////////////////////////////////////////////////////////
/// sort section
////////////////////////////////////////////////////////////////////////////////
bool compare(Collection& collection, size_t a, size_t b,
             vector<string> keys);

void heapify(Collection& collection, size_t heap[], size_t pending,
             vector<string> keys);

// TODO: move to a utility file
template <typename T>
string stringify(T array[], size_t size);
template <typename T>
string stringify(T array[], size_t begin, size_t size);

void Collection::sort(string output_file, vector<string> keys) {
  fstream buffer, offsets;

  string buffer_file_name = "tmp/buffer";
  buffer.open(buffer_file_name.c_str(),
              buffer.binary | buffer.trunc | buffer.out | buffer.in);
  if (!buffer.good()) {
    ostringstream ss;
    ss << "Could not open file " << buffer_file_name;
    throw runtime_error(ss.str());
  }

  string offset_file_name = "tmp/offsets";
  offsets.open(offset_file_name.c_str(),
               buffer.binary | buffer.trunc | buffer.out | buffer.in);
  if (!offsets.good()) {
    ostringstream ss;
    ss << "Could not open file " << offset_file_name;
    throw runtime_error(ss.str());
  }

  fstream output;
  output.open(output_file.c_str(), output.trunc | output.out);
  if (!offsets.good()) {
    ostringstream ss;
    ss << "Could not open file " << output_file;
    throw runtime_error(ss.str());
  }

  this->replacement_selection_sort(buffer, offsets, keys);
  this->kway_merge(buffer, output, offsets, keys);
}

fstream& Collection::replacement_selection_sort(fstream& buffer,
                                                fstream& offsets,
                                                vector<string> keys) {
  size_t heap[Collection::HEAP_SIZE];
  buffer.seekg(0);
  offsets.seekg(0);

  size_t size = 0;     // current count of elements in heap;
  size_t pending = 0;  // artificial size of heap
  size_t count = 0;    // number of elements in current bucket
  size_t item;         // last item to be put into buffer
  size_t row = 0;
  for (; row < this->size(); row += 1) {
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
        cout << "End of bucket. Writing out bucket size to " << count << "." << endl;
        write_raw<short unsigned int>(offsets, count);
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
        write_raw<short unsigned int>(offsets, count);
        count = 0;
        pending = size;
      }
    }
    cout << endl;
  }

  buffer.flush();
  offsets.flush();
  return buffer;
}


fstream& Collection::kway_merge(fstream& buffer, fstream& output,
                               fstream& offsets,
                               vector<string> keys) {
  buffer.seekg(0);
  output.seekg(0);
  string line;
  while(getline(buffer, line)) {
    output << line;
  }
  output.flush();
  return output;
}

bool compare(Collection& collection, size_t a, size_t b,
             vector<string> keys) {
  Record rec_a = collection[a];
  Record rec_b = collection[b];
  return rec_a.lt(rec_b, keys);
}

void heapify(Collection& collection, size_t heap[], size_t size,
             vector<string> keys) {
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
