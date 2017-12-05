#ifndef UTILITIES_H
#define UTILITIES_H

#include <sstream>
#include <vector>

namespace Util {
template<typename WordSize>
std::ostream& write_raw(std::ostream& outs, WordSize value) {
  for (size_t size = sizeof(WordSize); size; --size, value >>= 8) {
    outs.put((char)(value & 0xFF));
  }
  return outs;
}

template<typename WordSize>
std::istream& read_raw(std::istream& ins, WordSize& value) {
  value = 0;
  size_t length = sizeof(WordSize);
  for (size_t i = 0; i < length; i++) {
    int byte = ins.get() << (8 * i);
    value |= byte;
  }
  return ins;
}

template <typename T>
std::string stringify(const std::vector<T>& vec) {
  if (vec.size() == 0) {
    return "[]";
  }

  std::ostringstream ss;
  ss << "[";
  for (size_t i = 0; i < (vec.size() - 1); i += 1) {
    ss << vec[i] << ", ";
  }
  ss << vec[vec.size() - 1];
  ss << "]";
  return ss.str();
}

template <typename T>
std::string stringify(const T array[], size_t begin, size_t size) {
  if (size == 0) {
    return "[]";
  }

  std::ostringstream ss;
  ss << "[";
  for (size_t i = begin; i < (size - 1); i += 1) {
    ss << array[i] << ", ";
  }
  ss << array[size - 1];
  ss << "]";
  return ss.str();
}

template <typename T>
std::string stringify(const T array[], size_t size) {
  return stringify<T>(array, 0, size);
}

template <typename T>
std::string stringifyVector(std::vector<T>& vec) {
  if (vec.size() == 0) {
    return "[]";
  }

  std::ostringstream ss;
  ss << "[";
  for (size_t i = 0; i < (vec.size() - 1); i += 1) {
    ss << vec[i] << ", ";
  }
  ss << vec[vec.size() - 1];
  ss << "]";
  return ss.str();
}

template <typename T>
void initialize(T array[], size_t size, T initial = 0) {
  for (size_t i = 0; i < size; i += 1) {
    array[i] = initial;
  }
}
} // end namespace Util

#endif
