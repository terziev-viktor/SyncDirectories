#include "CommandExecutor.h"
using namespace cmds;

int main(int argc, char * argv[])
{
	CommandExecutor executor;

	// Test 'vectors' ----
	int test_argc = 5;
	char * test_argv[5];
	char a[10];
	char b[10];
	char c[10];
	char d[50];
	char e[50];
	strcpy_s(a, 10, "sync.exe");
	strcpy_s(b, 10, "analyze");
	strcpy_s(c, 10, "mirror");
	strcpy_s(d, 50, "C:\\Users\\terzi\\Desktop\\Home");
	strcpy_s(e, 50, "C:\\Users\\terzi\\Desktop\\Home2");
	test_argv[0] = a;
	test_argv[1] = b;
	test_argv[2] = c;
	test_argv[3] = d;
	test_argv[4] = e;
	// --------------------

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