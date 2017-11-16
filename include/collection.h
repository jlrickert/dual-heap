#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

/**
* Character that delimits keys
*/
const char DELIM = ',';

class Collection;

struct Key {
  const enum Type {
    INTEGER,
    STRING,
  } type;
  /**
  * Key::pos is the position of the key in the record
  */
  const size_t pos;  // TODO: this should be part of record
  /**
  * Key::name is the name of the key
  */
  const std::string name;
  /**
  * 
  * @param pos Position of the key
  * @param name Name of the key
  * @param type Type of the key
  */
  Key(size_t pos, std::string name, Type type)
    : type(type), pos(pos), name(name) {}
  Key(const Key& other) : type(other.type), pos(other.pos), name(other.name) {}
};

struct Field {
  const union Data {
    int integer;
    std::string* string;
  } data;
  Key key;
  /**
  * Construct Field
  *
  * @param data
  * @param key
  */
  Field(Data data, Key key) : data(data), key(key) {}
  ~Field();
  /**
  * str member function
  *
  * @return convert.str(), std::string(this->data.string->c_str()), or std::string("This is impossible")
  */
  std::string str() const;
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
};

class Index {
public:
  /**
  * Reads keys from a the given file name, outputs to output_file_name.c_str()
  *
  * @param file-name name of the csv file
  */
  static Index* from_csv(std::string file_name);
  /**
  * Gets offset 
  *
  * @param i
  * @return offset
  */
  size_t get(size_t i);
  /**
  * Gets offset 
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
  * Index::Word is the 
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

typedef std::pair<std::string, Key> key_pair_t;

class Collection {
public:
  class Record {
  public:
    /**
    * Cellection::Record::offset is the offset of the record.
    */
    const size_t offset;
    /**
    * Finds key name and gets key
    *
    * @param key_name
    * @return Collection::Record::get(key)
    */
    Field get(std::string key_name);
    /**
    * Gets key from field in record
    *
    * @param key
    */
    Field get(Key key);

    Field operator[](std::string key_name);

    Field operator[](Key key);
  private:
    friend class Collection;
    /**
    * Constructs Collection::Record::Record 
    *
    * @param collection
    * @param offset
    */
    Record(Collection& collection, size_t offset);
    Collection& collection;
  };

  const std::string file_name;
  /**
  * Constructs Collection::Collection, initializes file, keys, and index
  *
  * @param file_name 
  */
  Collection(const std::string& file_name);
  ~Collection();
  /**
  * Sorts keys of string
  *
  * @param str
  * @param keys
  */
  Collection sort(std::string str, std::vector<std::string> keys);
  /**
  * Sorts keys
  *
  * @param str
  * @param keys
  */
  Collection sort(std::string str, std::vector<Key> keys);
  /**
  * Get record from row
  *
  * @param row
  * @return 
  */
  Record get(size_t row);
  Record operator[](size_t row);
  /**
  * Returns the size of the index in the collection
  *
  * @return this->index->size()
  */
  size_t size() const;
private:
  friend class Record;
  std::ifstream input;
  std::map<std::string, Key> keys_;
  Index* index;

  /**
  * this->input.seekg(pos)
  *
  * @param pos
  */
  void seek(size_t pos);
  /**
  * @return this->input.tellg()
  */
  size_t cur_pos();
  /**
  * Initializes file by opening file-name
  */
  void init_file();
  /**
  * Inserts keys
  */
  void init_keys();
  /**
  * Initializes index from csv file
  */
  void init_index();
};
