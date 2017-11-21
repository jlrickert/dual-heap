#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>  // defines size_t
#include <utilities.h>

/**
 * Character that delimits keys
 */
const char CSV_DELIM = ',';

// this collides with key_t somewhere
enum Tkey {
  INTEGER,
  STRING,
};


typedef std::pair<std::string, Tkey> key_pair_t;


const size_t HEAP_SIZE = 3;

#endif
