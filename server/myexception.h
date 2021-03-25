#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H
#include "common.h"
  
class myException :public std::exception
{
public:
	myException(std::string c)
	{
		m_p = c;
	}
	virtual const char* what()
	{
		return m_p.c_str();
	}
private:
	std::string m_p;
};
#endif 
