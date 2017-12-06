#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "bucket.h"
#include "bucket_manager.h"
#include "collection.h"
#include "record.h"
#include "rss_impl.h"
#include "types.h"
#include "utilities.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
/// Construct collection
////////////////////////////////////////////////////////////////////////////////
Collection::Collection(const string& file_name) : file_name_(file_name) {
  this->init_file();
  this->init_keys();
  this->init_index();
}

Collection::~Collection() {
  delete this->index;
  if (input->is_open()) {
    this->input->close();
  }
  delete this->input;
}

void Collection::init_file() {
  this->input = new ifstream();
  this->input->open(file_name_.c_str());
  if (!this->input->is_open()) {
    stringstream ss;
    ss << "File " << file_name_.c_str() << " doesn't exist";
    throw runtime_error(ss.str());
  }
}

void Collection::init_keys() {
  string line;
  getline(*input, line);

  int key_count = 0;
  int cp = 0;  // alias to checkpoint
  for (size_t i = 0; i < line.length(); i += 1) {
    bool is_delim = line[i] == CSV_DELIM;
    bool is_end = i == (line.length() - 1);
    if (is_delim || is_end) {
      int pos = cp;
      int offset = i - cp + (is_end ? 1 : 0);
      string raw_key = line.substr(pos, offset);
      key_pair_t key = this->parse_key(raw_key);
      this->keys_.push_back(key);
      this->key_index.insert(pair<string, key_pair_t>(key.first, key));
      this->key_pos_index.insert(pair<string, size_t>(key.first, key_count));
      key_count += 1;
      cp = i + 1;
    }
  }
}

void Collection::init_index() {
  this->index = Index::from_csv(this->file_name_);
}

key_pair_t Collection::parse_key(string raw) {
  for (size_t i = 0; i < raw.length() - 1; i += 1) {
    if (raw[i] == ':') {
      string key_name = raw.substr(0, i);
      string type_string = raw.substr(i + 1);

      // caps don't matter
      transform(type_string.begin(), type_string.end(), type_string.begin(),
                ::toupper);

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
  ss << "\"" << raw << "\""
     << " is not a valid syntax";
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

Record Collection::get(size_t row) {
  if (row >= this->size()) {
    stringstream ss;
    ss << "Record " << row << " doesn't exist.";
    throw out_of_range(ss.str());
  }
  size_t rrn = this->index->get(row);
  return Record(*this, row, rrn);
}

Record Collection::operator[](size_t row) { return this->get(row); }

size_t Collection::size() const { return this->index->size(); }

string Collection::file_name() const { return this->file_name_; }

int Collection::compare(size_t a, size_t b, vector<string> keys) {
  Record rec_a = this->get(a);
  Record rec_b = this->get(b);
  if (rec_a.lte(rec_b, keys)) {
    return -1;
  } else if (rec_a.eq(rec_b, keys)) {
    return 0;
  } else {
    return 1;
  }
}

////////////////////////////////////////////////////////////////////////////////
/// sort section
////////////////////////////////////////////////////////////////////////////////
fstream& Collection::output_keys(fstream& output) {
  for (size_t i = 0; i < this->keys_.size(); i += 1) {
    key_pair_t key = this->keys_[i];
    string key_name = key.first;
    Tkey type = key.second;
    output << key_name << ':';
    switch (type) {
      case STRING: {
        output << "string";
        break;
      }
      case INTEGER: {
        output << "integer";
        break;
      }
    }
    if (i < this->keys_.size() - 1) {
      output << ',';
    } else {
      output << endl;
    }
  }
  return output;
}

void Collection::sort(string output_file, vector<string> keys) {
  fstream buffer;

  string buffer_file_name = "tmp/buffer";
  buffer.open(buffer_file_name.c_str(),
              buffer.binary | buffer.trunc | buffer.out | buffer.in);
  if (!buffer.good()) {
    ostringstream ss;
    ss << "Could not open file " << buffer_file_name;
    throw runtime_error(ss.str());
  }

  fstream output;
  output.open(output_file.c_str(), output.trunc | output.out);
  if (!output.good()) {
    ostringstream ss;
    ss << "Could not open file " << output_file;
    throw runtime_error(ss.str());
  }

  vector<size_t> bucket_sizes;
  this->replacement_selection_sort(buffer, bucket_sizes, keys);
  this->output_keys(output);

  this->kway_merge(buffer, output, bucket_sizes, keys);
}

std::fstream& Collection::replacement_selection_sort(
    fstream& buffer, vector<size_t>& bucket_sizes, vector<std::string> keys) {
  buffer.seekg(0);
  RSSImpl impl(*this, keys);
  impl.init();

  cout << "Initialize Heap: " << impl.stringify_heap() << endl;

  int count = 0;
  while (!impl.finished() && count <= 100) {
    cout << impl.stats() << endl;

    impl.heapify();
    cout << "Heapified: " << impl.stringify_heap() << endl;

    Record rec = impl.pop();
    Util::write_raw<size_t>(buffer, rec.row());
    cout << "Writing row " << rec.row() << " to buffer. peek=" << rec.str()[17]
         << rec.str()[18] << endl;

    cout << "Updating heap: " << impl.stringify_heap() << endl;

    cout << endl;
    count += 1;
  }

  bucket_sizes = impl.bucket_counts();
  return buffer;
}

fstream& Collection::kway_merge(fstream& buffer, fstream& output,
                                vector<size_t> bucket_sizes,
                                vector<string> keys) {
  cout << "Starting Kway merge" << endl;
  cout << "Current Bucket sizes: "
       << Util::stringifyVector<size_t>(bucket_sizes) << endl;
  buffer.seekg(0);

  BucketManager manager(*this, buffer, bucket_sizes);
  manager.init();

  while (!manager.finished()) {
    manager.heapify(keys);
    cout << "Heapify: " << manager.buckets_str() << endl;

    Bucket bucket = manager.pop();

    Record record = bucket.record();
    output << record << endl;
    cout << "Writing out " << bucket << " " << record.str()[17]
         << record.str()[18] << endl;
    cout << "Heap updated: " << manager.buckets_str() << endl;

    cout << endl;
  }
  return output;
}
