#ifndef INDEX_H
#define INDEX_H

#include <fstream>

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
  size_t get(size_t i);

  /**
   * Return the offset of record at position i.
   *
   * @param i
   * @return offset
   */
  size_t operator[](size_t i);

  ~Index();

  /**
   * Returns the size of the index
   *
   * @ return this->size_
   */
  size_t size() const;
private:
  /**
   * Word size of entry in the index.
   */
  typedef size_t Word;

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
