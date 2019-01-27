#include "GenerateHash.h"
#include "libs.h"

#include <fstream>

using sha1::SHA1;

void GenerateHashForFile(const string & path_to_file, uintmax_t fileSizeInBytes, uint32_t * hash_buffer)
{
	SHA1 sha;
	std::ifstream ifs(path_to_file, std::ios::binary);
	// a new scope for the unique_ptr as Bjarne Stroustrup suggested
	{
		std::unique_ptr<char> file(new char[fileSizeInBytes]);

		ifs.read(file.get(), fileSizeInBytes);

		sha.processBytes(file.get(), fileSizeInBytes);

		sha.getDigest(hash_buffer);
	}

	ifs.close();
}

void GenerateHash(const unsigned char * data, unsigned long long dataSize, uint32_t * hash_buffer)
{
	SHA1 sha;

	sha.processBytes(data, dataSize);

	sha.getDigest(hash_buffer);
}
