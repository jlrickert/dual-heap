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

struct Key : std::multimap<std::string, FieldType> {
  const std::string name;
  const FieldType t;
  Key(std::string name, FieldType type) : name(name), t(type) {}
  std::string type() const;
};

struct Field {
  const FieldType type;
  const FieldData data;
  Field(FieldType type, FieldData data) : type(type), data(data) {}
  bool operator<(const Field& other) const;
  bool operator<=(const Field& other) const;
  bool operator>(const Field& other) const { return !(*this <= other); }
  bool operator>=(const Field& other) const { return !(*this < other); }

  std::string str();
};

enum ErrorCode {
  FILE_DOESNT_EXIST_ERROR,
  PARSE_ERROR,
  INVALID_FIELD_TYPE,
  KEY_ERROR,
};

class Record {
public:
  const std::string file_name;
  Record(const std::string& file_name);
  ~Record();

  class Row {
  public:
    Row(const Record& rec, size_t offset);
    Field fetch(Key key) const;
    Field fetch(std::string key_name) const;
  private:
    const Record& rec;
    size_t offset;

    Field fetch_by_offset(int pos) const;
  };

  /// Get row by id. This is the row number minus 2; row - 1 from header - 1 from
  /// index by 0.
  Row fetch(int id) const;

  /// Sorts the record
  void sort(std::string tmp_dir);

  /// Get the number of entries in record
  size_t row_count() const;

  /// Get keys used for looking up fields within a row. Also contains metadata
  /// about type information.
  ///
  /// # Example
  ///
  /// Key key = rec.keys
  /// std::string key_name = key.first
  /// FieldType type = key.second
  std::vector<Key> keys() const;

  /// Helper function to get the Key from a string.
  /// Searches for the key in O(log n)
  Key lookup_key(std::string key_name) const;
private:
  std::vector<Key> keys_;
  std::map<Key, size_t> keys_index;  /// Map containing the order of data from
                                    /// the schema
  std::fstream file;
  std::vector<size_t> index;  /// Contains start of each row position in file.
  std::vector<Row> rows;

  void init_fstream();
  void init_keys();
  void init_index();
};
