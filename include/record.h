#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <map>

#include "index.h"
#include "field.h"
#include "collection.h"

class Collection;

class Record {
public:
  /**
   * Constructs Collection::Record::Record
   *
   * @param collection
   * @param offset
   */
  Record(Collection& collection, size_t offset);
  Record(const Record& other);

  /**
   * Collection::Record::offset is the offset of the record.
   */
  const size_t offset;

  /**
   * Return the field with given key
   *
   * @param key_name
   * @return Field
   */
  Field get(std::string key_name);

  /**
   * Gets key from field in record
   *
   * @param key
   */
  Field operator[](std::string key_name);

  bool lt(Record& other, std::vector<std::string> keys);
  bool lte(Record& other, std::vector<std::string> keys);
  bool gt(Record& other, std::vector<std::string> keys);
  bool gte(Record& other, std::vector<std::string> keys);
  std::string str();
private:
  Field get(key_pair_t key);
  Field operator[](key_pair_t key);
  Collection& collection;
};

#endif
