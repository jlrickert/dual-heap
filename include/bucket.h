#ifndef BUCKET_H
#define BUCKET_H

#include "bucket.h"
#include "collection.h"
#include "record.h"

class Record;

class Bucket {
public:
  Bucket(Record& record, size_t bucket_number);
  Bucket(const Bucket& other);
  Bucket& operator=(const Bucket& other);
  Record& record();
  size_t bucket_number() const;
private:
  Record& rec;
  size_t row_;
  size_t bucket_number_;
};

#endif
