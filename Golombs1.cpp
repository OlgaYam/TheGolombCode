// Golombs1.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <ctime>

using namespace std;

const int size = 100000; //����� �������
vector<bool> Code; //��� �������������� �����
vector<bool> BinaryCode; //�������� ������ �������
map<char, vector<bool> > table; //��������������� �������
map<int, int> distribute; //�������������

//�������� ��������������� �������������:
//���������, ���� ��������� ����� > p, �� ����������� �������
static int geometr(double p)
{
	int n = 1;
	double q = (double)rand() / RAND_MAX;
	while (q>p)
	{
		n++;
		q = (double)rand() / RAND_MAX;
	}
	return n;
}

//�������� ������ �������� ���� ����
static void Gray()
{
	int k = BinaryCode.size()-1;
	while (BinaryCode[k] != 0) k--;//������� ������ ���� ������
	BinaryCode[k] = 1; //������ ��� �� 1
	for (int m = k+1; m < BinaryCode.size(); m++)
		BinaryCode[m] = 0; //��� ��������� �� 1 ������� ��������
}

//���������� �������
static void BuildTable(int m)
{
	int c = int(ceil(log(m)/log(2))); //��������������� ��������
	//������ ������� � ������� ������ �������� �� �-1 �����
	for (int k = 0; k < c - 1; ++k)
		BinaryCode.push_back(0);
	//���������� ��� � ������� � �������� ������� ��������� ��������
	table[0] = BinaryCode; 
	int i = 1;
	// �� b-��� �������� ���� ����� ����� �-1, � ����� - � 
	int b = int(pow(2, double(c)) - m); //
	while (i < m)
	{
		Gray();
		//��� ����� � ������ ��������� ��� b+1.0
		//�.�. ��������� �� 1 � �������� � �������� ���������� �������� 0
		if (i == b) BinaryCode.push_back(0); 
		table[i]=BinaryCode;   
		++i;
	}
}

//�������� �������� ����
static void CreateUnaryCode(int q)
{
	for (int i = 0; i < q; ++i) Code.push_back(1);
	Code.push_back(0);
}


//-------------------------�����������--------------------------------
static void Coder(int m, double p)
{
	ofstream fileSource("SourceCode.txt", ios::out | ios::binary);
	ofstream fileBinary("Binary.txt", ios::out | ios::binary);
	int countBytes = 0; //
	int count=0; char buf=0;
	srand(time(NULL)); 
	for (int i = 0; i < size; ++i)
    {
		int n = geometr(p); //��������� ����� n �� ��������������� �������������
		distribute[n]++; //����������� ������� ��������� � n
		fileSource << n; //������ � ���� SourceCode
		int q = (int)n / m; //����� �����
		int r = n - q * m; //�������
		Code.clear();
		CreateUnaryCode(q);
		BinaryCode.clear(); 
		BinaryCode = table[r];
		Code.insert(Code.end(), BinaryCode.begin(), BinaryCode.end());
		//���������� ��� ��������
		for(int n=0; n < Code.size(); n++)
		{
			buf = buf | Code[n]<<(7-count); //����� ������ ������
			count++;
			//���� ����������� ����, �� ���������� ��� � ����
			if (count == 8) { 
				count=0;   
				fileBinary<<buf; 
				buf=0;
				countBytes++;
			}
		}
    }
	fileSource.close();
	fileBinary.close();
	//������ � ���� �������������
	ofstream fileDistr("Distribution.dat", ios::out | ios::binary);
	for (int i = 1; i < distribute.size(); ++i )
		fileDistr << distribute[i] << endl;
    fileDistr.close();


	//�������� �������������
	double entropy1 = 0;
	for (int k = 1; k < distribute.size(); ++k )
	{
		if (distribute[k] != 0) {
			double pk = double(distribute[k])/double(size);
			entropy1 += pk*(log(pk)/log(2));
		}
	}
	entropy1 *= -1;
	cout << "Teoretical entropy = " << entropy1 << endl;

	//�������� ������������
	double entropy2 = double(countBytes*8)/double(size);
    cout << "Practical entropy = " << entropy2 << endl;
}


//������� ���������� ����� � ����������
static int BinaryToDecimal(vector<bool> str)
{
	int res = 0;
	int k = str.size()-1;
	for (int i = 0; i < str.size(); ++i)
	{
		res += int(str[i])*pow(2, k);
		k--;
	}
	return res;
}

//------------------------�������������-------------------------------
static void Decoder(int m)
{
	int c = int(ceil(log(m)/log(2))); //��������������� ��������
	ifstream fileIn("Binary.txt", ios::in | ios::binary);
	ofstream fileOut("DecodeCode.txt", ios::out | ios::binary);
	int count = 0; int k = c-1;
	int len_u = 0; //������ ������������������ u;
	int a = 0; // ����� ������ � ������ u
	bool unary_flag = true; //����, ������������, ��� ������ �������������� ������� ������ �������
	bool binary_flag = false; //����, ������������, ��� ������ �������������� �������� ������ �������
	vector<bool> str; // �������
	int n = 0; //��������������� �����
	char byte = fileIn.get(); //������� ������ ����
	while (!fileIn.eof())
	{
		bool b = byte & (1<<(7-count));  //������������� ��������
		//���� ����� ������ 0, ������ ������� ������ �����������, ���������� a
		//��������� ���� ��� ������ �������� ����� �����
		if (binary_flag && b)
		{
			n = m * a;
			fileOut << n;
			unary_flag = true;
			binary_flag = false;
			k = c - 1;
			len_u = 0;
		}

		if (unary_flag && !b) {
			a = len_u; 
			binary_flag = true;
			unary_flag = false;
		}else {
			//����� ��������� �������
			if (binary_flag) {
				str.push_back(b); //���������� ���������� ������������������
				if (len_u == a+c-1) // ��������� �-1 ���
				{
					int r = BinaryToDecimal(str);
					if (r < pow(2,c) - m) 
						n = m * a + r;
					else 
					{
						count++;
						if (count==8) 
						{
							count=0; 
							byte = fileIn.get();
						}
						b = byte & (1<<(7-count));
						str.push_back(b);
						r = BinaryToDecimal(str);
						n = m * a + r - (pow(2,c) - m);
					}
					fileOut << n;
					unary_flag = true;
					binary_flag = false;
					k = c - 1;
					len_u = -1;
					str.clear();
				}
			}
		}   
		count++;
		len_u++;
		//���������, �� ���� �� ����� ��������� ����
		if (count==8) 
		{
			count=0; 
			byte = fileIn.get();
		}
	}
	fileIn.close();
	fileOut.close();
}



int main(int argc, char* argv[])
{
	double p;
	cout << "Enter p" << endl; //������ p
	cin >> p;
	int m = 1 - log(1+p)/log(p); // ������� ������������ m
	cout << "m = " << m << endl;

	BuildTable(m); //���������� ������� ��� ���������
	Coder(m, p); //�����������
	Decoder(m); //�������������
	cout << "Decoding is complete" << endl;
	system("pause");
	return 0;
}

