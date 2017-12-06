#ifndef BUCKET_MANAGER_H
#define BUCKET_MANAGER_H

#include "collection.h"
#include "bucket.h"
#include "types.h"

class BucketManager {
public:
  BucketManager(Collection& collection, std::fstream& buffer,
                std::vector<int> buckets_sizes);
  void init();
  Bucket pop();
  void heapify(std::vector<std::string> keys);
  bool finished();
  int compare(const Bucket& a, const Bucket& b,
              std::vector<std::string> keys) const;
  std::string buckets_str();
 private:
  Collection& collection;
  std::vector<Bucket> buckets_;
  std::vector<int> bucket_sizes_;
  std::vector<size_t> offsets_;
  std::vector<size_t> cursors;
  std::fstream& sorted_buffer;
  std::vector<size_t> find_offsets(const std::vector<int>& bucket_sizes);
  Bucket fetch(size_t bucket_number);
};

#endif
