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
  Record(Collection& collection, size_t row, size_t offset);
  Record(const Record& other);

  Record& operator=(const Record& other);

  /**
   * Returns the row that the record belongs too.
   *
   * @return size_t
   */
  size_t row() const;

  /**
   * Returns the offset of where the file is located.
   *
   * @return size_t
   */
  size_t offset() const;

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

  int compare(Record& other, std::vector<std::string> keys);
  bool lt(Record& other, std::vector<std::string> keys);
  bool lte(Record& other, std::vector<std::string> keys);
  bool gt(Record& other, std::vector<std::string> keys);
  bool gte(Record& other, std::vector<std::string> keys);
  bool eq(Record& other, std::vector<std::string> keys);
  bool neq(Record& other, std::vector<std::string> keys);
  std::string str();
  std::ostream& write(std::ostream& stream);
private:
  size_t row_;
  size_t offset_;

  Field get(key_pair_t key);
  Field operator[](key_pair_t key);
  Collection& collection;

  friend std::ostream& operator<<(std::ostream& stream, Record& rec);
};

std::ostream& operator<<(std::ostream& stream, Record& rec);

#endif
