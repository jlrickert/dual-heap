#include <string>
#include <iostream>

#include "collection.h"
#include "utilities.h"
#include "secondary_index.h"

using namespace std;

int main(int argc, char** argv) {
  (void) argc; (void) argv;

  Collection collection("test/fixtures/data1.csv");
  vector<string> keys = collection.keys();
  vector<string> keys_to_sort_by;
  keys_to_sort_by.push_back("Email");
  keys_to_sort_by.push_back("Full Name");
  collection.sort("tmp/sorted.csv", keys_to_sort_by);

  SecondaryIndex full_name_index(collection, keys[0]);
  Record record = collection.get(69);
  full_name_index.rebuild();
  full_name_index.insert(collection.get(23));
  full_name_index.insert(collection.get(33));
  full_name_index.insert(collection.get(43));
  full_name_index.insert(collection.get(53));
  // full_name_index.insert(collection.get(63));
  cout << full_name_index.get(record.get(keys[69]))
    .row();
  // // collection.sort("tmp/sorted2.csv", keys);
  // cout << stringify(collection.keys());
  // cout << collection[0]["Full Name"].str() << endl;
  // cout << collection[1]["Country"].str() << endl;
  // std::cout << collection[0]["Full Name"].str() << std::endl;
  // std::cout << collection[2]["Country"].str() << std::endl;
  // collection[2];
  // collection[3];
  // collection[4];
  // Field field = collection[0]["Full Name"];
  // std::cout << field.str();

  return 0;
}
