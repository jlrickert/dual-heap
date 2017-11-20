#ifndef BPTREE_H
#define BPTREE_H
#include <vector>
#include <string>
#include <stdexcept>

/**
 * A Namespace containing the Map class
 */
namespace BPlusTree {
  template<typename K, typename V>
  struct Node {
    std::vector<K> keys;
    std::vector<Node<K, V> > edges;
    std::vector<V> values;
  };

  template<typename K, typename V>
  class Map {
  public:
    class KeyError : public std::runtime_error {
    public:
      /**
      * Map::Key Error
      *
      * @throw msg
      * @param msg
      */
      KeyError(std::string msg) throw() : msg(msg) {};
      const char* what() const throw() { return msg.c_str(); }
    private:
      std::string msg;
    };
    const int degree;
    Map(int degree) throw() : degree(degree) {};
    /**
    * Map::set 
    *
    * @param key
    * @param value
    * @throw
    */
    void set(K key, V value) throw();
    /**
    * Map::get key
    *
    * @param key
    */
    V get(K key) const;
    V get(K key, V default_) const throw();
  /**
   * Map::Remove key
   *
   * @param key
   * @throw 
   */
    void remove(K key) throw();
  private:
    Node<K, V> root;
    int length;
    int depth;
    int b;
  };
};

#endif // BPTREE_H
