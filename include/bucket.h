#ifndef BUCKET_H
#define BUCKET_H

#include "record.h"
#include "bucket.h"

class Bucket {
public:
  Bucket(Record record, size_t bucket_number);
  Bucket(const Bucket& other);
  Bucket& operator=(const Bucket& other);
  Record record() const;
  size_t bucket_number() const;
  std::string str() const;
private:
  Record rec;
  size_t row_;
  size_t bucket_number_;

  friend std::ostream& operator<<(std::ostream& stream, Bucket& rec);
};

std::ostream& operator<<(std::ostream& stream, Bucket& rec);

#endif
