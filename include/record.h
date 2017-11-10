#ifndef RECORD_H
#define RECORD_H
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "index.h"

class Field {
public:
  enum Type {
    STRING,
    INTEGER,
    // DATE,
  };
  const Type field_type;
  union Data {
    char* str;
    int num;
    // Date date;
  } data;
private:
};

typedef std::map<std::string, int> Record;

#endif
