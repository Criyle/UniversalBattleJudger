#ifndef MANAGER_H
#define MANAGER_H
#include "runner.h"
#include <string>
using namespace std;

namespace ai
{
	class manager
	{
	private:
		managerRunner *man;
		aiRunner *ai[AI_MAX];
		int ai_num;
		int now;
		int run_flag;
		//int a_mem_lmt, a_time_lmt, a_step_lmt, m_mem_lmt, m_time_lmt, m_step_lmt;
		ai_info ai_lmt, man_lmt;
		
	public:
		manager(string, int, ai_info ai_lmt = (ai_info){0, 60 * 1000, 0, 256 * STD_MB, 0, 2000, 100, 0}, ai_info man_lmt = (ai_info){0, 60 * 1000, 0, 256 * STD_MB, 0, 10000, 100, 1000});
		~manager();
		void work();
		string result[AI_MAX], work_dir;
		int score[AI_MAX];
		ai_info info[AI_MAX];
	};
}

#endif
