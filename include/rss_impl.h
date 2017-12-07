#ifndef RSS_IMPL_H
#define RSS_IMPL_H

#include <vector>

#include "collection.h"
#include "record.h"
#include "types.h"

class RSSImpl {
public:
  RSSImpl(Collection& coll, const std::vector<std::string>& keys);
  void init();
  bool finished();
  Record pop();
  void heapify();
  std::vector<size_t> bucket_counts();
  std::string stringify_heap();
  std::string stats();
private:
  Collection& coll_;
  size_t active_;
  size_t bucket_size_;
  std::vector<Record> heap_;
  size_t counter_;
  std::vector<size_t> bucket_counts_;
  std::vector<std::string> keys_;

  void update(Record& last);
  Record fetch(row_t i);
};

#endif
