#include "sstream"
#include "bucket.h"
#include "bucket_manager.h"

#include "utilities.h"
#include "iostream"

using namespace std;

BucketManager::BucketManager(Collection& collection,
                            fstream& buffer,
                            vector<int> bucket_sizes)
    : collection(collection), sorted_buffer(buffer) {
  this->buckets_ = vector<Bucket>();
  this->bucket_sizes_ = bucket_sizes;
  this->bucket_offsets_ = this->find_offsets(this->bucket_sizes_);
}

void BucketManager::init() {
  for (size_t i = 0; i < bucket_sizes_.size(); i += 1) {
    size_t row;
    this->sorted_buffer.seekg(this->bucket_offsets_[i] * sizeof(size_t));
    Util::read_raw<size_t>(this->sorted_buffer, row);
    Record rec(this->collection.get(row));
    Bucket bucket(rec, i);
    this->buckets_.push_back(bucket);
    cout << this->buckets_[i] << endl;
    this->cursors.push_back(this->bucket_offsets_[i]);
  }
  cerr << "DEBUG: initial bucket offsets " << Util::stringifyVector(this->bucket_offsets_) << endl;
  cerr << "DEBUG: initial bucket positions " << Util::stringifyVector(this->cursors) << endl;
  cerr << "DEBUG: initial buckets " << Util::stringifyVector(this->buckets_) << endl;
}

bool BucketManager::finished() {
  return this->buckets_.size() == 0;
}

Record BucketManager::pop() {
  Bucket bucket(this->buckets_[0]);
  // Record rec(this->buckets_[0].record());
  this->buckets_[0] = this->buckets_[this->buckets_.size() - 1];

  this->buckets_[this->buckets_.size() - 1];
  return bucket.record();
}

void BucketManager::heapify(vector<string> keys) {
}

vector<size_t> BucketManager::find_offsets(const vector<int>& bucket_sizes) {
  vector<size_t> bucket_offsets;
  if (bucket_sizes.size() > 0) bucket_offsets.push_back(0);
  unsigned int count = 0;
  for (size_t i = 0; i < bucket_sizes.size() - 1; i += 1) {
    count += bucket_sizes[i];
    bucket_offsets.push_back(count);
  }
  return bucket_offsets;
}
