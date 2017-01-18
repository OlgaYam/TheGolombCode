// Golombs1.cpp: определяет точку входа для консольного приложения.
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

const int size = 100000; //длина выборки
vector<bool> Code; //все закодированное число
vector<bool> BinaryCode; //бинарная запись остатка
map<char, vector<bool> > table; //вспомогательная таблица
map<int, int> distribute; //распределение

//создание геометрического распределения:
//проверяем, если случайное число > p, то увеличиваем счетчик
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

//создание нового элемента кода Грея
static void Gray()
{
	int k = BinaryCode.size()-1;
	while (BinaryCode[k] != 0) k--;//находим первый ноль справа
	BinaryCode[k] = 1; //меняем его на 1
	for (int m = k+1; m < BinaryCode.size(); m++)
		BinaryCode[m] = 0; //все следующие за 1 символы обнуляем
}

//построение таблицы
static void BuildTable(int m)
{
	int c = int(ceil(log(m)/log(2))); //вспомогательный параметр
	//первый элемент в таблице должен состоять из с-1 нулей
	for (int k = 0; k < c - 1; ++k)
		BinaryCode.push_back(0);
	//записываем его в таблицу и начинаем строить остальные элементы
	table[0] = BinaryCode; 
	int i = 1;
	// до b-ого элемента коды имеют длину с-1, а после - с 
	int b = int(pow(2, double(c)) - m); //
	while (i < m)
	{
		Gray();
		//Код длины с должен строиться как b+1.0
		//т.е. увеличили на 1 и добавили в качестве последнего элемента 0
		if (i == b) BinaryCode.push_back(0); 
		table[i]=BinaryCode;   
		++i;
	}
}

//создание унарного кода
static void CreateUnaryCode(int q)
{
	for (int i = 0; i < q; ++i) Code.push_back(1);
	Code.push_back(0);
}


//-------------------------Кодирование--------------------------------
static void Coder(int m, double p)
{
	ofstream fileSource("SourceCode.txt", ios::out | ios::binary);
	ofstream fileBinary("Binary.txt", ios::out | ios::binary);
	int countBytes = 0; //
	int count=0; char buf=0;
	srand(time(NULL)); 
	for (int i = 0; i < size; ++i)
    {
		int n = geometr(p); //формируем число n по геометрическому распределению
		distribute[n]++; //увеличиваем счетчик попадания в n
		fileSource << n; //запись в файл SourceCode
		int q = (int)n / m; //целая часть
		int r = n - q * m; //остаток
		Code.clear();
		CreateUnaryCode(q);
		BinaryCode.clear(); 
		BinaryCode = table[r];
		Code.insert(Code.end(), BinaryCode.begin(), BinaryCode.end());
		//записываем код побитово
		for(int n=0; n < Code.size(); n++)
		{
			buf = buf | Code[n]<<(7-count); //берем первый символ
			count++;
			//если образовался байт, то записываем его в файл
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
	//запись в файл распределения
	ofstream fileDistr("Distribution.dat", ios::out | ios::binary);
	for (int i = 1; i < distribute.size(); ++i )
		fileDistr << distribute[i] << endl;
    fileDistr.close();


	//Энтропия теоритическая
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

	//энтропия практическая
	double entropy2 = double(countBytes*8)/double(size);
    cout << "Practical entropy = " << entropy2 << endl;
}


//перевод двыоичного числа в десятичное
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

//------------------------Декодирование-------------------------------
static void Decoder(int m)
{
	int c = int(ceil(log(m)/log(2))); //вспомогательный параметр
	ifstream fileIn("Binary.txt", ios::in | ios::binary);
	ofstream fileOut("DecodeCode.txt", ios::out | ios::binary);
	int count = 0; int k = c-1;
	int len_u = 0; //длинна последовательности u;
	int a = 0; // число единиц в начале u
	bool unary_flag = true; //флаг, показывающий, что начали расшифровывать унарную запись остатка
	bool binary_flag = false; //флаг, показывающий, что начали расшифровывать двоичную запись остатка
	vector<bool> str; // остаток
	int n = 0; //раскодированное число
	char byte = fileIn.get(); //считали первый байт
	while (!fileIn.eof())
	{
		bool b = byte & (1<<(7-count));  //рассматриваем побитово
		//если нашли первый 0, значит унарная запись закончилась, запоминаем a
		//поднимаем флаг для поиска бинарной части числа
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
			//поиск бинарного остатка
			if (binary_flag) {
				str.push_back(b); //записываем получаемую последовательность
				if (len_u == a+c-1) // отсчитали с-1 бит
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
		//проверяем, не надо ли брать следующий байт
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
	cout << "Enter p" << endl; //запрос p
	cin >> p;
	int m = 1 - log(1+p)/log(p); // задание оптимального m
	cout << "m = " << m << endl;

	BuildTable(m); //построение таблицы для параметра
	Coder(m, p); //кодирование
	Decoder(m); //декодирование
	cout << "Decoding is complete" << endl;
	system("pause");
	return 0;
}

