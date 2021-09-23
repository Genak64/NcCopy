// NcCopy.cpp: defines the entry point for the console application.


#include "stdafx.h"
#include "GcodePrCopy.h"

using namespace std;

int main(int size, char *str[])
{

	// size- number of arguments
	// char * str [] - string array with arguments

	setlocale(LC_ALL, "rus");//Russian language support in the program
	setlocale(LC_NUMERIC, "C");//Setting a point instead of a comma when displaying fractional numbers

	cout << "  NcCopy  " << endl;
	cout << "  Версия 1.03  " << endl;
	cout << "  10.01.2021  " << endl;
	// Create a variable for the program data type according to the progData structure
	GcodePrCopy::progData p;
	// Create an instance (object) of the GcodeProgCopy class to access its methods
	GcodePrCopy GcodeData;
	// Fill the variable fields by the progData structure according to the command line parameters
	p = GcodeData.parseComStr(size, str);
	// Check for the required parameters in the command line
	if (p.notfile == true) return 0;
	if (p.notcx == true && p.notcy == true) return 0;
	// Print input information to the console
	cout << "Исходный файл программы: " << p.filename << endl;
	cout << "Количество копий по оси Х: " << p.cx << endl;
	cout << "Количество копий по оси Y: " << p.cy << endl;
	// Make copies according to command line parameters
	GcodeData.CopyProgram(p);


    return 0;
}

