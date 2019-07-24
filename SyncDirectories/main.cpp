#include "SyncCommandExecutor.h"

#ifdef _DEBUG
	#include "Inputs.h"
#endif // !_DEBUG

using namespace cmds;

int main(int argc, char * argv[])
{
	SyncCommandExecutor executor;

#pragma region IF_DEBUG_ENABLED

#ifndef _DEBUG
	const char** c_argv = const_cast<const char**>(argv);
#endif

#ifdef _DEBUG
	argc = 6;
#endif // _DEBUG

#pragma endregion

	try
	{
		CommandResult result = executor.Execute(c_argv[1], argc - 2, c_argv + 2);
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