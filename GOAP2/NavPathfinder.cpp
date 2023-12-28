#include "NavPathfinder.h"
#include <random>

NavPathfinder::NavPathfinder(const matrix& distanceMatrix,
    const std::map<std::string, std::vector<unsigned>>& pointNameToVertexId)
{
    _distanceMatrix = distanceMatrix;
    _numVertices = _distanceMatrix.size();
    _pointNameToVertexIds = pointNameToVertexId;
}

void NavPathfinder::GetNeighbors(std::vector<unsigned>& neighbors, const unsigned& vertex, const unsigned& finish) const
{
    for (unsigned i = 0; i < _numVertices; i++)
        if (_distanceMatrix[vertex][i] < MathHelper::INFTY)
            neighbors.push_back(i);
}

bool NavPathfinder::Satisfies(const unsigned& vertex, const unsigned& finish) const
{
    return vertex == finish;
}

unsigned NavPathfinder::GetId(const unsigned& vertex) const
{
    return vertex;
}

unsigned NavPathfinder::GetDistance(const unsigned& from, const unsigned& to) const
{
    return _distanceMatrix[from][to];
}

unsigned NavPathfinder::EmulateGetCurrentVertex() const
{
    return std::rand() % _numVertices;
}

unsigned NavPathfinder::EmulateGetDestinationVertexByName(const std::string& name) const
{
    const auto& it = _pointNameToVertexIds.find(name);
    if (it == _pointNameToVertexIds.end())
    {
        std::cout << "No point was found under the name " << name <<". Aborting.\n";
        exit(-1);
    }
    auto& vertices = it->second;
    unsigned randomIndex = std::rand() % vertices.size();
    return vertices[randomIndex];
}

