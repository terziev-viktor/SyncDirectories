#include "Copy.h"
#include "DirectoryTree.h"
#include <fstream>
using std::ofstream;
using std::ifstream;
using dirtree::Entity;

cmds::Copy::Copy()
	:Command("COPY")
{

}

void cmds::Copy::LoadFiles()
{
	std::ifstream ifs;
	ifs.open("sync.bin", std::ios::binary);
	if (!ifs)
	{
		throw "Exception";
	}
	HashValue hshVal;
	size_t size;
	while (!ifs.eof())
	{
		ifs.read(reinterpret_cast<char*>(size), sizeof(size_t));
		std::unique_ptr<char> fullpath(new char[size]);
		ifs.read(fullpath.get(), size);
		ifs.read(reinterpret_cast<char*>(size), sizeof(size_t));
		std::string s(fullpath.get());

		for (size_t i = 0; i < size; ++i)
		{
			ifs.read(reinterpret_cast<char*>(&hshVal), sizeof(HashValue));
			files[s].push_back(hshVal);
		}
	}
}

cmds::CommandResult cmds::Copy::Execute(int argc, const char * argv[])
{
	if (this->files.empty())
	{
		this->LoadFiles();
	}

	std::string arg = argv[0]; // \some\path\to\file FROM \other\path\to\file
	size_t n = arg.find(' ');
	std::string left = arg.substr(0, n);
	n += 6; // skip from
	std::string right = arg.substr(n, arg.find_first_of(" #\n", n) - n);
	std::ifstream ifs;
	std::ofstream ofs;
	ifs.open(right);
	ofs.open(left);
	if (!ifs || !ofs)
	{
		throw std::exception("Could not open file");
	}
	char buffer[Entity::BlockSize];
	for (size_t i = 0; i < files[left].size() && i < files[right].size(); ++i)
	{
		if (files[left][i] != files[right][i])
		{
			ifs.seekg(0, i * Entity::BlockSize);
			ifs.read(buffer, Entity::BlockSize);
			ofs.seekp(0, i * Entity::BlockSize);
			ofs.write(buffer, ifs.gcount());
		}
	}
	for (size_t i = files[left].size(); i < files[right].size(); ++i)
	{
		ifs.seekg(0, i * Entity::BlockSize);
		ofs.seekp(0, i * Entity::BlockSize);
		ifs.read(buffer, Entity::BlockSize);
		ofs.write(buffer, ifs.gcount());
	}

	ifs.close();
	ofs.close();
	return CommandResult() =
	{
		false,
		"Not Implemented!"
	};
}

cmds::Command * cmds::Copy::Create()
{
	return new Copy();
}
