// #ifndef ARGPARSER_H
// #define ARGPARSER_H

// #include "string"
// #include "vector"

// /// Expected usage from the command line
// /// $> dual_heap -o sorted_data.csv data.csv -k "Full Name,Last Name,Age"
// ///
// /// How to use the class
// /// ```
// /// int main(int argc, char* argv[]) {
// ///   ArgParser parser(argc, argv);
// ///   vector<string> keys = parser.keys();
// ///   string input_file = parser.input_file();
// ///   string output_file = parser.output_file();
// /// }
// /// ```
// class ArgParser {
// public:
//   ArgParser(int argc, char* argv[]);
//   std::string keys();
//   std::string input_file();
//   std::string output_file();
// private:
//   std::vector<std::string> raw;
//   std::vector<std::string> init_raw(int argc, char* argv[]);
// };

// #endif
