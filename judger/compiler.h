#ifndef COMPILER_H
#define COMPILER_H
#include <string>
using namespace std;

namespace ai
{
	class compiler
	{
	private:
		int lang;
		string path;
	public:
		int work();
		compiler(string, int);
	};
}
#endif
