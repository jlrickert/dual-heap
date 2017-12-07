#ifndef INDEX_H
#define INDEX_H

#include <fstream>
#include "types.h"

class Index {
public:
  /**
   * Create an index from a given csv file.
   *
   * @param file-name name of the csv file
   */
  static Index* from_csv(std::string file_name);

  /**
   * Return the offset of record at position i.
   *
   * @param i
   * @return offset
   */
  size_t get(row_t i);

  /**
   * Return the offset of record at position i.
   *
   * @param i
   * @return offset
   */
  size_t operator[](row_t i);

  ~Index();

  /**
   * Returns the size of the index
   *
   * @ return this->size_
   */
  row_t size() const;
private:
  /**
   * Get record count from beginning of file
   *
   * @param file_name
   */
  Index(std::string file_name);

  /**
   * Index::file_name is the name of the input csv file
   */
  const std::string file_name;

  size_t size_;

  std::fstream stream;
};

#endif
