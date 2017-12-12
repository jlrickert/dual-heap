#include <string>
#include <iostream>

#include "utilities.h"
#include "argparser.h"
#include "collection.h"
#include "utilities.h"
#include "secondary_index.h"

using namespace std;

int main(int argc, char** argv) {

  ArgParser parser(argc, argv);
  if (parser.good()) {
    vector<string> keys = parser.keys();
    cout << "keys: " << Util::stringifyVector(keys) << endl;
    cout << "Input file: " << parser.input_file() << endl;
    cout << "Output file: " << parser.output_file() << endl;
  } else {
    if (parser.get_error() != "help") {
      cout << parser.get_error() << endl;
    }
    cout << parser.help() << endl;
  }
  // Collection collection("test/fixtures/data1.csv");
  // vector<string> keys = collection.keys();
  // vector<string> keys_to_sort_by;
  // keys_to_sort_by.push_back("Email");
  // keys_to_sort_by.push_back("Full Name");
  // collection.sort("tmp/sorted.csv", keys_to_sort_by);

  // SecondaryIndex full_name_index(collection, keys[0]);
  // Record record = collection.get(69);
  // full_name_index.rebuild();
  // full_name_index.insert(collection.get(23));
  // full_name_index.insert(collection.get(33));
  // full_name_index.insert(collection.get(43));
  // full_name_index.insert(collection.get(53));
  // // full_name_index.insert(collection.get(63));
  // cout << full_name_index.get(record.get(keys[69]))
  //   .row();

  return 0;
}
