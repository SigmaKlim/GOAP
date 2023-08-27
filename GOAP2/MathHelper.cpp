#include "MathHelper.h"
#include <random>
#include <string>
#include <iostream>
#include <cmath>
#pragma optimize( "", off )
void MathHelper::MakeEmptyMatrix(mtrx& result, int dim)
{
	result = mtrx(dim);
	for (size_t i = 0; i < dim; i++)
		result[i] = std::vector<int>(dim);
}
void MathHelper::MakeRndMtrx(mtrx& result, int dim, int lower, int upper)
{
	MakeEmptyMatrix(result, dim);
	for (size_t i = 0; i < dim; i++)
	{
		for (size_t j = 0; j < dim; j++)
			result[i][j] = lower + u_int(rand() % upper);
	}
}

void MathHelper::MakeRndIncidenceMatrx(mtrx& result, int dim, float discChance)
{
	MakeEmptyMatrix(result, dim);
	int discValue = discChance * 100.0f;
	for (size_t i = 0; i < dim; i++)
	{
		for (size_t j = 0; j < dim; j++)
			result[i][j] = (rand() % 101 <= discValue || i == j) ? INFTY : 1 + u_int(rand() % 100);
	}
}

int MathHelper::PrintMtrxToFile(const mtrx& toPrint,  std::ofstream& fout, unsigned char separator)
{
	if (fout.is_open() == false)
		return -1;
	for (const auto& line : toPrint)
	{
		for (const auto& cell : line)
			fout << cell << separator;
		fout << '\n';
	}
	fout << '\n';
	return 0;
}

int MathHelper::ReadMtrxFromFile(mtrx& result, std::ifstream& fin, /*int startingPos,*/ unsigned char separator)
{
	if (fin.is_open() == false)
		return -1;
	/*fin.seekg(startingPos, std::ios_base::beg);*/
	std::string str;
	std::string nextStr = "x";
	for (fin >> nextStr; fin.eof() != true; fin >> nextStr)
		str.append("\n" + nextStr);
	//std::cout << str;
	int commaPos = 0, nextCommaPos = 0;
	int ctr = 0;
	while (commaPos < str.length())
	{
		nextCommaPos = str.find(separator, commaPos);
		int line = ctr / result.size();
		int row = ctr - line * result.size();
		result[line][row] = stoi(str.substr(commaPos, nextCommaPos - commaPos));
		commaPos = nextCommaPos + 1;
		ctr++;
	}
	return 0;
}

void MathHelper::ToAdjacencyList(mtrx& incidence, mtrx& result)
{
	auto dim = incidence.size();
	result = mtrx(dim*dim, std::vector<int>(3)); //from, to, dist
	for (auto i = 0; i < dim; i++)
		for (auto j = 0; j < dim; j++)
			result[i * dim + j] = { i, j, incidence[i][j] };		
}

unsigned MathHelper::NumDigits(int number)
{
	unsigned digits = 1;
	unsigned bound = 10;
	number = abs(number);
	while (bound <= number)
	{
		digits++;
		bound *= 10;
	}
	return digits;
}

