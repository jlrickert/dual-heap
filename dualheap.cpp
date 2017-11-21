// #include "simple.h"
#include <string>
#include <iostream>
#include "collection.h"
#include "utilities.h"
// #include "lib.h"

using namespace std;

int main(int argc, char** argv) {
  (void) argc; (void) argv;

  Collection collection(string("test/fixtures/data1.csv"));
  vector<string> keys;
  keys.push_back("Full Name");
  keys.push_back("Email");
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
