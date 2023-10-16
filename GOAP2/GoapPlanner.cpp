#include "GoapPlanner.h"

void GoapPlanner::GetNeighbors(std::vector<Vertex>& neighbors_, const Vertex& vertex_, const Vertex& finish_) const
{
	for (auto& actionName : vertex_.availableActions)
	{
		WorldState nextState; //change state by action
		auto& action = planner.GetAction(actionName);
		if (WorldState::IsActionUseful(nextState, vertex_.state, action)) //check if nextState is closer to finish_ than vertex_.state
		{
			auto neighborAvailableActions = vertex_.availableActions;
			neighborAvailableActions.erase(actionName);
			auto neighborPrevActions = vertex_.prevActions;
			neighborPrevActions.push_back(actionName);
			neighbors_.emplace_back(nextState, neighborAvailableActions, neighborPrevActions);
		}
	}
}

bool GoapPlanner::Satisfies(const Vertex& vertex_, const Vertex& initialVertex_) const
{
	const auto& initialState = initialVertex_.state;
	const auto& activeCndSet = vertex_.state;
	return (initialState.GetMask() & activeCndSet.GetMask()) == activeCndSet.GetMask();
}

t_node_id GoapPlanner::GetId(const Vertex& vertex_) const
{
	if (typeid(t_node_id) != typeid(t_mask))
	{
		std::cout << "t_node_id and t_mask have to same types! \n";
		exit(-1);
	}
	return t_node_id(vertex_.state.GetMask());
}

unsigned GoapPlanner::GetDist(const Vertex& from_, const Vertex& to_) const
{
	return planner.GetAction(to_.prevActions.back()).GetCost();
}