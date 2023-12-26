#pragma once
#include "AStar.h"
#include "MathHelper.h"

class NavPathfinder : public AStarSolver<unsigned, unsigned>
{

public:

    std::unordered_map<std::pair<unsigned, unsigned>, Path<unsigned>, boost::hash<std::pair<unsigned, unsigned>>> calculatedPaths;
    
    NavPathfinder(const matrix& distanceMatrix, const  std::map<std::string, std::vector<unsigned>>& pointNameToVertexId);
    //Placeholder for engine function returning the navgraph vertex at which the agent is located currently. Returns random valid vertex id.
    unsigned EmulateGetCurrentVertex() const;
    //Placeholder for engine function returning the navgraph vertex where the closest point is located byt its name
    unsigned EmulateGetClosestObjectByName(const std::string& name) const;
private:
    void GetNeighbors(std::vector<unsigned>& neighbors, const unsigned& vertex, const unsigned& finish, void* userData) const override;
    bool Satisfies(const unsigned& vertex, const unsigned& finish, void* userData) const override;
    unsigned GetId(const unsigned& vertex) const override;
    unsigned GetDistance(const unsigned& from, const unsigned& to, void* userData) const override;



    unsigned _numVertices;
    matrix _distanceMatrix;
    //A map from point of interest name (cover, ammo box etc.) to an array of vertex ids corresponding to these points location in the level
    std::map<std::string, std::vector<unsigned>> _pointNameToVertexIds;
};

