#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <iostream>
#include <functional>
#include <map>
#include <set>
#include "Graph.hpp"

namespace nikonov
{
  using graphsMap = std::map< std::string, Graph >;

  void printGraphs(std::ostream& out, const graphsMap& graphs);
  void vertexes(std::istream& in, std::ostream& out, const graphsMap& graphs);
  void outBound(std::istream& in, std::ostream& out, const graphsMap& graphs);
  void inBound(std::istream& in, std::ostream& out, const graphsMap& graphs);
  void bind(std::istream& in, graphsMap& graphs);
  void cut(std::istream& in, graphsMap& graphs);
  void create(std::istream& in, graphsMap& graphs);
  void merge(std::istream& in, graphsMap& graphs);
  void extract(std::istream& in, graphsMap& graphs);

  std::map< std::string, std::function< void() > > createCommandsHandler(std::istream&, std::ostream&, graphsMap&);
}

#endif
