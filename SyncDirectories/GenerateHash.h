#pragma once
#include <string>
#include <vector>
#include "HashValue.h"
using std::string;

void GenerateHashForFile(const string & path_to_file, uint32_t * hash_buffer);

void GenerateHashForFile(const string & path_to_file, std::vector<HashValue> & hash_buffer);

void GenerateHash(const char * data, unsigned long long dataSize, uint32_t * hash_buffer);