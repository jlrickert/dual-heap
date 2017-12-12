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
    Collection collection(parser.input_file());
    collection.sort(parser.output_file(), keys);

    // vector<string> keys = collection.keys();
    // vector<string> keys_to_sort_by;
    // keys_to_sort_by.push_back("Email");
    // keys_to_sort_by.push_back("Full Name");

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

  } else {
    if (parser.get_error() != "help") {
      cout << parser.get_error() << endl;
    }
    cout << parser.help() << endl;
    return 0;
  }

  return 0;
}
