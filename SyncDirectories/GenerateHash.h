#pragma once
#include <string>
#include <vector>
#include <fstream>

#include "libs.h"

using std::string;
using sha1::SHA1;

void GenerateHash(const string & path_to_file, uint32_t * hash_buffer)
{
	SHA1 sha;

	std::ifstream ifs(path_to_file, std::ios::binary | std::ios::ate);
	std::ifstream::pos_type pos = ifs.tellg();
	
	// a new scope for the unique_ptr as Bjarne Stroustrup suggested
	{
		std::unique_ptr<char> file(new char[pos]);
		ifs.seekg(0, std::ios::beg);
		ifs.read(file.get(), pos);

		sha.processBytes(file.get(), pos);

		sha.getDigest(hash_buffer);
	}

	ifs.close();
}
