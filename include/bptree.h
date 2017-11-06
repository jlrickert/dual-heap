#ifndef BPTREE_H
#define BPTREE_H
#include <vector>

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
    const int degree;
    Map(int degree) : degree(degree) {};
    void insert(K key, V value);
    void remove(K key);
    V find(K key);
  private:
    Node<K, V> root;
    int length;
    int depth;
    int b;
  };
};

#endif // BPTREE_H
