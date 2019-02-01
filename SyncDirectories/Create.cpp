#include "Create.h"

#include <experimental/filesystem>
using std::experimental::filesystem::create_directory;

cmds::CREATE::CREATE()
	:Command("CREATE")
{
}

cmds::CommandResult cmds::CREATE::Execute(int argc, const char * argv[])
{
	std::string arg = argv[0];
	try
	{
		create_directory(arg);
	}
	catch (const std::exception& e)
	{
		return CommandResult() = 
		{
			false, e.what()
		};	
	}
	
	return CommandResult() = { true };
}

cmds::Command * cmds::CREATE::Create()
{
	return new CREATE();
}

