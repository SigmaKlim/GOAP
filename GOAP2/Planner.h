#pragma once

#include "AStar.h"
#include "Tools/Catalogue.h"
#include "Actions/BAction.h"

struct Vertex
{
    ConditionSet ActiveConditionSet; //A set of conditions required for all previously taken actions
    std::size_t PrevActionId;
    //debug
    std::string PrevActionName; 
};

class Planner : public AStarSolver<Vertex>
{
    void GetNeighbors(std::vector<Vertex>& neighbors, const Vertex& vertex, const Vertex& finish) const override;
    bool Satisfies(const Vertex& vertex, const Vertex& finish) const override;
    float GetDistance(const Vertex& from, const Vertex& to) const override;
    float GetDistanceDenominator() const override;
    float GetHeuristic(const Vertex& vertex, const Vertex& finish) const override;
    float GetHeuristicsDenominator() const override;

    template <typename t_attribute>
    void RegisterAttribute(const std::string& name, EValueType valueType);

    Catalogue<BAttribute*> attributeCatalogue;
    Catalogue<BAction*> actionCatalogue;
};

template <typename t_attribute>
void Planner::RegisterAttribute(const std::string& name, EValueType valueType)
{
    BAttribute* bAttributePtr = dynamic_cast<BAttribute*>(new t_attribute(valueType));
    assert(bAttributePtr);
    attributeCatalogue.AddItem(name, bAttributePtr);
}
