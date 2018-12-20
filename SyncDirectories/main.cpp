#include "CommandExecutor.h"
using namespace cmds;

int main(int argc, char * argv[])
{
	CommandExecutor executor;
	
	try
	{
		CommandResult result = executor.Execute(argv[1], argc - 2, argv + 2);

		if (!result.Successful)
		{
			std::cout << result.Message << std::endl;
			return 1;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 2;
	}
	
	return 0;
}