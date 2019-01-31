#include "SyncCommandExecutor.h"
#include "SortedVector.h"
#include <ctime>
using namespace cmds;
#include <experimental/filesystem>
using namespace experimental::filesystem;


int main(int argc, char * argv[])
{
	SyncCommandExecutor executor;
	// Test1 ---------------
	int test_argc = 6;
	const char * test_argv[6];
	char a[10];
	char b[10];
	char b1[10];
	char c[10];
	char d[40];
	char e[40];
	strcpy_s(a, 10, "sync.exe");
	strcpy_s(b, 10, "analyze");
	strcpy_s(b1, 10, "mirror");
	strcpy_s(c, 10, "-block");
	strcpy_s(d, 40, "C:\\Users\\terzi\\Desktop\\Home");
	strcpy_s(e, 40, "C:\\Users\\terzi\\Desktop\\Home2");
	test_argv[0] = a;
	test_argv[1] = b;
	test_argv[2] = b1;
	test_argv[3] = c;
	test_argv[4] = d;
	test_argv[5] = e;
	// --------------------
	try
	{																										
		CommandResult result = executor.Execute(test_argv[1], test_argc - 2, test_argv + 2);
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