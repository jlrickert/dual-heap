#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

const char DELIM = ',';

class Collection;
struct Key;

struct Field {
  const enum Type {
    INTEGER,
    STRING,
  } type;
  const union Data {
    int integer;
    std::string* string;
  } data;
  Field(Data data, Type type) : type(type), data(data) {}
  ~Field();
  std::string str();
};

struct Key {
  const size_t pos;  // TODO: this should be part of record
  const std::string name;
  const Field::Type type;
  Key(size_t pos, std::string name, Field::Type type)
    : pos(pos), name(name), type(type) {}
  Key(const Key& other) : pos(other.pos), name(other.name), type(other.type) {}
};

class Index {
public:
  typedef size_t Word;
  static Index* from_csv(std::string file_name);
  size_t get(size_t i);
  size_t operator[](size_t i);
  ~Index();
  size_t size() const;
private:
  Index(std::string file_name);
  const std::string file_name;
  size_t size_;
  std::fstream stream;
};

typedef std::pair<std::string, Key> key_pair_t;

class Collection {
public:
  class Record {
  public:
    const size_t offset;
    Field get(std::string key_name);
    Field get(Key key);
    Field operator[](std::string key_name);
    Field operator[](Key key);
  private:
    friend class Collection;
    Record(Collection& collection, size_t offset);
    Collection& collection;
  };

  const std::string file_name;
  Collection(const std::string& file_name);
  ~Collection();
  Record get(size_t row);
  Record operator[](size_t row);
  size_t size() const;
  // std::vector<std::string> key_names();
  // std::vector<Key> keys();
private:
  friend class Record;
  std::ifstream input;
  std::map<std::string, Key> keys_;
  Index* index;

  void seek(size_t pos);
  size_t cur_pos();

  void init_file();
  void init_keys();
  void init_index();
};
