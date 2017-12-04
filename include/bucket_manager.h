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
  Record& pop();
  void heapify();
  bool finished();
private:
  Collection& collection;
  std::vector<Bucket> buckets_;
  std::vector<int> bucket_sizes_;
  std::vector<size_t> bucket_offsets_;
  std::vector<size_t> cursors;
  std::fstream& sorted_buffer;
  std::vector<size_t> find_offsets(const std::vector<int>& bucket_sizes);
};

#endif
