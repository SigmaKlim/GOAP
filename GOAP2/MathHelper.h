#pragma once
#include <vector>
#include <fstream>

typedef unsigned int u_int;
typedef std::vector <std::vector<int>> mtrx;

const int INFTY = INT_MAX;

class MathHelper
{
public:
	static void MakeEmptyMatrix(mtrx& result, int dim);
	static void MakeRndMtrx(mtrx& result, int dim, int lower, int upper);
	static void MakeRndIncidenceMatrx(mtrx& result, int dim, float discChance = 0.0f);
	static int PrintMtrxToFile(const mtrx& toPrint, std::ofstream& fout, unsigned char separator = ' ');
	static int ReadMtrxFromFile(mtrx& result, std::ifstream& fin,/* int startingPos,*/ unsigned char separator = ' ');
	static void ToAdjacencyList(mtrx& incidence, mtrx& result);
	static unsigned NumDigits(int number);
	
};

template<class T> struct PtrLess
{
	bool operator()(T* lhs, T* rhs)
	{
		return *lhs < *rhs;
	}
};