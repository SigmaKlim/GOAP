#pragma once
#include "Plan.h"
#include "Pathfind.hpp"
struct Vertex
{
	Vertex					()  = default;
	Vertex					(const WorldState& state_, const std::set <std::string>& availableActions_, const std::vector<std::string>& prevActions_) :
								availableActions(availableActions_), state(state_), prevActions(prevActions_) {};

	std::set<std::string> availableActions;
	WorldState state;
	std::vector<std::string> prevActions;
};

struct GoapPlanner : public BasePathfinder<Vertex>
{
	GoapPlanner				(const Planner& planner_) : planner(planner_) {};
	~GoapPlanner			(){};
private:
	void		GetNeighbors(std::vector<Vertex>& neighbors_, const Vertex& vertex_, const Vertex& finish_ = Vertex()) const override;
	bool		Satisfies	(const Vertex& vertex_, const Vertex& finish_ = Vertex()) const override;
	t_node_id	GetId		(const Vertex& vertex_) const override;
	unsigned	GetDist		(const Vertex& from_, const Vertex& to_) const override;

	const Planner& planner;
};