#pragma once
#include <string>
using std::string;

void GenerateHash(const string & path_to_file, uint32_t * hash_buffer);

void GenerateHash(const unsigned char * data, unsigned long long dataSize, uint32_t * hash_buffer);