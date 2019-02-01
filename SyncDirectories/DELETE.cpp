#include "DELETE.h"
#include <experimental/filesystem>
using std::experimental::filesystem::remove;

cmds::DELETE::DELETE()
	:Command("DELETE")
{
}

cmds::CommandResult cmds::DELETE::Execute(int argc, const char * argv[])
{
	std::string arg = argv[0];

	try
	{
		remove(arg);
	}
	catch (const std::exception& e)
	{
		return CommandResult() = { false, e.what() };
	}

	return CommandResult() = { true };
}

cmds::Command * cmds::DELETE::Create()
{
	return new DELETE();
}


