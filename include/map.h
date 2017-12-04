#ifndef MAP_H
#define MAP_H
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

/// Example usage
/// ```
/// #include "map.h"
/// int main() {
///   Map<string, int> index();
///    index.insert("Jack", 5);
///    int rrn = -1;
///    try {
///       rrn = index.get("Jack");
///       // do something with rnn
///
///       rrn = index.get("Jill");
///       // do something with rnn  
///   } catch (index::KeyError e) {
///     cout << "key not found";
///   }

/// }
/// ```

template<typename K, typename V, unsigned int degree = 6>
class Map {
public:
/////////////////////////////////////////////////////////<-START class KeyError
  class KeyError : public std::runtime_error {
  public:
    KeyError(T item) throw() : item(item) {};

    //Error thrown when the key is invalid. Prints out what the invalid key is.
    const char* what() const throw() { 
        std::ostringstream ss;
        ss << "\"" << item << "\"" << " is not a valid key";
        return ss.str(); 
      }

  private:
    T item;
  };
///////////////////////////////////////////////////////<-END class KeyError
  
///////////////////////////////////////////////////////<-START class Map  
  Map() {
    this->root = new InnerLeaf();
  };

// Overwrites when key already exists
  void insert(const K& key, V value) throw();
  
// Throws an error if the key is invalid or not found.
  V get(const K& key) throw() {
    throw new KeyError(key) 
    // Code here
  }

/* Clutter - Pretty much don't need this.
//If the key is not found, it puts the default in there.  
  V get(const K& key, V default_){
      try {
        return this->get(key);
      }  catch (KeyError e) {
        return default_
      }
  };

  void remove(const K& key){
  };
*/

private:
  struct LeafNode {
    K keys[degree];
    size_t num_keys;
    LeafNode* next;
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
//////////////////////////////////////////////////////<-END class Map

#endif // MAP_H