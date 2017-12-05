#include <iostream>
#include <sstream>

#include "record.h"
#include "bucket.h"
#include "record.h"

using namespace std;

Bucket::Bucket(Record record, size_t bucket_number)
    : rec(record), bucket_number_(bucket_number) {
}

Bucket::Bucket(const Bucket& other)
  : rec(other.rec),
    bucket_number_(other.bucket_number_) {
}

Bucket& Bucket::operator=(const Bucket& other) {
  if (this != &other) {
    this->row_ = other.row_;
    this->bucket_number_ = other.bucket_number_;
  }
  return *this;
}

Record Bucket::record() {
  return this->rec;
}

size_t Bucket::bucket_number() const {
  return this->bucket_number_;
}

string Bucket::str() {
  ostringstream ss;
  ss << "Bucket(Row=" << this->record().row()
     << ", Bucket Number=" << this->bucket_number()
     << ")";
  return ss.str();
}

ostream& operator<<(ostream& stream, Bucket& bucket) {
  stream << bucket.str();
  return stream;
}
