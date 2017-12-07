// #include "simple.h"
#include <string>
#include <iostream>
#include "collection.h"
#include "utilities.h"
#include "secondary_index.h"


using namespace std;

int main(int argc, char** argv) {
  (void) argc; (void) argv;

  Collection collection("test/fixtures/data1.csv");
  // SecondaryIndex full_name_index(collection, collection.keys()[0]);
  // full_name_index.rebuild();
  vector<string> keys;
  // keys.push_back("Email");
  keys.push_back("Full Name");
  // collection.sort("tmp/sorted2.csv", keys);
  collection.sort("tmp/sorted.csv", keys);
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
