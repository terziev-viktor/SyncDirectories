#include "Move.h"
#include <experimental/filesystem>
using std::experimental::filesystem::exists;
using std::experimental::filesystem::rename;

cmds::Move::Move()
	:Command("MOVE")
{

}

cmds::CommandResult cmds::Move::Execute(int argc, const char * argv[])
{
	std::string arg = argv[0];
	// \some\path\to\file TO \other\path\to\file  # bla bla
	std::string left, right;
	size_t n = arg.find(' ');
	left = arg.substr(0, n);
	n += 4; // skipping "TO "
	right = arg.substr(n, arg.find_first_of(" #\n", n) - n);
	if (!exists(left))
	{
		return CommandResult { false, left + " not found" };
	}
	try
	{
		rename(left, right);
	}
	catch (const std::exception& e)
	{
		return CommandResult
		{
			false, e.what()
		};
	}
	
	return CommandResult 
	{
		true, ""
	};
}

cmds::Command * cmds::Move::Create()
{
	return new Move();
}
