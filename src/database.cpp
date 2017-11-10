#include "record.h"

// const Database::Query& Database::Query::set_option(QueryOptionType opt,
//                                                    QueryOptionData value) {
//   switch (opt) {
//   case CAPACITY: {
//     this->capacity = value.capacity;
//     break;
//   }
//   default:
//     break;
//   }
//   return *this;
// }

// const Database::Query& Database::Query::select(std::string key) {
//   this->selects.push_back(key);
//   return *this;
// }

// const Database::Query& Database::Query::order_by(std::string key) {
//   this->orders.push_back(key);
//   return *this;
// }

// Database::Database(std::string filename, DBFormat fmt)
//   : filename(filename),
//     format(fmt) {
//   this->initialize();
// }

// Database::~Database() {
//   input_stream.close();
// }

// Database::Query Database::query() {
//   input_stream.open(filename.c_str(), std::ios::in);
//   output_stream.open("/tmp/sort_file", std::ios::out);

//   input_stream.close();
//   output_stream.close();
//   Database::Query query = Database::Query(*this);
//   return query;
// }

// void Database::initialize() {
// }
