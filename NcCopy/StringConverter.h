#pragma once
#include <string>
using namespace std;
class StringConverter
{
public:
	
	static char * strToChar(string str)
	{
		return (char*)str.c_str();
	};

};

