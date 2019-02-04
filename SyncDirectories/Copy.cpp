#include "Copy.h"
#include "Entity.h"
#include <fstream>
using std::experimental::filesystem::exists;
using std::ofstream;
using std::ifstream;

cmds::Copy::Copy()
	:Command("COPY")
{

}

cmds::CommandResult cmds::Copy::LoadFiles()
{
	std::ifstream ifs;
	ifs.open("sync.bin", std::ios::binary);
	if (!ifs)
	{
		ifs.close();
		return CommandResult() = { false, "Could not open sync.bin" };
	}
	HashValue hshVal;
	size_t size;
	while (!ifs.eof())
	{
		ifs.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		if (ifs.eof())
		{
			break;
		}
		std::unique_ptr<char> fullpath(new char[size]);
		ifs.read(fullpath.get(), size);
		ifs.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		std::string s(fullpath.get());

		for (size_t i = 0; i < size; ++i)
		{
			ifs.read(reinterpret_cast<char*>(&hshVal), sizeof(HashValue));
			files[s].push_back(hshVal);
		}
	}
	ifs.close();
	return CommandResult() = { true };
}

cmds::CommandResult cmds::Copy::Execute(int argc, const char * argv[])
{
	std::string arg = argv[0];
	if (Entity::Block && this->files.empty())
	{
		CommandResult r = this->LoadFiles();
		if (!r.Successful)
		{
			return r;
		}
	}

	// \some\path\to\file FROM \other\path\to\file
	size_t n = arg.find(' ');
	std::string outFile = arg.substr(0, n);
	n += 6; // skip from
	std::string readFromFile = arg.substr(n, arg.find_first_of(" #\n", n) - n);
	if (!exists(readFromFile))
	{
		return CommandResult() =
		{
			false,
			string("Directory ") + readFromFile + string(" not found")
		};
	}
	if (!exists(outFile))
	{
		std::ofstream(outFile).close();
	}
	std::ifstream ifs;
	std::fstream ofs;
	ifs.open(readFromFile, std::ios::binary);
	ofs.open(outFile, std::ios::binary | std::ios::in | std::ios::out);
	if (!ifs)
	{
		ifs.close();
		return CommandResult() = { false, string("Could not open ") + readFromFile + " file while performing a copy command" };
	}
	if (!ofs)
	{
		ofs.close();
		return CommandResult() = { false, string("Could not open ") + outFile + " file while performing a copy command" };
	}

	std::unique_ptr<char> buffer(new char[Entity::BlockSize]);
	if (Entity::Block)
	{
		for (size_t i = 0; i < files[outFile].size() && i < files[readFromFile].size(); ++i)
		{
			if (files[outFile][i] != files[readFromFile][i])
			{
				ifs.seekg(i * Entity::BlockSize, std::ios::beg);
				ifs.read(buffer.get(), Entity::BlockSize);
				ofs.seekp(i * Entity::BlockSize, std::ios::beg);
				ofs.write(buffer.get(), ifs.gcount());
				ofs.flush();
			}
		}
		ofs.seekp(0, std::ios::end);

		for (size_t i = files[outFile].size(); i < files[readFromFile].size(); ++i)
		{
			ifs.seekg(i * Entity::BlockSize, std::ios::beg);
			ifs.read(buffer.get(), Entity::BlockSize);
			ofs.write(buffer.get(), ifs.gcount());
			ofs.flush();
		}
	}
	else
	{
		while(!ifs.eof())
		{
			ifs.read(buffer.get(), Entity::BlockSize);
			if (ifs.eof())
			{
				break;
			}
			ofs.write(buffer.get(), ifs.gcount());
		}
	}
	ifs.close();
	ofs.close();
	return CommandResult() =
	{
		true
	};
}

cmds::Command * cmds::Copy::Create()
{
	return new Copy();
}
