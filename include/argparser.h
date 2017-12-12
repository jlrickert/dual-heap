#ifndef ARGPARSER_H
#define ARGPARSER_H

#include "string"
#include "vector"

/// Expected usage from the command line
/// $> dual_heap -o sorted_data.csv data.csv -k "Full Name,Last Name,Age"
///
/// How to use the class
/// ```
/// int main(int argc, char* argv[]) {
///   ArgParser parser(argc, argv);
///   vector<string> keys = parser.keys();
///   string input_file = parser.input_file();
///   string output_file = parser.output_file();
/// }
/// ```
class ArgParser {
public:
  static std::string help();
  ArgParser(int argc, char* argv[]);

  bool good() const;
  std::string get_error() const;

  std::vector<std::string> keys() const;
  std::string input_file() const;
  std::string output_file() const;
private:
  std::vector<std::string> args_;
  bool good_;
  std::string error_;

  std::vector<std::string> keys_;
  std::string input_file_;
  std::string output_file_;

  void parse();
  std::vector<std::string> stringify_args(int argc, char* argv[]);
};

#endif
