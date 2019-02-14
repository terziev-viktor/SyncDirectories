#include "SyncCommandExecutor.h"
using namespace cmds;

int main(int argc, char * argv[])
{
	SyncCommandExecutor executor;
	const char ** c_argv = const_cast<const char **>(argv);

	try
	{
		CommandResult result = executor.Execute(argv[1], argc - 2, c_argv + 2);
		std::cout << result.Message << std::endl;
		if (!result.Successful)
		{
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