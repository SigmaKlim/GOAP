#pragma once
#include <random>
#include <ctime>
#include <string>
#include <iostream> 
#include <fstream>
#include "MathHelper.h"
#include "Pathfind.hpp"
#pragma optimize( "", off )

int test()
{
	srand(time(0));
	const std::vector<size_t> DIMS = { 500 };
	const int K = 1;
	const float DISC_CHANCE = 0.67f;
	//for (const auto& dim : DIMS)
	//{
	//	for (auto k = 0; k < K; k++)
	//	{
	//		std::ofstream fout("test_data/test_matrix_set_" + std::to_string(dim) + "_" + std::to_string(k) + ".txt");
	//		if (fout.is_open() == false)
	//			return -1;
	//		std::ofstream fout_("test_data/test_adjacency_list_" + std::to_string(dim) + "_" + std::to_string(k) + ".txt");
	//		if (fout.is_open() == false)
	//			return -1;
	//		mtrx m;
	//		mtrx aList;
	//		MathHelper::MakeRndIncidenceMatrx(m, dim, DISC_CHANCE);
	//		MathHelper::ToAdjacencyList(m, aList);
	//		MathHelper::PrintMtrxToFile(m, fout, ',');
	//		MathHelper::PrintMtrxToFile(aList, fout_, ' ');
	//		fout.close();
	//		fout_.close();
	//	}
	//}

	std::ofstream fout("test_data/results.txt");
	for (const auto& dim : DIMS)
	{
		if (fout.is_open() == false)
			return -1;
		for (auto k = 0; k < K; k++)
		{
			std::ifstream fin("test_data/test_matrix_set_" + std::to_string(dim) + "_" + std::to_string(k) + ".txt");
			if (fin.is_open() == false)
				return -1;
			TestPathfinder tpf(fin, dim);
			int start = rand() % dim;
			int finish = 0;
			do
			{
				finish = rand() % dim;
			} while (finish == start);
			fin.close();
			auto path = tpf.Pathfind(start, nullptr, finish);
			std::cout << "dim = " + std::to_string(dim) + "\t k = " + std::to_string(k) + "\n";
			std::cout << "start = " + std::to_string(start) + "\t finish = " + std::to_string(finish) + "\n";
			std::cout << "path: ";
			if (path == nullptr)
				std::cout << "NONE\ncost = INFTY\n\n";
			else
			{
				for (const auto& arc : path->GetArcList())
					std::cout << arc << " ";
				std::cout << "\n";
				std::cout << "cost = " << path->GetCost() << "\n\n";
			}

			fout << "dim = " + std::to_string(dim) + "\t k = " + std::to_string(k) + "\n";
			fout << "start = " + std::to_string(start) + "\t finish = " + std::to_string(finish) + "\n";
			fout << "path: ";
			if (path == nullptr)
				fout << "NONE\ncost = INFTY\n\n";
			else
			{
				for (const auto& arc : path->GetArcList())
					fout << arc << " ";
				fout << "\n";
				fout << "cost = " << path->GetCost() << "\n\n";
			}
			delete path;
		}
	}
}