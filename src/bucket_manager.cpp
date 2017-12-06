#include "bucket_manager.h"
#include "bucket.h"
#include "sstream"

#include "iostream"
#include "utilities.h"

using namespace std;

BucketManager::BucketManager(Collection& collection, fstream& buffer,
                             vector<size_t> bucket_sizes)
    : collection(collection), sorted_buffer(buffer) {
  this->buckets_ = vector<Bucket>();
  this->bucket_sizes_ = bucket_sizes;
  this->offsets_ = this->find_offsets(this->bucket_sizes_);
}

void BucketManager::init() {
  this->cursors = this->offsets_;
  for (size_t i = 0; i < bucket_sizes_.size(); i += 1) {
    Bucket bucket = this->fetch(i);
    this->buckets_.push_back(bucket);
  }
  cerr << "DEBUG: initial bucket sizes "
       << Util::stringifyVector(this->bucket_sizes_) << endl;
  cerr << "DEBUG: initial bucket offsets "
       << Util::stringifyVector(this->offsets_) << endl;
  cerr << "DEBUG: initial bucket cursor positions "
       << Util::stringifyVector(this->cursors) << endl;
  cerr << "DEBUG: initial buckets " << this->buckets_str() << endl;
}

bool BucketManager::finished() { return this->buckets_.size() == 0; }

Bucket BucketManager::pop() {
  Bucket bucket = this->buckets_[0];
  this->buckets_[0] = this->buckets_[this->buckets_.size() - 1];

  size_t bucket_number = bucket.bucket_number();
  size_t bucket_end =
      this->offsets_[bucket_number] + this->bucket_sizes_[bucket_number];
  if (this->cursors[bucket_number] >= bucket_end) {
    vector<Bucket>::iterator last = this->buckets_.end();
    last -= 1;
    this->buckets_.erase(last);
  } else {
    this->buckets_[this->buckets_.size() - 1] = this->fetch(bucket_number);
  }

  return bucket;
}

int BucketManager::compare(const Bucket& a, const Bucket& b,
                           std::vector<std::string> keys) const {
  Record rec_a = a.record();
  Record rec_b = b.record();
  if (rec_a.lte(rec_b, keys)) {
    return -1;
  } else if (rec_a.eq(rec_b, keys)) {
    return 0;
  } else {
    return 1;
  }
}

void BucketManager::heapify(vector<string> keys) {
  size_t size = this->buckets_.size();
  if (size <= 1) {
    return;
  } else if (size <= 2 &&
             this->compare(this->buckets_[1], this->buckets_[0], keys) < 0) {
    Bucket temp = this->buckets_[0];
    this->buckets_[0] = this->buckets_[1];
    this->buckets_[1] = temp;
    return;
  }

  bool done = false;
  while (!done) {
    done = true;
    for (size_t i = size / 2; i >= 1; i -= 1) {
      Bucket parent = this->buckets_[i - 1];
      Bucket left = this->buckets_[i * 2 - 1];

      if (i * 2 >= size) {
        if (this->compare(left, parent, keys) < 0) {
          this->buckets_[i - 1] = left;
          this->buckets_[i * 2 - 1] = parent;
          done = false;
        }
      } else {
        Bucket right = this->buckets_[i * 2];
        if (this->compare(left, right, keys) &&
            this->compare(left, parent, keys) < 0) {
          this->buckets_[i - 1] = left;
          this->buckets_[i * 2 - 1] = parent;
          done = false;
        } else if (this->compare(right, left, keys) < 0 &&
                   this->compare(right, parent, keys) < 0) {
          this->buckets_[i - 1] = right;
          this->buckets_[i * 2] = parent;
          done = false;
        }
      }
    }
  }
}

string BucketManager::buckets_str() {
  return Util::stringifyVector(this->buckets_);
}

vector<size_t> BucketManager::find_offsets(const vector<size_t>& bucket_sizes) {
  vector<size_t> bucket_offsets;
  if (bucket_sizes.size() > 0) bucket_offsets.push_back(0);
  unsigned int count = 0;
  for (size_t i = 0; i < bucket_sizes.size() - 1; i += 1) {
    count += bucket_sizes[i];
    bucket_offsets.push_back(count);
  }
  return bucket_offsets;
}

Bucket BucketManager::fetch(size_t bucket_number) {
  size_t row;
  this->sorted_buffer.seekg(this->cursors[bucket_number] * sizeof(size_t));
  Util::read_raw<size_t>(this->sorted_buffer, row);
  Record rec(this->collection.get(row));
  Bucket bucket(rec, bucket_number);
  this->cursors[bucket_number] += 1;
  return bucket;
}
