#ifndef DATABASE_H
#define DATABASE_H
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "index.h"

enum DBFormat {
  CSV,
  // JSON,
  BINARY_V0,
};

enum QueryOptionType {
  CAPACITY,
};

union QueryOptionData {
  int capacity;
};

enum FieldType {
  INTEGER,
  CHAR32,
};

union FieldData {
  std::string* string;
  int number;
};

struct Field {
  const FieldType field;
  const FieldData data;
  Field(FieldType field, FieldData data) : field(field), data(data) {};
};

class Database {
public:
  class Record {
  public:
    Record(const Database& db, int row);
  private:
    std::map<std::string, Field> fields;
  };

  class Query {
  public:
    Query(const Database& db) : db(db) {};
    // const Query& where();
    const Query& select(std::string key);
    const Query& set_option(QueryOptionType opt, QueryOptionData value);
    const Query& order_by(std::string key);
    Database all();
  private:
    std::vector<std::string> selects;
    std::vector<std::string> orders;
    int capacity;
    const Database& db;
  };

  const std::string filename;
  const DBFormat format;

  Database(std::string filename, DBFormat fmt);
  ~Database();

  const Record& get(int n);
  Query query();

private:
  std::map<std::string, FieldType> schema;
  std::ifstream input_stream;
  std::ofstream output_stream;

  // Index index;

  void initialize();
};

#endif
