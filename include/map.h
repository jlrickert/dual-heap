#ifndef MAP_H
#define MAP_H
#include <vector>
#include <string>
#include <stdexcept>

template<typename K, typename V, unsigned int degree = 6>
class Map {
public:
  class KeyError : public std::runtime_error {
  public:
    KeyError(std::string msg) throw() : msg(msg) {};
    const char* what() const throw() { return msg.c_str(); }
  private:
    std::string msg;
  };
  Map() {};

  // overwrites when key already exists
  void insert(const K& key, V value) throw();
  V& get(const K& key) throw();
  V& get(const K& key, V default_);
  void remove(const K& key);
private:
  struct LeafNode {
    K keys[degree];
    size_t num_keys;
    LeafNode() : num_keys(0) {};
    ~LeafNode();
  };
  struct InnerNode {
    K keys[degree];
    void* children[degree + 1];  // use reinterpret_cast
    size_t num_keys;
    InnerNode() : num_keys(0) {};
    ~InnerNode();
  };
  void* root;  // either an InnerNode or a LeafNode. Use reinterpret_cast. Ex.
               // root = reinterpret_cast<InnerNode*>(new InnerNode())
  size_t depth;
};

#endif // MAP_H
