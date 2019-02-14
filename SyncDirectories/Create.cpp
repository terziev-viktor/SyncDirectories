#include "Create.h"

#include <experimental/filesystem>
using std::experimental::filesystem::create_directory;
using std::experimental::filesystem::exists;

cmds::CREATE::CREATE()
	:Command("CREATE")
{
}

cmds::CommandResult cmds::CREATE::Execute(int argc, const char * argv[])
{
	std::string arg = argv[0];
	if (exists(arg))
	{
		std::cout << arg << " already exists so it will not be created" << std::endl;
		return CommandResult { true };
	}
	try
	{
		create_directory(arg);
	}
	catch (const std::exception& e)
	{
		return CommandResult 
		{
			false, e.what()
		};	
	}
	
	return CommandResult { true };
}

cmds::Command * cmds::CREATE::Create()
{
	return new CREATE();
}

