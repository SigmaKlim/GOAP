#pragma once
#include <vector>
#include <fstream>

typedef unsigned int u_int;
typedef std::vector <std::vector<u_int>> matrix;



class MathHelper
{
public:
	static const unsigned INFTY = UINT_MAX;
	static void MakeEmptyMatrix(matrix& result, u_int dim);
	static void MakeRndMtrx(matrix& result, u_int dim, u_int lower, u_int upper);
	static void MakeRndIncidenceMatrx(matrix& result, u_int dim, float discChance = 0.0f);
	static int PrintMtrxToFile(const matrix& toPrint, std::ofstream& fout, unsigned char separator = ' ');
	static int ReadMtrxFromFile(matrix& result, std::ifstream& fin,/* int startingPos,*/ unsigned char separator = ' ');
	static void ToAdjacencyList(matrix& incidence, matrix& result);
	static unsigned NumDigits(int number);
	static bool SatisfiesMask(const unsigned& mask_, const unsigned& value_);
	static bool SatisfiesMask(const unsigned long& mask_, const unsigned long& value_);
	static bool SatisfiesMask(const unsigned long long& mask_, const unsigned long long& value_);
	};

