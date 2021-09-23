#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include "StringConverter.h"

using namespace std;


class GcodePrCopy
{
public:

	virtual ~GcodePrCopy()
	{
	}

	//Структура для данных программы
	struct progData {
		int size;//Количество аргументов коммандной строки
		char *arg[10]; //аргумент
		bool argFlag[10];
		char *filename; //имя файла
		bool notfile;
		char *newfilename; //полное имя файла для вывода
		int cx; //Количество копий по оси Х
		bool notcx;
		int cy; //Количество копий по оси Y
		bool notcy;
		float xmax;
		float ymax;
		std::string oldname;
		std::string newname;
		//Инициализация xmax и ymax (присваивается нулевое значение)
		void initXYmax() {
			xmax = 0;
			ymax = 0;
		};
	};
	
private:
	//Структура для кадра GCODE
	struct CadrData {
		int cadrType;//тип команды М5=1,М4=2,G0=3,G1=4,XY=5,X=6,Y=7,S=8,F=9,M3=10
		bool flagX; //наличие данных по X
		bool flagY; //наличие данных по Y
		bool flagS; //наличие данных по S
		bool flagF; //наличие данных по F
		float dataX; //значение X
		float dataY; //значение Y
		float dataS; //значение S
		float dataF; //значение F
	};

	/*---------------------------------------------
	Функции для конвертирования разных типов данных
	-----------------------------------------------*/
	//проверка символа на принадлежность к числовому ряду 0-9
	//возвращает true при соответствии и false если не соответствует.
	bool isDigit(char p)
	{
		switch (p)
		{
		case '0': // если p = 0
		case '1': // если p = 1
		case '2': // если p = 2
		case '3': // если p = 3
		case '4': // если p = 4
		case '5': // если p = 5
		case '6': // если p = 6
		case '7': // если p = 7
		case '8': // если p = 8
		case '9': // если p = 9
			return true; // возвращаем true
		default: // в противном случае, возвращаем false
			return false;
		}
	}

	//пребразоание символа в соответствующее чиловое значение
	//возвращает целое число int
	int decChar2Dec(char sym) {
		switch (sym)
		{
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		default: // в противном случае, возвращаем false
			return false;
		}
	}
	//пребразоание строки в соответствующее чиловое значение
	//строка фолжна содержать только символы 0-9 и "."
	//возвращает вещественное число float
	float Str2Dec(string str) {
		int tmp, i, lenStr;
		bool pFlag = false;
		float tmpDigit = 0;
		lenStr = str.length();
		if (str.find_first_of(".") != string::npos) {
			tmp = str.find_first_of(".");	pFlag = true;
		}
		else pFlag = false;
		for (i = 0; i<lenStr; i++) {
			if (isDigit(str[i]) == true) tmpDigit = tmpDigit * 10 + decChar2Dec(str[i]);
		}
		if (pFlag == true) tmpDigit = tmpDigit / pow(10, (lenStr - tmp - 1));
		return tmpDigit;
	}
	//пребразоание числа в строку
	//возвращает строку string
	string Dec2Str(float digit) {
		string s;
		char str[20];
		sprintf(str, "%.2f", digit);
		s.append(str);
		return s;
	}
	//пребразоание целого числа в строку
	//возвращает строку string
	string Int2Str(int digit) {
		string s;
		char str[20];
		sprintf(str, "%d", digit);
		s.append(str);
		return s;
	}
	//ищет заданный символ в строке и читает цифровые символы и знак "." после него до первого
	//нецифрового символа или до конца строки
	//возвращает строку содержащую только символы 0-9 и "."
	string findDigits(char sym, string str) {
		int tmp, i;
		string tmpStrDigit, tmpStr;
		tmp = str.find_first_of(sym) + 1;
		tmpStr = str.substr(tmp);
		for (i = tmp; i<tmp + 20; i++) {
			if (isDigit(str[i]) != false || (str[i]) == '.') {
				tmpStrDigit = tmpStrDigit + str[i];
			}
			else break;
		}
		return tmpStrDigit;
	}




	/*---------------------------------------------
	Функции для конвертирования кадра G-code в данные программы и обратно в кадр G-code
	-----------------------------------------------*/
	//пребразоание строки кадра GCODE в данные согласно структуры CadrData
	//принимает строку string
	//возвращает заполненный набор данных структуры CadrData
	CadrData cadr2Data(string cadr)
	{
		CadrData tmpData;
		tmpData.flagX = false;
		tmpData.flagY = false;
		tmpData.flagS = false;
		tmpData.flagF = false;
		tmpData.dataX = 0;
		tmpData.dataY = 0;
		tmpData.dataS = 0;
		tmpData.dataF = 0;
		string tmpStr;
		if (cadr.find("M5") != string::npos) {
			tmpData.cadrType = 1;
			return tmpData;
		}
		if (cadr.find("M4") != string::npos) {
			tmpData.cadrType = 2;
			tmpData.dataS = Str2Dec(findDigits('S', cadr));
			if (tmpData.dataS != 0) tmpData.flagS = true;
			return tmpData;
		}
		if (cadr.find("M3") != string::npos) {
			tmpData.cadrType = 10;
			tmpData.dataS = Str2Dec(findDigits('S', cadr));
			if (tmpData.dataS != 0) tmpData.flagS = true;
			return tmpData;
		}
		if (cadr.find("G0") != string::npos) {
			tmpData.cadrType = 3;
			tmpData.dataX = Str2Dec(findDigits('X', cadr));
			tmpData.dataY = Str2Dec(findDigits('Y', cadr));
			if (tmpData.dataX != 0)tmpData.flagX = true;
			if (tmpData.dataY != 0)tmpData.flagY = true;
			return tmpData;
		}
		if (cadr.find("G1") != string::npos) {
			tmpData.cadrType = 4;
			tmpData.dataX = Str2Dec(findDigits('X', cadr));
			tmpData.dataY = Str2Dec(findDigits('Y', cadr));
			tmpData.dataF = Str2Dec(findDigits('F', cadr));
			if (tmpData.dataX != 0)tmpData.flagX = true;
			if (tmpData.dataY != 0)tmpData.flagY = true;
			if (tmpData.dataF != 0) tmpData.flagF = true;
			return tmpData;
		}
		if (cadr.find("X") != string::npos && cadr.find("Y") != string::npos) {
			tmpData.cadrType = 5;
			tmpData.dataX = Str2Dec(findDigits('X', cadr));
			tmpData.dataY = Str2Dec(findDigits('Y', cadr));
			if (tmpData.dataX != 0)tmpData.flagX = true;
			if (tmpData.dataY != 0)tmpData.flagY = true;
			return tmpData;
		}
		else {
			if (cadr.find("X") != string::npos) {
				tmpData.cadrType = 6;
				tmpData.dataX = Str2Dec(findDigits('X', cadr));
				if (tmpData.dataX != 0)tmpData.flagX = true;
				return tmpData;
			}
			if (cadr.find("Y") != string::npos) {
				tmpData.cadrType = 7;
				tmpData.dataY = Str2Dec(findDigits('Y', cadr));
				if (tmpData.dataY != 0)tmpData.flagY = true;
				return tmpData;
			}
		}
		if (cadr.find("S") != string::npos) {
			tmpData.cadrType = 8;
			tmpData.dataS = Str2Dec(findDigits('S', cadr));
			if (tmpData.dataS != 0) tmpData.flagS = true;
			return tmpData;
		}
		if (cadr.find("F") != string::npos) {
			tmpData.cadrType = 9;
			tmpData.dataF = Str2Dec(findDigits('F', cadr));
			if (tmpData.dataF != 0) tmpData.flagF = true;
			return tmpData;
		}
		return tmpData;
	}

	//пребразоание в строку кадра GCODE данных согласно структуры CadrData
	//принимает заполненный набор данных структуры CadrData 
	//возвращает строку string
	string Data2Cadr(CadrData tmpData)
	{
		string cadr = "";
		switch (tmpData.cadrType)
		{
		case 1:
			cadr = "M5";
			break;
		case 2:
			cadr = "M4";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 3:
			cadr = "G0";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 4:
			cadr = "G1";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 5:
			//cadr="XY";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 6:
			//cadr="X";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 7:
			//cadr="Y";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 8:
			//cadr="S";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 9:
			//cadr="F";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 10:
			cadr = "M3";
			if (tmpData.flagX == true) cadr = cadr + "X" + Dec2Str(tmpData.dataX);
			if (tmpData.flagY == true) cadr = cadr + "Y" + Dec2Str(tmpData.dataY);
			if (tmpData.flagS == true) cadr = cadr + "S" + Dec2Str(tmpData.dataS);
			if (tmpData.flagF == true) cadr = cadr + "F" + Dec2Str(tmpData.dataF);
			break;
		case 0:
			break;
		default: // в противном случае, возвращаем false
			return "";
		}
		return cadr;
	}

	/*---------------------------------------------
	Функции для работы с массивами строковых и кадрами G-code
	-----------------------------------------------*/
	//принимает заполненный набор данных структуры CadrData 
	//возвращает заполненный набор данных структуры CadrData (копию)
	CadrData* CopyCadrDataArray(CadrData *tmpIn, int counts)
	{
		int i;
		CadrData *tmpOut;
		tmpOut = new CadrData[counts];
		for (i = 0; i<counts; i++) {
			tmpOut[i] = tmpIn[i];
		}
		return tmpOut;
	}

	//принимает заполненный набор данных структуры CadrData 
	//возвращает заполненный набор данных структуры CadrData с приращением по Х
	CadrData* CadrArrDeltaX(CadrData *tmpIn, int counts, float deltax)
	{
		int i;
		CadrData *tmpOut;
		tmpOut = new CadrData[counts];
		for (i = 0; i<counts; i++) {
			tmpOut[i] = tmpIn[i];
			if (tmpOut[i].flagX == true) tmpOut[i].dataX = tmpOut[i].dataX + deltax + 0.1;
		}
		return tmpOut;
	}
	//принимает заполненный набор данных структуры CadrData 
	//возвращает заполненный набор данных структуры CadrData с приращением по Y
	CadrData* CadrArrDeltaY(CadrData *tmpIn, int counts, float deltay)
	{
		int i;
		CadrData *tmpOut;
		tmpOut = new CadrData[counts];
		for (i = 0; i<counts; i++) {
			tmpOut[i] = tmpIn[i];
			if (tmpOut[i].flagY == true) tmpOut[i].dataY = tmpOut[i].dataY + deltay + 0.1;
		}
		return tmpOut;
	}
	//принимает массив строк с программой
	//возвращает массив заполненный наборами данных структуры CadrData 
	CadrData* ArrSrt2CadrArr(string *tmpIn, int counts)
	{
		int i;
		CadrData *tmpOut;
		tmpOut = new CadrData[counts];
		for (i = 0; i<counts; i++) {
			tmpOut[i] = cadr2Data(tmpIn[i]);
		}
		return tmpOut;
	}
	//принимает массив заполненный наборами данных структуры CadrData 
	//возвращает массив строк с программой
	string* CadrArr2ArrSrt(CadrData *tmpIn, int counts)
	{
		int i;
		string *tmpOut;
		tmpOut = new string[counts];
		for (i = 0; i<counts; i++) {
			tmpOut[i] = Data2Cadr(tmpIn[i]);
		}
		return tmpOut;
	}


	/*---------------------------------------------
	Функции для работы с файлами (ввод/вывод)
	-----------------------------------------------*/
	//Читает файл построчно для посчета количества строк
	//возвращает целое число или -1 при ошибке
	int countStrOfFile(char *pathfile)
	{
		int i;
		string s;
		int counts; // количество строк в файле
					//Открываем файл на чтение	
		ifstream file;
		file.open(pathfile);
		if (!file)
		{
			//cout << "Ошибка! Файл не открыт\n\n"; 
			return -1;
		}
		//Читаем файл построчно для посчета количества строк 	
		for (counts = 0; !file.eof(); counts++) {
			getline(file, s);
			// 	cout << s <<"  "<<counts+1<<endl;
		}
		file.close();
		return counts;
	}


	//Читает файл построчно для записи в массив
	//возвращает строковый массив
	string* readfile2ArrayStr(char *pathfile, int counts)
	{
		int i;
		string s;
		//char *pathfile имя файла
		//int counts количество строк в файле
		string *AS;
		AS = new string[counts];
		//Открываем файл на чтение	
		ifstream file;
		file.open(pathfile);
		if (!file)
		{
			//cout << "Ошибка! Файл не открыт\n\n"; 
		}
		//Читаем файл построчно для посчета количества строк 	
		for (counts = 0; !file.eof(); counts++) {
			getline(file, AS[counts]);
		}
		file.close();
		return AS;
	}
	//Записывает файл построчно из массива
	//возвращает ошибку -1 если файл не создан
	int writeArrayStr2file(bool newfile, char *pathfile, int counts, string *AS)
	{
		int i;
		//Открываем файл на запись	
		ofstream fileo;
		if (newfile == true) {
			//	ofstream fileo(pathfile);
			fileo.open(pathfile, ios::out);
		}
		else {
			//	ofstream fileo(pathfile,ios_base::app);
			fileo.open(pathfile, ios::app);
		}
		if (!fileo)
		{
			cout << "Ошибка! Файл не создан\n\n";
			return -1;
		}
		//Записываем файл построчно 	
		for (i = 0; i<counts; i++) {
			fileo << AS[i] << endl;
		}
		fileo.close();
		return 0;
	}

	//возвращает строку (имя файла) содержащую только символы до "."
	string filename2name(string str) {
		int tmp;
		string tmpStrDigit;
		tmp = str.find_first_of('.');
		str.resize(tmp);
		tmpStrDigit = str;

		return tmpStrDigit;
	}
	//ищет заданный символ в строке и читает цифровые символы и знак "." после него до первого
	//нецифрового символа или до конца строки
	//возвращает строку после "." (расширение фала)
	string filename2type(string str) {
		int tmp;
		string tmpStrDigit;
		tmp = str.find_first_of('.');
		tmpStrDigit = str.substr(tmp + 1);
		return tmpStrDigit;
	}
	//формирует и возвращает строку полное имя файла  с расширением 
	//содержащую новое имя файла для записи
	string CreateFilename(string filename, int x, int y)
	{
		string name, type, tmpStr, pfile;
		name = filename2name(filename);
		type = filename2type(filename);
		pfile.append(name);
		pfile.append("-x");
		tmpStr = Int2Str(x);
		pfile.append(tmpStr);
		if (y != 1) {
			pfile.append("-y");
			tmpStr = Int2Str(y);
			pfile.append(tmpStr);
		}
		pfile.append(".");
		pfile.append(type);
		return pfile;
	}

	public:
		/*---------------------------------------------
		Функции для работы с параметрами коммандной строки
		-----------------------------------------------*/
		//Разбор командной строки
		//Возвращает заполненную структуру данных для программы
		progData parseComStr(int size, char *str[])
		{
			int i;
			progData tmp; //Переменная структуры данных программы	
			string fname, newfname;
			for (i = 0; i<size; i++)  tmp.arg[i] = str[i];
			tmp.size = size;
			tmp.cx = 0;
			tmp.cy = 0;
			tmp.notfile = false;
			tmp.notcx = false;
			tmp.notcy = false;
			switch (tmp.size)
			{
			case 1:
				cout << "Синтаксис: " << str[0] << " {имя файла для копирования} {количество копий по оси Х} {количество копий по оси Y}" << endl;
				tmp.notfile = true;
				break;
			case 2:
				tmp.filename = tmp.arg[1];
				tmp.argFlag[1] = true;
				cout << "Необходимо указать количество копий по оси Х и/или Y" << endl;
				tmp.notcx = true;
				tmp.notcy = true;
				break;
			case 3:
				tmp.filename = tmp.arg[1];
				tmp.argFlag[1] = true;
				tmp.cx = Str2Dec(tmp.arg[2]);
				tmp.argFlag[2] = true;
				tmp.notcy = true;
				break;
			case 4:
				tmp.filename = tmp.arg[1];
				tmp.argFlag[1] = true;
				tmp.cx = Str2Dec(tmp.arg[2]);
				tmp.argFlag[2] = true;
				tmp.cy = Str2Dec(tmp.arg[3]);
				tmp.argFlag[3] = true;
				break;
			default:
				cout << "Синтаксис: " << str[0] << " {имя файла для копирования} {количество копий по оси Х} {количество копий по оси Y}" << endl;
			}
			//Создаем новое имя файла
			if (tmp.size>1) {
				fname = string(tmp.filename);
				tmp.oldname = fname;
				newfname = CreateFilename(fname, tmp.cx + 1, tmp.cy + 1);
				tmp.newname = newfname;
				//tmp.newfilename=StringConverter::strToChar(newfname);

			}
			return tmp;
		}

		/*---------------------------------------------
		Функции для работы со специальными данными
		-----------------------------------------------*/
		//возвращает (структура progData) максимальные габариты оригинала (dimCadrOrigin) по X,Y
		progData GetMaxXY(progData dataIn, CadrData *dimCadrOrigin, int counts) {
			int i;
			progData dataOut;
			dataOut = dataIn;
			dataOut.initXYmax();
			//максимальные габариты оригинала по X,Y
			for (i = 0; i<counts; i++) {
				if (dataOut.xmax<dimCadrOrigin[i].dataX) dataOut.xmax = dimCadrOrigin[i].dataX;
				if (dataOut.ymax<dimCadrOrigin[i].dataY) dataOut.ymax = dimCadrOrigin[i].dataY;
			}
			//cout<<"0-"<<"x max: "<<dataOut.xmax<<endl;  
			//cout<<"0-"<<"y max: "<<dataOut.ymax<<endl;  
			return dataOut;
		}
		/*---------------------------------------------
		Функции для работы со специальными данными для копирования программы (ядро)
		-----------------------------------------------*/
		//Выполняет приращение по X и/или У, дописывает результат в файл
		/*Принимает: 	char - исходное имя файла (*pathfile)
		CadrData - массив данных оригинальной программы (*dimCadrOrigin)
		int - количество строк/кадров программы (counts)
		float - смещение по X (deltaX)
		float - смещение по Y (deltaY)
		int - количество копий по X (cx)
		int - количество копий по Y (cy)
		*/
		void CreateCopy2file(string namefile, CadrData *dimCadrOrigin, int counts, float deltaX, float deltaY) {
			int err;
			CadrData *dimCadrCopy;
			dimCadrCopy = new CadrData[counts];
			char *nfilename;
			string  *ArrayStrCopy;
			ArrayStrCopy = new string[counts];
			//прибавляем deltaX каждой координате Х в копии программы
			dimCadrCopy = CadrArrDeltaX(dimCadrOrigin, counts, deltaX);
			//прибавляем deltaX каждой координате Y в копии программы
			dimCadrCopy = CadrArrDeltaY(dimCadrCopy, counts, deltaY);
			//Формируем стороковое представление копии программы со смещением для записи в файл
			ArrayStrCopy = CadrArr2ArrSrt(dimCadrCopy, counts);
			//Получаем имя файла для записи
			nfilename =StringConverter::strToChar(namefile);
			//Дописываем стороковое представление копии программы со смещением в новый файл
			err = writeArrayStr2file(0, nfilename, counts, ArrayStrCopy);
			return;
		}
		//Создает копии по X и/или У, дописывает результат в файл
		/*Принимает: 	progData - заполнная структура данных программы (p)
		содержит данные по количеству копий,
		новом имени файла и т.д.
		CadrData - массив данных оригинальной программы (*dimCadrOrigin)
		int - количество строк/кадров программы (counts)
		*/
		void CopyXY(progData p, CadrData *dimCadrOrigin, int counts) {
			int i, j, g;
			//float xmax=0, ymax=0;
			float deltaX = 0, deltaY = 0;
			//максимальные габариты оригинала по X,Y
			p = GetMaxXY(p, dimCadrOrigin, counts);
			//Формируем первый ряд копий
			for (j = 0; j<p.cx; j++) {
				deltaX = deltaX + p.xmax;//Смещение по х для каждой копии
				CreateCopy2file(p.newname, dimCadrOrigin, counts, deltaX, deltaY);
			}
			//Формируем следующие ряды копий
			if (p.cy>0) {
				for (i = 0; i<p.cy; i++) {
					deltaY = deltaY + p.ymax;//Смещение по y для каждой копии
					deltaX = 0;
					CreateCopy2file(p.newname, dimCadrOrigin, counts, deltaX, deltaY);
					//формируем копии оригинальной программы со смещением deltaX с количеством копий cx,cy
					for (j = 0; j<p.cx; j++) {
						deltaX = deltaX + p.xmax;//Смещение по х для каждой копии
						CreateCopy2file(p.newname, dimCadrOrigin, counts, deltaX, deltaY);
					}
				}
			}
			return;
		}

		//Создает копии по X и/или У, записывает результат в файл
		/*Принимает: 	progData - заполнная структура данных программы (p)
		содержит данные по количеству копий,
		новом имени файла и т.д.
		*/
		void CopyProgram(progData p) {
			int err, i, j, g, cx = 0, cy = 0;
			char *nfilename;//имя файла
			int counts; // количество элементов в массиве AS
			string *ArrayStr; // указатель на тип string
			CadrData *dimCadrOrigin;
			//Определяем количество строк в файле
			// и проверяем корректность открытия файла
			counts = countStrOfFile(p.filename);
			if (counts == -1) {
				cout << "Ошибка! Файл не открыт\n\n";
				return;
			}
			//  Динамическое выделение памяти для строкового массива 
			//  Для записи программы в память
			ArrayStr = new string[counts];
			//  Динамическое выделение памяти для преобразованного массива 
			//  Для манипулирования данными программы в памяти  
			dimCadrOrigin = new CadrData[counts];
			//Загружаем оригинальную программу в структуированный массив 
			ArrayStr = readfile2ArrayStr(p.filename, counts);
			//Конвертируем массив строк в массив данных
			dimCadrOrigin = ArrSrt2CadrArr(ArrayStr, counts);
			//максимальные габариты оригинала по X,Y
			p = GetMaxXY(p, dimCadrOrigin, counts);
			cout << "Габариты оригинала:" << endl;
			cout << "0-" << "x max: " << p.xmax << endl;
			cout << "0-" << "y max: " << p.ymax << endl;
			//формируем новое имя файла
			nfilename = StringConverter::strToChar(p.newname);
			cout << "Результат записан в файл:" << nfilename << endl;
			//Записываем оригинальную программу в новый файл
			err = writeArrayStr2file(1, nfilename, counts, ArrayStr);
			if (err == -1) {
				cout << "Ошибка записи в файл." << endl;
				return;
			}
			//Создаем копии по X и/или Y, сохраняем в файл
			CopyXY(p, dimCadrOrigin, counts);
			//Удаляем массив
			delete[] ArrayStr;
			delete[] dimCadrOrigin;
			return;
		}




};

