#ifndef BPTREE_H
#define BPTREE_H
#include <vector>
#include <string>
#include <stdexcept>

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
      KeyError(std::string msg) throw() : msg(msg) {};
      const char* what() const throw() { return msg.c_str(); }
    private:
      std::string msg;
    };
    const int degree;
    Map(int degree) throw() : degree(degree) {};
    void set(K key, V value) throw();
    V get(K key) const;
    V get(K key, V default_) const throw();
    void remove(K key) throw();
  private:
    Node<K, V> root;
    int length;
    int depth;
    int b;
  };
};

#endif // BPTREE_H
