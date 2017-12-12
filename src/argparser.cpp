#include <sstream>
#include "argparser.h"

using namespace std;

string ArgParser::help() {
  return "This is a help message";
}

ArgParser::ArgParser(int argc, char* argv[]) : args_(stringify_args(argc, argv)) {
  this->parse();
};

bool ArgParser::good() const {
  return this->good_;
}

string ArgParser::get_error() const {
  return this->error_;
}

vector<string> ArgParser::keys() const {
  return this->keys_;
}

string ArgParser::input_file() const {
  return this->input_file_;
}

string ArgParser::output_file() const {
  return this->output_file_;
}

vector<string> ArgParser::stringify_args(int argc, char* argv[]) {
  vector<string> args;
  for (int i = 1; i < argc; i += 1) {
    args.push_back(string(argv[i]));
  }
  return args;
}

enum State {
  DEFAULT,
  OUTPUT_FILE,
  KEYS,
  HELP,
  DONE,
};

void ArgParser::parse() {
  this->good_ = true;

  State state = DEFAULT;
  bool error = false;

  for (size_t i = 0; i < this->args_.size(); i += 1) {
    string arg = this->args_[i];
    switch (state) {
    case DEFAULT: {
      if (arg == "-o") {
        state = OUTPUT_FILE;
      } else if (arg == "-k") {
        state = KEYS;
      } else if (arg == "-h") {
        state = HELP;
      } else {
        if (this->input_file_ == "") {
          this->input_file_ = arg;
        } else {
          this->error_ = "Can only have 1 input file";
          error = true;
        }
        state = DEFAULT;
      }
      break;
    }
    case OUTPUT_FILE: {
      if (this->output_file_ == "") {
        this->output_file_ = arg;
      } else {
        this->error_ = "Can only have 1 output file";
        error = true;
      }
      state = DEFAULT;
      break;
    }
    case KEYS: {
      ostringstream ss;
      for (string::iterator ch = arg.begin(); ch != arg.end(); ch += 1) {
        if (*ch != ',') {
          ss << *ch;
        } else {
          this->keys_.push_back(ss.str());
          ss.str("");
          ss.clear();
        }
      }
      this->keys_.push_back(ss.str());

      state = DEFAULT;
      break;
    }
    case HELP: {
      this->error_ = "help";  // need a better method for letting the program
                              // know that the use wants a help message
      this->good_ = false;
      return;
    }
    case DONE: {
      this->good_ = true;
      return;
    }
    }

    if (error) {
      this->good_ = false;
      return;
    }
  }

  if (this->input_file_ == "") {
    this->error_ = "An input file is required";
    this->good_ = false;
  } else if (this->output_file() == "") {
    this->error_ = "An output file is required";
    this->good_ = false;
  }
}
