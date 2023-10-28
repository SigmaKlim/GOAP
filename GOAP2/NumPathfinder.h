#pragma once
#include "Pathfind.hpp"
class NumPathfinder : public BasePathfinder<u_int>
{
	unsigned _dim;
	mtrx _matrix;

public:
	NumPathfinder(std::ifstream& fin, u_int dim_)
	{
		_dim = dim_;
		MathHelper::MakeEmptyMatrix(_matrix, _dim);
		MathHelper::ReadMtrxFromFile(_matrix, fin, ',');
	}
	void GetNeighbors(std::vector<u_int>& neighbors_, const u_int& vertex_, const u_int& finish_ = 0) const
	{
		for (int i = 0; i < _dim; i++)
			if (_matrix[vertex_][i] != INFTY)
			{
				neighbors_.push_back(i);
			}
	};
	unsigned GetDist(const u_int& from_, const u_int& to_) const
	{
		return _matrix[from_][to_];
	}
	t_node_id	GetId(const u_int& vertex_) const
	{
		return vertex_;
	}
	bool Satisfies(const u_int& vertex_, const u_int& finish_) const
	{
		return vertex_ == finish_;
	};
};