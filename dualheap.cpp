#include "simple.h"
#include <string>
// #include "lib.h"


int main(int argc, char** argv) {
  (void) argc; (void) argv;

  try {
    Record rec(std::string("test/fixtures/data1.csv"));
    std::vector<std::string> keys = rec.keys();
    // for (std::vector<std::string>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    //   std::cout << *it << std::endl;
    // }
  } catch (ErrorCode n) {
    switch (n) {
    case FILE_DOESNT_EXIST_ERROR: {
      std::cout << "File doesn't exist" << std::endl;
      break;
    }
    case PARSE_ERROR: {
      std::cout << "Unable to parse csv file" << std::endl;
      break;
    }
    case INVALID_FIELD_TYPE: {
      std::cout << "Invalid field type" << std::endl;
      break;
    }
    }
  }

  return 0;
}
