#include "Perform.h"
#include <string>
#include <fstream>
#include "Entity.h"
using std::ifstream;
using std::string;

cmds::Perform::Perform()
	:Command("perform")
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

	string current;
	string inp;
	string commandName;
	string commandArg;
	CommandResult r;
	
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
		const char * args[1];
		in >> commandName;
		if (commandName == "hash-only")
		{
			Entity::HashOnly = true;
		}
		in >> commandName;
		if (commandName == "block")
		{
			Entity::Block = true;
		}
		in.ignore();
		while (!in.eof())
		{
			in >> commandName;
			if (in.eof())
			{
				break;
			}
			in.ignore();
			getline(in, commandArg);
			args[0] = commandArg.data();
			try
			{
				r = this->executor.Execute(commandName, 1, args);
				if (!r.Successful)
				{
					in.close();
					return r;
				}
			}
			catch (const std::exception& e)
			{
				in.close();
				return CommandResult() = { false, e.what() };
			}
		}
		in.close();
	}

	return CommandResult() =
	{
		true,
		"All actions performed"
	};
}

cmds::Command * cmds::Perform::Create()
{
	return new Perform();
}
