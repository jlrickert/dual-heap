#include <sstream>
#include <string>
#include <stdexcept>
#include "field.h"
#include "types.h"

using namespace std;

Field::Field(std::string str, key_pair_t key_pair) {
  this->string = str;
  this->key_pair = key_pair;
}

Field::Field(int integer, key_pair_t key_pair) {
  this->integer = integer;
  this->key_pair = key_pair;
}

Field::Field(const Field& other) {
  this->key_pair = other.key_pair;
  switch (other.key_pair.second) {
  case STRING: {
    this->string = other.string;
    break;
  }
  case INTEGER: {
    this->integer = other.integer;
    break;
  }
  }
}

string Field::str() const {
  switch (this->key_pair.second) {
  case INTEGER: {
    ostringstream convert;
    convert << this->integer;
    return convert.str();
   break;
  }
  case STRING: {
    return this->string;
    break;
  }
  }
  return string("This is impossible");
};

void Field::panic_on_bad_compare(const Field& other) const {
  if (this->key_pair.second != other.key_pair.second) {
    stringstream ss;
    ss << "Fields " << this->key_pair.first << " and " << other.key_pair.first
      << " Cannot be logically compared for order";
    throw logic_error(ss.str());
  }
}

bool Field::operator<(const Field& other) const {
  this->panic_on_bad_compare(other);
  switch (this->key_pair.second) {
  case INTEGER: {
    return this->integer < other.integer;
    break;
  }
  case STRING: {
    return this->string < other.string;
    break;
  }
  }
  throw logic_error("This should be impossible.");
}

bool Field::operator>(const Field& other) const {
  this->panic_on_bad_compare(other);

  switch (this->key_pair.second) {
  case INTEGER: {
    return this->integer > other.integer;
    break;
  }
  case STRING: {
    return this->string > other.string;
    break;
  }
  }
  throw logic_error("This should be impossible.");
}

bool Field::operator<=(const Field& other) const {
  return !(*this > other);
}

bool Field::operator>=(const Field& other) const {
  return !(*this < other);
}

bool Field::operator==(const Field& other) const {
  this->panic_on_bad_compare(other);
  if (this->key_pair.second == other.key_pair.second) {
    switch (this->key_pair.second) {
    case INTEGER: {
      return this->integer == other.integer;
      break;
    }
    case STRING: {
      return this->string == other.string;
      break;
    }
    }
    return true;
  }
  return false;
}
