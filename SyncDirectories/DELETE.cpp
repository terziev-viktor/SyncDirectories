#include "DELETE.h"
#include <experimental/filesystem>
using std::experimental::filesystem::remove;
using std::experimental::filesystem::exists;

cmds::DELETE::DELETE()
	:Command("DELETE")
{

}

cmds::CommandResult cmds::DELETE::Execute(int argc, const char * argv[])
{
	std::string arg = argv[0];
	if (!exists(arg))
	{
		// Assuming it was already deleted by the user or by another process
		std::cout << arg << " was moved or already deleted." << std::endl;
		return CommandResult
		{
			true
		};
	}
	try
	{
		remove(arg);
	}
	catch (const std::exception& e)
	{
		return CommandResult { false, e.what() };
	}

	return CommandResult { true };
}

std::unique_ptr<cmds::Command> cmds::DELETE::Create()
{
	return std::make_unique<DELETE>();
}


