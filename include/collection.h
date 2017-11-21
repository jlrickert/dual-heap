#ifndef COLLECTION_H
#define COLLECTION_H

#include <map>
#include <vector>
#include "record.h"
#include "types.h"
#include "index.h"

class Record;

class Collection {
public:
  const static size_t HEAP_SIZE = 3;
  const std::string file_name;
  /**
   * Constructs Collection::Collection, initializes file, keys, and index
   *
   * @param file_name
   */
  Collection(const std::string& file_name);
  Collection(const Collection& collection);
  ~Collection();

  /**
   * Creates a new sorted CSV file at the location `output_file` sorted in the
   * order of entries in `keys`.
   *
   * @param file_name
   * @param keys
   */
  void sort(std::string output_file, std::vector<std::string> keys);

  /**
   * Return a vector of keys available in the associated CSV file. The order of
   * the keys also corresponds to the order of keys in a record.
   *
   * @return std::vector<std::string> keys
   */
  std::vector<std::string> keys() const;

  Record get(size_t row);
  Record operator[](size_t row);

  /**
   * Return the number of records in a collection
   *
   * @return count of records
   */
  size_t size() const;
private:
  friend class Record;
  Index* index;
  std::ifstream input;
  std::vector<key_pair_t> keys_;  // this preserves order of keys

  // these do not preserve order
  std::map<std::string, key_pair_t> key_index;
  std::map<std::string, size_t> key_pos_index;

  /**
   * this->input.seekg(pos)
   *
   * @param pos
   */
  void seek(size_t pos);

  /**
   * @return this->input.tellg()
   */
  size_t cur_pos();

  std::fstream& replacement_selection_sort(std::fstream& buffer,
                                           std::fstream& offsets,
                                           std::vector<std::string> keys);
  std::fstream& kway_merge(std::fstream& input,
                           std::fstream& output,
                           std::fstream& offsets,
                           std::vector<std::string> keys);

  /**
   * Initializes file by opening file-name
   */
  void init_file();

  /**
   * Parse and initializes keys
   */
  void init_keys();

  /**
   * Generate and index of the csv file
   */
  void init_index();
};

#endif
