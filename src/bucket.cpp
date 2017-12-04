#include "bucket.h"

Bucket::Bucket(Record& record, size_t bucket_number)
    : rec(record), bucket_number_(bucket_number) {}

Bucket::Bucket(const Bucket& other)
  : rec(other.rec),
    bucket_number_(other.bucket_number_) {}

Bucket& Bucket::operator=(const Bucket& other) {
  if (this != &other) {
    this->row_ = other.row_;
    this->bucket_number_ = other.bucket_number_;
  }
  return *this;
}

Record& Bucket::record() {
  return this->rec;
}

size_t Bucket::bucket_number() const {
  return this->bucket_number_;
}
