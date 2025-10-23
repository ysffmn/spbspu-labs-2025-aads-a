#include "Graph.hpp"
#include <stdexcept>

void nikonov::Graph::addVertex(const std::string& str)
{
  vertexes_.insert(str);
}

void nikonov::Graph::addEdge(std::string first, std::string second, size_t weigth)
{
  addVertex(first);
  addVertex(second);
  auto edge = std::make_pair(first, second);
  auto it = edges_.find(edge);
  
  if (it != edges_.end())
  {
    it->second[weigth]++;
  }
  else
  {
    std::map< size_t, size_t > weightMap;
    weightMap[weigth] = 1;
    edges_.insert(edge, weightMap);
  }
}

void nikonov::Graph::addEdges(const Graph& other)
{
  for (auto iter = other.edges_.begin(); iter != other.edges_.end(); ++iter)
  {
    for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
    {
      for (size_t i = 0; i < iter2->second; ++i)
      {
        addEdge(iter->first.first, iter->first.second, iter2->first);
      }
    }
  }
}

bool nikonov::Graph::removeEdge(const std::string& first, const std::string& second, size_t weigth)
{
  auto edge = std::make_pair(first, second);
  auto& weightMap = edges_[edge];
  auto pos = weightMap.find(weigth);
  if (pos == weightMap.end())
  {
    return false;
  }
  weightMap.erase(pos);
  if (weightMap.empty())
  {
    edges_.erase(edge);
  }
  return true;
}

std::set< std::string > nikonov::Graph::getVertexes() const
{
  return vertexes_;
}

typename nikonov::Graph::hashMapForEdges nikonov::Graph::getEdges() const
{
  return edges_;
}

bool nikonov::Graph::hasVert(const std::string& str) const
{
  return vertexes_.find(str) != vertexes_.end();
}

std::map< std::string, std::map< size_t, size_t > > nikonov::Graph::getOutBound(const std::string& str) const
{
  if (!hasVert(str))
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  std::map< std::string, std::map< size_t, size_t > > temp;
  for (auto iter = edges_.begin(); iter != edges_.end(); ++iter)
  {
    if (iter->first.first == str)
    {
      for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
      {
        temp[iter->first.second][iter2->first] = iter2->second;
      }
    }
  }
  return temp;
}

std::map< std::string, std::map< size_t, size_t > > nikonov::Graph::getInBound(const std::string& str) const
{
  if (!hasVert(str))
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  std::map< std::string, std::map< size_t, size_t > > temp;
  for (auto iter = edges_.begin(); iter != edges_.end(); ++iter)
  {
    if (iter->first.second == str)
    {
      for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
      {
        temp[iter->first.first][iter2->first] = iter2->second;
      }
    }
  }
  return temp;
}
