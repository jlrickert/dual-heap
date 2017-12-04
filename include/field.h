#ifndef FIELD_H
#define FIELD_H

#include <string>
#include "types.h"


class Field {
public:
  const std::string key;

  /**
   * Construct Field
   *
   * @param data
   * @param key
   */
  Field(std::string str, key_pair_t key_pair);
  Field(int integer, key_pair_t key_pair);
  Field(const Field& other);

  /**
   * String representation of the field;
   *
   * @return convert.str(), std::string(this->data.string->c_str())
   */
  std::string str() const;

  /**
   * Key related to field;
   *
   * @return
   */
  std::string key_name() const;

  /**
   * < operator applied to Field
   *
   * @param other
   * @return this->data.integer < other.data.integer, or *(this->data.string) < *(other.data.string)
   */
  bool operator<(const Field& other) const;

  /**
   * > operator applied to Field
   *
   * @param other
   * @return this->data.integer > other.data.integer, or *(this->data.string) > *(other.data.string)
   */
  bool operator>(const Field& other) const;

  /**
   * <= operator applied to Field
   *
   * @param other
   * @return !(*this > other)
   */
  bool operator<=(const Field& other) const;

  /**
   * <= operator applied to Field
   *
   * @param other
   * @return !(*this < other)
   */
  bool operator>=(const Field& other) const;

  /**
   * == opeartor applied to Field
   *
   * @param other
   * @return this->data.integer == other.data.integer, or *(this->data.string) == *(other.data.string)
   */
  bool operator==(const Field& other) const;
  bool operator!=(const Field& other) const;
private:
  key_pair_t key_pair;

  //
  int integer;
  std::string string;


  void panic_on_bad_compare(const Field& other) const;
};

#endif
