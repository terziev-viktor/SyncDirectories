#include "CommandExecutor.h"
#include "libs.h"

using namespace cmds;
using namespace CryptoPP;

int main(int argc, char * argv[])
{
	CommandExecutor executor;
	int test_argc = 5;
	char * test_argv[5];
	char a[25];
	char b[25];
	char c[25];
	char d[25];
	char e[25];
	strcpy(a, "sync.exe");
	strcpy(b, "analyze");
	strcpy(c, "mirror");
	strcpy(d, "/home/viktor/test_dir_1");
	strcpy(e, "/home/viktor/test_dir_2");
	test_argv[0] = a;
	test_argv[1] = b;
	test_argv[2] = c;
	test_argv[3] = d;
	test_argv[4] = e;

	try
	{																																																																				
		CommandResult result = executor.Execute(test_argv[1], test_argc - 2, test_argv + 2);

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