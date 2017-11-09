#include <iostream>
#include <fstream>
#include <vector>
#include <map>

const char HEADER_DELIM = ',';

enum FieldType {
  INTEGER,
  STRING,
};

union FieldData {
  int integer;
  char* string;
};

enum ErrorCode {
  FILE_DOESNT_EXIST_ERROR,
  PARSE_ERROR,
  INVALID_FIELD_TYPE,
};

class Record {
public:
  typedef std::pair<std::string, FieldType> Key;
  class Row {
  public:
    Row(int x);
  private:
  };
  // typedef std::map<Key, FieldData> Row;

  const char* file_name;

  Record(const std::string& file_name);
  ~Record();
  Row get(int offset);
  void sort(std::string);
  size_t row_count();
  std::vector<std::string> keys() const;
private:
  // typedef std::map<std::string, FieldType> KeyMap;
  // KeyMap key_map;
  std::vector<Key> keys_;
  std::fstream file;
  // std::vector<FieldData> data;
  std::vector<size_t> index;

  void init_fstream();
  void init_keys();
  void init_index();
};
