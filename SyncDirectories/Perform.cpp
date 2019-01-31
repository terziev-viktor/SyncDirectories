#include "Perform.h"
#include "PerformCommandExecutor.h"
#include <fstream>
#include <string>
using std::string;
using std::ifstream;

using namespace cmds;


Perform::Perform()
	:Command("perform")
{

}

Perform::~Perform()
{

}

const char * GetToken(const char * str, const char delimeter)
{
	while (str[0] != delimeter)
	{
		if (str[0] == '\0')
		{
			return nullptr;
		}
		++str;
	}

	return str;

}
CommandResult cmds::Perform::Execute(int argc, const char * argv[])
{
	if (argc < 1)
	{
		return CommandResult() =
		{
			false,
			"No path to sync.txt"
		};
	}
	PerformCommandExecutor exe;
	string current;
	string inp;
	std::vector<string> commandNames;
	std::vector<string> commandArgs;
	{ // reading sync.txt
		
		ifstream in;
		in.open(argv[0]);

		if (!in)
		{
			in.close();
			return CommandResult() =
			{
				false,
				"The file sync.txt could not be opened or found"
			};
		}
		while (!in.eof())
		{
			in >> inp;
			in.ignore();
			commandNames.push_back(inp);
			getline(in, inp);
			commandArgs.push_back(inp);
		}
		in.close();
	}

	std::unique_ptr<const char*> args(new const char*[commandArgs.size()]);

	for (size_t i = 0; i < commandArgs.size(); ++i)
	{
		args.get()[i] = commandArgs[i].c_str();
	}
	CommandResult r;
	for (size_t i = 0; i < commandNames.size(); ++i)
	{
		r = exe.Execute(commandNames[i], 1, args.get() + i);
		if (!r.Successful)
		{
			return r;
		}
	}

	return CommandResult() =
	{
		true,
		"All actions performed"
	};
}

Command * cmds::Perform::Create()
{
	return new Perform();
}
