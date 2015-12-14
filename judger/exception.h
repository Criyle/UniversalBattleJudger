#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <string>
using namespace std;

namespace ai
{
	class myException
	{
	public:
		explicit myException() throw(){}
		virtual ~myException() throw(){}
		virtual string code() const throw() = 0;
		virtual string what() const throw() = 0;
	};

	class time_limit_exceeded_error : public myException
	{
	public:
		virtual string what() const throw()
		{
			return "Time Limit Exceeded";
		}
		virtual string code() const throw()
		{
			return "<font color=red>Time Limit Exceeded</font>";
		}
	};
	class real_time_limit_exceeded_error : public myException
	{
		string usedtime;
	public:
	real_time_limit_exceeded_error(string x=""):usedtime(x){	}
		virtual ~real_time_limit_exceeded_error() throw(){}
		virtual string what() const throw()
		{
			return "Time Limit Exceeded(real time exceeded 10s)" + usedtime;
		}
		virtual string code() const throw()
		{
			return "<font color=red>Time Limit Exceeded</font>";
		}
	};
	class memory_limit_exceeded_error : public myException
	{
	public:
		virtual string what() const throw()
		{
			return "Memory Limit Exceeded";
		}
		virtual string code() const throw()
		{
			return "<font color=red>Memory Limit Exceeded</font>";
		}
	};
	class output_limit_exceeded_error : public myException
	{
	public:
		virtual string what() const throw()
		{
			return "Output Limit Exceeded";
		}
		virtual string code() const throw()
		{
			return "<font color=red>Output Limit Exceeded</font>";
		}
	};
	class runtime_error_error : public myException
	{
		string info;
	public:
	runtime_error_error(string x=""):info(x){}
		virtual ~runtime_error_error() throw(){}
		virtual string what() const throw()
		{
			return "Runtime Error" + info;
		}
		virtual string code() const throw()
		{
			return "<font color=red>Runtime Error</font>";
		}
	};
	class manager_invalid : public myException
	{
		string info;
	public:
	manager_invalid(string x=""):info(x){}
		virtual ~manager_invalid() throw(){}
		virtual string what() const throw()
		{
			return "Manager Invalid" + info;
		}
		virtual string code() const throw()
		{
			return "<font color=red>Manager Invalid</font>";
		}
	};
	class input_error : public myException
	{
		string info;
	public:
	input_error(string x=""):info(x){}
		virtual ~input_error() throw(){}
		virtual string what() const throw()
		{
			return "Input Error" + info;
		}
		virtual string code() const throw()
		{
			return "<font color=red>Input Error</font>";
		}
	};
}
#endif
