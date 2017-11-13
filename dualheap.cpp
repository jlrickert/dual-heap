// #include "simple.h"
#include <string>
#include "collection.h"
// #include "lib.h"

int main(int argc, char** argv) {
  (void) argc; (void) argv;

  Collection collection(std::string("test/fixtures/data1.csv"));
  // std::cout << collection.size() << std::endl;
  std::cout << collection[1]["Full Name"].str() << std::endl;
  std::cout << collection[1]["Country"].str() << std::endl;
  // std::cout << collection[2]["Full Name"].str() << std::endl;
  // std::cout << collection[2]["Country"].str() << std::endl;
  // collection[2];
  // collection[3];
  // collection[4];
  // Field field = collection[0]["Full Name"];
  // std::cout << field.str();
  // try {
  //   Record rec(std::string("test/fixtures/data1.csv"));
  //   std::vector<Key> keys = rec.keys();
  //   Key full_name_key = rec.lookup_key("Full Name");
  //   Record::Row row = rec.fetch(0);
  //   Field field = row.fetch(full_name_key);
  //   std::cout << field.str();
  //   // rec.sort("file_name", std::vector<Key>(keys[0], keys[3]));
  //   // for (std::vector<Record::Key>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
  //   //   std::cout << it->name << " " << it->type << std::endl;
  //   // }
  // } catch (ErrorCode n) {
  //   switch (n) {
  //   case FILE_DOESNT_EXIST_ERROR: {
  //     std::cout << "File doesn't exist" << std::endl;
  //     break;
  //   }
  //   case PARSE_ERROR: {
  //     std::cout << "Unable to parse csv file" << std::endl;
  //     break;
  //   }
  //   case INVALID_FIELD_TYPE: {
  //     std::cout << "Invalid field type" << std::endl;
  //     break;
  //   }
  //   case KEY_ERROR: {
  //     std::cout << "Invalid key" << std::endl;
  //     break;
  //   }
  //   }
  // }

  return 0;
}
