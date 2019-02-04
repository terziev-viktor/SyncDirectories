#include "SyncCommandExecutor.h"
using namespace cmds;

int main(int argc, char * argv[])
{
	SyncCommandExecutor executor;
	// Test1 ---------------
	int test_argc = 7;
	const char * test_argv[7];
	char a[10];
	char b[10];
	char c[15];
	char d[15];
	char e[15];
	char f[40];
	char g[40];
	strcpy_s(a, 10, "sync.exe");
	strcpy_s(b, 10, "perform");
	strcpy_s(c, 15, "sync.txt");
	strcpy_s(d, 15, "-hash-only");
	strcpy_s(e, 15, "-block");
	strcpy_s(f, 40, "C:\\Users\\terzi\\Desktop\\folder1");
	strcpy_s(g, 40, "C:\\Users\\terzi\\Desktop\\folder2");
	test_argv[0] = a;
	test_argv[1] = b;
	test_argv[2] = c;
	test_argv[3] = d;
	test_argv[4] = e;
	test_argv[5] = f;
	test_argv[6] = g;
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