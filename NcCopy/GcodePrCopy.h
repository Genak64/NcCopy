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

	//��������� ��� ������ ���������
	struct progData {
		int size;//���������� ���������� ���������� ������
		char *arg[10]; //��������
		bool argFlag[10];
		char *filename; //��� �����
		bool notfile;
		char *newfilename; //������ ��� ����� ��� ������
		int cx; //���������� ����� �� ��� �
		bool notcx;
		int cy; //���������� ����� �� ��� Y
		bool notcy;
		float xmax;
		float ymax;
		std::string oldname;
		std::string newname;
		//������������� xmax � ymax (������������� ������� ��������)
		void initXYmax() {
			xmax = 0;
			ymax = 0;
		};
	};
	
private:
	//��������� ��� ����� GCODE
	struct CadrData {
		int cadrType;//��� ������� �5=1,�4=2,G0=3,G1=4,XY=5,X=6,Y=7,S=8,F=9,M3=10
		bool flagX; //������� ������ �� X
		bool flagY; //������� ������ �� Y
		bool flagS; //������� ������ �� S
		bool flagF; //������� ������ �� F
		float dataX; //�������� X
		float dataY; //�������� Y
		float dataS; //�������� S
		float dataF; //�������� F
	};

	/*---------------------------------------------
	������� ��� ��������������� ������ ����� ������
	-----------------------------------------------*/
	//�������� ������� �� �������������� � ��������� ���� 0-9
	//���������� true ��� ������������ � false ���� �� �������������.
	bool isDigit(char p)
	{
		switch (p)
		{
		case '0': // ���� p = 0
		case '1': // ���� p = 1
		case '2': // ���� p = 2
		case '3': // ���� p = 3
		case '4': // ���� p = 4
		case '5': // ���� p = 5
		case '6': // ���� p = 6
		case '7': // ���� p = 7
		case '8': // ���� p = 8
		case '9': // ���� p = 9
			return true; // ���������� true
		default: // � ��������� ������, ���������� false
			return false;
		}
	}

	//������������ ������� � ��������������� ������� ��������
	//���������� ����� ����� int
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
		default: // � ��������� ������, ���������� false
			return false;
		}
	}
	//������������ ������ � ��������������� ������� ��������
	//������ ������ ��������� ������ ������� 0-9 � "."
	//���������� ������������ ����� float
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
	//������������ ����� � ������
	//���������� ������ string
	string Dec2Str(float digit) {
		string s;
		char str[20];
		sprintf(str, "%.2f", digit);
		s.append(str);
		return s;
	}
	//������������ ������ ����� � ������
	//���������� ������ string
	string Int2Str(int digit) {
		string s;
		char str[20];
		sprintf(str, "%d", digit);
		s.append(str);
		return s;
	}
	//���� �������� ������ � ������ � ������ �������� ������� � ���� "." ����� ���� �� �������
	//����������� ������� ��� �� ����� ������
	//���������� ������ ���������� ������ ������� 0-9 � "."
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
	������� ��� ��������������� ����� G-code � ������ ��������� � ������� � ���� G-code
	-----------------------------------------------*/
	//������������ ������ ����� GCODE � ������ �������� ��������� CadrData
	//��������� ������ string
	//���������� ����������� ����� ������ ��������� CadrData
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

	//������������ � ������ ����� GCODE ������ �������� ��������� CadrData
	//��������� ����������� ����� ������ ��������� CadrData 
	//���������� ������ string
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
		default: // � ��������� ������, ���������� false
			return "";
		}
		return cadr;
	}

	/*---------------------------------------------
	������� ��� ������ � ��������� ��������� � ������� G-code
	-----------------------------------------------*/
	//��������� ����������� ����� ������ ��������� CadrData 
	//���������� ����������� ����� ������ ��������� CadrData (�����)
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

	//��������� ����������� ����� ������ ��������� CadrData 
	//���������� ����������� ����� ������ ��������� CadrData � ����������� �� �
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
	//��������� ����������� ����� ������ ��������� CadrData 
	//���������� ����������� ����� ������ ��������� CadrData � ����������� �� Y
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
	//��������� ������ ����� � ����������
	//���������� ������ ����������� �������� ������ ��������� CadrData 
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
	//��������� ������ ����������� �������� ������ ��������� CadrData 
	//���������� ������ ����� � ����������
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
	������� ��� ������ � ������� (����/�����)
	-----------------------------------------------*/
	//������ ���� ��������� ��� ������� ���������� �����
	//���������� ����� ����� ��� -1 ��� ������
	int countStrOfFile(char *pathfile)
	{
		int i;
		string s;
		int counts; // ���������� ����� � �����
					//��������� ���� �� ������	
		ifstream file;
		file.open(pathfile);
		if (!file)
		{
			//cout << "������! ���� �� ������\n\n"; 
			return -1;
		}
		//������ ���� ��������� ��� ������� ���������� ����� 	
		for (counts = 0; !file.eof(); counts++) {
			getline(file, s);
			// 	cout << s <<"  "<<counts+1<<endl;
		}
		file.close();
		return counts;
	}


	//������ ���� ��������� ��� ������ � ������
	//���������� ��������� ������
	string* readfile2ArrayStr(char *pathfile, int counts)
	{
		int i;
		string s;
		//char *pathfile ��� �����
		//int counts ���������� ����� � �����
		string *AS;
		AS = new string[counts];
		//��������� ���� �� ������	
		ifstream file;
		file.open(pathfile);
		if (!file)
		{
			//cout << "������! ���� �� ������\n\n"; 
		}
		//������ ���� ��������� ��� ������� ���������� ����� 	
		for (counts = 0; !file.eof(); counts++) {
			getline(file, AS[counts]);
		}
		file.close();
		return AS;
	}
	//���������� ���� ��������� �� �������
	//���������� ������ -1 ���� ���� �� ������
	int writeArrayStr2file(bool newfile, char *pathfile, int counts, string *AS)
	{
		int i;
		//��������� ���� �� ������	
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
			cout << "������! ���� �� ������\n\n";
			return -1;
		}
		//���������� ���� ��������� 	
		for (i = 0; i<counts; i++) {
			fileo << AS[i] << endl;
		}
		fileo.close();
		return 0;
	}

	//���������� ������ (��� �����) ���������� ������ ������� �� "."
	string filename2name(string str) {
		int tmp;
		string tmpStrDigit;
		tmp = str.find_first_of('.');
		str.resize(tmp);
		tmpStrDigit = str;

		return tmpStrDigit;
	}
	//���� �������� ������ � ������ � ������ �������� ������� � ���� "." ����� ���� �� �������
	//����������� ������� ��� �� ����� ������
	//���������� ������ ����� "." (���������� ����)
	string filename2type(string str) {
		int tmp;
		string tmpStrDigit;
		tmp = str.find_first_of('.');
		tmpStrDigit = str.substr(tmp + 1);
		return tmpStrDigit;
	}
	//��������� � ���������� ������ ������ ��� �����  � ����������� 
	//���������� ����� ��� ����� ��� ������
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
		������� ��� ������ � ����������� ���������� ������
		-----------------------------------------------*/
		//������ ��������� ������
		//���������� ����������� ��������� ������ ��� ���������
		progData parseComStr(int size, char *str[])
		{
			int i;
			progData tmp; //���������� ��������� ������ ���������	
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
				cout << "���������: " << str[0] << " {��� ����� ��� �����������} {���������� ����� �� ��� �} {���������� ����� �� ��� Y}" << endl;
				tmp.notfile = true;
				break;
			case 2:
				tmp.filename = tmp.arg[1];
				tmp.argFlag[1] = true;
				cout << "���������� ������� ���������� ����� �� ��� � �/��� Y" << endl;
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
				cout << "���������: " << str[0] << " {��� ����� ��� �����������} {���������� ����� �� ��� �} {���������� ����� �� ��� Y}" << endl;
			}
			//������� ����� ��� �����
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
		������� ��� ������ �� ������������ �������
		-----------------------------------------------*/
		//���������� (��������� progData) ������������ �������� ��������� (dimCadrOrigin) �� X,Y
		progData GetMaxXY(progData dataIn, CadrData *dimCadrOrigin, int counts) {
			int i;
			progData dataOut;
			dataOut = dataIn;
			dataOut.initXYmax();
			//������������ �������� ��������� �� X,Y
			for (i = 0; i<counts; i++) {
				if (dataOut.xmax<dimCadrOrigin[i].dataX) dataOut.xmax = dimCadrOrigin[i].dataX;
				if (dataOut.ymax<dimCadrOrigin[i].dataY) dataOut.ymax = dimCadrOrigin[i].dataY;
			}
			//cout<<"0-"<<"x max: "<<dataOut.xmax<<endl;  
			//cout<<"0-"<<"y max: "<<dataOut.ymax<<endl;  
			return dataOut;
		}
		/*---------------------------------------------
		������� ��� ������ �� ������������ ������� ��� ����������� ��������� (����)
		-----------------------------------------------*/
		//��������� ���������� �� X �/��� �, ���������� ��������� � ����
		/*���������: 	char - �������� ��� ����� (*pathfile)
		CadrData - ������ ������ ������������ ��������� (*dimCadrOrigin)
		int - ���������� �����/������ ��������� (counts)
		float - �������� �� X (deltaX)
		float - �������� �� Y (deltaY)
		int - ���������� ����� �� X (cx)
		int - ���������� ����� �� Y (cy)
		*/
		void CreateCopy2file(string namefile, CadrData *dimCadrOrigin, int counts, float deltaX, float deltaY) {
			int err;
			CadrData *dimCadrCopy;
			dimCadrCopy = new CadrData[counts];
			char *nfilename;
			string  *ArrayStrCopy;
			ArrayStrCopy = new string[counts];
			//���������� deltaX ������ ���������� � � ����� ���������
			dimCadrCopy = CadrArrDeltaX(dimCadrOrigin, counts, deltaX);
			//���������� deltaX ������ ���������� Y � ����� ���������
			dimCadrCopy = CadrArrDeltaY(dimCadrCopy, counts, deltaY);
			//��������� ���������� ������������� ����� ��������� �� ��������� ��� ������ � ����
			ArrayStrCopy = CadrArr2ArrSrt(dimCadrCopy, counts);
			//�������� ��� ����� ��� ������
			nfilename =StringConverter::strToChar(namefile);
			//���������� ���������� ������������� ����� ��������� �� ��������� � ����� ����
			err = writeArrayStr2file(0, nfilename, counts, ArrayStrCopy);
			return;
		}
		//������� ����� �� X �/��� �, ���������� ��������� � ����
		/*���������: 	progData - ��������� ��������� ������ ��������� (p)
		�������� ������ �� ���������� �����,
		����� ����� ����� � �.�.
		CadrData - ������ ������ ������������ ��������� (*dimCadrOrigin)
		int - ���������� �����/������ ��������� (counts)
		*/
		void CopyXY(progData p, CadrData *dimCadrOrigin, int counts) {
			int i, j, g;
			//float xmax=0, ymax=0;
			float deltaX = 0, deltaY = 0;
			//������������ �������� ��������� �� X,Y
			p = GetMaxXY(p, dimCadrOrigin, counts);
			//��������� ������ ��� �����
			for (j = 0; j<p.cx; j++) {
				deltaX = deltaX + p.xmax;//�������� �� � ��� ������ �����
				CreateCopy2file(p.newname, dimCadrOrigin, counts, deltaX, deltaY);
			}
			//��������� ��������� ���� �����
			if (p.cy>0) {
				for (i = 0; i<p.cy; i++) {
					deltaY = deltaY + p.ymax;//�������� �� y ��� ������ �����
					deltaX = 0;
					CreateCopy2file(p.newname, dimCadrOrigin, counts, deltaX, deltaY);
					//��������� ����� ������������ ��������� �� ��������� deltaX � ����������� ����� cx,cy
					for (j = 0; j<p.cx; j++) {
						deltaX = deltaX + p.xmax;//�������� �� � ��� ������ �����
						CreateCopy2file(p.newname, dimCadrOrigin, counts, deltaX, deltaY);
					}
				}
			}
			return;
		}

		//������� ����� �� X �/��� �, ���������� ��������� � ����
		/*���������: 	progData - ��������� ��������� ������ ��������� (p)
		�������� ������ �� ���������� �����,
		����� ����� ����� � �.�.
		*/
		void CopyProgram(progData p) {
			int err, i, j, g, cx = 0, cy = 0;
			char *nfilename;//��� �����
			int counts; // ���������� ��������� � ������� AS
			string *ArrayStr; // ��������� �� ��� string
			CadrData *dimCadrOrigin;
			//���������� ���������� ����� � �����
			// � ��������� ������������ �������� �����
			counts = countStrOfFile(p.filename);
			if (counts == -1) {
				cout << "������! ���� �� ������\n\n";
				return;
			}
			//  ������������ ��������� ������ ��� ���������� ������� 
			//  ��� ������ ��������� � ������
			ArrayStr = new string[counts];
			//  ������������ ��������� ������ ��� ���������������� ������� 
			//  ��� ��������������� ������� ��������� � ������  
			dimCadrOrigin = new CadrData[counts];
			//��������� ������������ ��������� � ���������������� ������ 
			ArrayStr = readfile2ArrayStr(p.filename, counts);
			//������������ ������ ����� � ������ ������
			dimCadrOrigin = ArrSrt2CadrArr(ArrayStr, counts);
			//������������ �������� ��������� �� X,Y
			p = GetMaxXY(p, dimCadrOrigin, counts);
			cout << "�������� ���������:" << endl;
			cout << "0-" << "x max: " << p.xmax << endl;
			cout << "0-" << "y max: " << p.ymax << endl;
			//��������� ����� ��� �����
			nfilename = StringConverter::strToChar(p.newname);
			cout << "��������� ������� � ����:" << nfilename << endl;
			//���������� ������������ ��������� � ����� ����
			err = writeArrayStr2file(1, nfilename, counts, ArrayStr);
			if (err == -1) {
				cout << "������ ������ � ����." << endl;
				return;
			}
			//������� ����� �� X �/��� Y, ��������� � ����
			CopyXY(p, dimCadrOrigin, counts);
			//������� ������
			delete[] ArrayStr;
			delete[] dimCadrOrigin;
			return;
		}




};

