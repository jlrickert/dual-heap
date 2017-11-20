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
   /**
   * Database::Record::Record
   *
   * @param db Database
   * @param row Row number
   */
    Record(const Database& db, int row);
  private:
    std::map<std::string, Field> fields;
  };

  class Query {
  public:
    Query(const Database& db) : db(db) {};
    // const Query& where();
    /**
    * Query::select
    *
    * @param key A string
    */
    const Query& select(std::string key);
    /**
    * Query::set_option Field
    *
    * @param opt Query option type
    * @param value Query option data value
    */
    const Query& set_option(QueryOptionType opt, QueryOptionData value);
    /**
    * Query::order_by
    *
    * @param key Order by given key
    */
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
  /**
   * Database::Database
   *
   * @param filename String of the filename
   * @param fmt Database format
   */
  Database(std::string filename, DBFormat fmt);
  ~Database();

  /**
  * Database::get 
  *
  * @param n Integer n
  * @return
  */
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
