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
   * Key is a metadata object that holds information about a field is in a
   * record.
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

  Field(const Field& other) : data(other.data), key(other.key) {}
  ~Field();
  /**
   * String representation of the field;
   *
   * @return convert.str(), std::string(this->data.string->c_str())
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
   * Create an index from a given csv file.
   *
   * @param file-name name of the csv file
   */
  static Index* from_csv(std::string file_name);

  /**
   * Return the offset of record at position i.
   *
   * @param i
   * @return offset
   */
  size_t get(size_t i);

  /**
   * Return the offset of record at position i.
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
   * Word size of entry in the index.
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
     * Collection::Record::offset is the offset of the record.
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
    bool lt(Record& other, std::vector<Key> keys);
    bool lte(Record& other, std::vector<Key> keys);
    bool gt(Record& other, std::vector<Key> keys);
    bool gte(Record& other, std::vector<Key> keys);
    std::string str();
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

  const static size_t HEAP_SIZE = 3;
  const std::string file_name;
  /**
   * Constructs Collection::Collection, initializes file, keys, and index
   *
   * @param file_name
   */
  Collection(const std::string& file_name);
  Collection(const Collection& collection);
  ~Collection();

  /**
   * Creates a new sorted CSV file at the location `output_file` sorted in the
   * order of entries in `keys`.
   *
   * @param file_name
   * @param keys
   */
  void sort(std::string output_file, std::vector<std::string> keys);
  void sort(std::string output_file, std::vector<Key> keys);
  /**
   * Return a vector of keys available in the associated CSV file. The order of
   * the keys also corresponds to the order of keys in a record.
   *
   * @return std::vector<std::string> keys
   */
  const std::vector<std::string>& keys() const;

  Record get(size_t row);
  Record operator[](size_t row);

  /**
   * Return the number of records in a collection
   *
   * @return count of records
   */
  size_t size() const;
private:
  friend class Record;

  std::ifstream input;
  std::vector<std::string> keys_;
  std::map<std::string, Key> key_type_map;
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

  typedef short unsigned BucketSize_t;
  std::fstream& replacement_selection_sort(std::fstream& buffer,
                                           std::vector<BucketSize_t>& bucket_sizes,
                                           std::vector<Key> keys);
  std::fstream& kway_merge(std::fstream& input, std::fstream& output,
                           std::vector<BucketSize_t>& bucket_sizes,
                           std::vector<Key> keys);
  std::fstream& kway_merge_pass(std::fstream& input, std::fstream& output,
                                std::vector<size_t>& bucket_offsets,
                                std::vector<BucketSize_t>& bucket_sizes,
                                std::vector<Key> keys);

  /**
   * Initializes file by opening file-name
   */
  void init_file();

  /**
   * Parse and initializes keys
   */
  void init_keys();

  /**
   * Generate and index of the csv file
   */
  void init_index();

  std::vector<size_t> find_offsets(const std::vector<BucketSize_t>& bucket_sizes);
};
