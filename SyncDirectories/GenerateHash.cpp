#include "GenerateHash.h"
#include "libs.h"
#include "Entity.h"
#include <vector>
#include <fstream>

using sha1::SHA1;

void GenerateHashForFile(const string & path_to_file, uint32_t * hash_buffer)
{
	std::ifstream ifs(path_to_file, std::ios::binary);
	SHA1 sha;
	{
		std::unique_ptr<char> buff(new char[Entity::BlockSize]);

		while (!ifs.eof())
		{
			ifs.read(buff.get(), Entity::BlockSize);
			sha.processBytes(buff.get(), ifs.gcount());
		}
		sha.getDigest(hash_buffer);
	}

	ifs.close();
}

void GenerateHash(const char * data, unsigned long long dataSize, uint32_t * hash_buffer)
{
	SHA1 sha;

	sha.processBytes(data, dataSize);

	sha.getDigest(hash_buffer);
}


void GenerateHashForFile(const string & path_to_file, std::vector<HashValue> & hash_buffer)
{
	std::ifstream ifs(path_to_file, std::ios::binary);
	uint32_t blockHash[5];

	{
		std::unique_ptr<char> buff(new char[Entity::BlockSize]);

		while (!ifs.eof())
		{
			ifs.read(buff.get(), Entity::BlockSize);
			GenerateHash(buff.get(), ifs.gcount(), blockHash);
			hash_buffer.push_back(blockHash);
		}
		
	}

	ifs.close();
}
