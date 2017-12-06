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
    this->rec = other.rec;
    this->bucket_number_ = other.bucket_number_;
  }
  return *this;
}

Record Bucket::record() const {
  return this->rec;
}

size_t Bucket::bucket_number() const {
  return this->bucket_number_;
}

size_t Bucket::row() const {
  return this->rec.row();
}

string Bucket::str() const {
  ostringstream ss;
  ss << "(";
  ss << this->row();
  ss << ", " << this->bucket_number();
  ss << ")";
  // ss << "Bucket(Row=" << this->record().row()
  //    << ", BN=" << this->bucket_number()
  //    << ")";
  return ss.str();
}

ostream& operator<<(ostream& stream, Bucket& bucket) {
  stream << bucket.str();
  return stream;
}
