#include "commands.hpp"
#include <stdexcept>
#include <functional>

void nikonov::printGraphs(std::ostream& out, const graphsMap& graphs)
{
  if (graphs.empty())
  {
    out << "\n";
    return;
  }
  for (auto it = graphs.begin(); it != graphs.end(); ++it)
  {
    out << it->first << "\n";
  }
}

void nikonov::vertexes(std::istream& in, std::ostream& out, const graphsMap& graphs)
{
  std::string name;
  in >> name;
  if (graphs.find(name) == graphs.end())
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  auto vertexes = graphs.at(name).getVertexes();
  if (vertexes.empty())
  {
    out << "\n";
    return;
  }
  for (auto it = vertexes.begin(); it != vertexes.end(); ++it)
  {
    out << *it << "\n";
  }
}

void nikonov::outBound(std::istream& in, std::ostream& out, const graphsMap& graphs)
{
  std::string name, vert;
  in >> name >> vert;
  if (graphs.find(name) == graphs.end())
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  Graph gr = graphs.at(name);
  if (!gr.hasVert(vert))
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  auto vertMap = gr.getOutBound(vert);
  if (vertMap.empty())
  {
    out << "\n";
    return;
  }
  for (auto iter = vertMap.begin(); iter != vertMap.end(); ++iter)
  {
    if (!iter->second.empty())
    {
      out << iter->first;
      for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
      {
        for (size_t i = 0; i < iter2->second; ++i)
        {
          out << " " << iter2->first;
        }
      }
      out << "\n";
    }
  }
}

void nikonov::inBound(std::istream& in, std::ostream& out, const graphsMap& graphs)
{
  std::string name, vert;
  in >> name >> vert;
  Graph gr = graphs.at(name);
  if (!gr.hasVert(vert))
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  auto vertMap = gr.getInBound(vert);
  if (vertMap.empty())
  {
    out << "\n";
    return;
  }
  for (auto iter = vertMap.begin(); iter != vertMap.end(); ++iter)
  {
    if (!iter->second.empty())
    {
      out << iter->first;
      for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
      {
        for (size_t i = 0; i < iter2->second; ++i)
        {
          out << " " << iter2->first;
        }
      }
      out << "\n";
    }
  }
}

void nikonov::bind(std::istream& in, graphsMap& graphs)
{
  std::string name, vert1, vert2;
  size_t weight = 0;
  if (!(in >> name >> vert1 >> vert2 >> weight))
  {
    throw std::invalid_argument("<INVALID ARGUMENT>");
  }
  graphs.at(name).addEdge(vert1, vert2, weight);
}

void nikonov::cut(std::istream& in, graphsMap& graphs)
{
  std::string name, vert1, vert2;
  size_t weight = 0;
  if (!(in >> name >> vert1 >> vert2 >> weight))
  {
    throw std::invalid_argument("<INVALID ARGUMENT>");
  }
  if (!graphs.at(name).removeEdge(vert1, vert2, weight))
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void nikonov::create(std::istream& in, graphsMap& graphs)
{
  std::string name;
  size_t count = 0;
  if (!(in >> name))
  {
    throw std::invalid_argument("<INVALID ARGUMENT>");
  }
  if (graphs.find(name) != graphs.end())
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  in >> count;
  Graph gr;
  for (size_t i = 0; i < count; ++i)
  {
    std::string vert;
    in >> vert;
    gr.addEdge(vert, vert, 0);
  }
  graphs[name] = gr;
}

void nikonov::merge(std::istream& in, graphsMap& graphs)
{
  std::string newGraph, firstGraph, secondGraph;
  if (!(in >> newGraph >> firstGraph >> secondGraph))
  {
    throw std::invalid_argument("<INVALID ARGUMENT>");
  }
  if (graphs.find(newGraph) != graphs.end())
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  Graph gr;
  gr.addEdges(graphs.at(firstGraph));
  gr.addEdges(graphs.at(secondGraph));
  graphs[newGraph] = gr;
}

void nikonov::extract(std::istream& in, graphsMap& graphs)
{
  std::string newGraph, firstGraph;
  size_t count = 0;
  if (!(in >> newGraph >> firstGraph >> count))
  {
    throw std::invalid_argument("<INVALID INPUT>");
  }
  if (graphs.find(newGraph) != graphs.end())
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
  Graph graph1 = graphs.at(firstGraph);
  Graph gr;
  std::set< std::string > vert;
  for (size_t i = 0; i < count; ++i)
  {
    std::string vert1;
    in >> vert1;
    if (!graph1.hasVert(vert1))
    {
      throw std::logic_error("<INVALID COMMAND>");
    }
    vert.insert(vert1);
  }
  auto edges = graph1.getEdges();
  for (auto iter = edges.begin(); iter != edges.end(); ++iter)
  {
    if (vert.find(iter->first.first) != vert.end() && vert.find(iter->first.second) != vert.end())
    {
      for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
      {
        for (size_t i = 0; i < iter2->second; ++i)
        {
          gr.addEdge(iter->first.first, iter->first.second, iter2->first);
        }
      }
    }
  }
  graphs[newGraph] = gr;
}

std::map< std::string, std::function< void() > > nikonov::createCommandsHandler(std::istream& in, std::ostream& out, graphsMap& graphs)
{
  std::map< std::string, std::function< void() > > commands;
  commands["graphs"] = std::bind(printGraphs, std::ref(out), std::cref(graphs));
  commands["vertexes"] = std::bind(vertexes, std::ref(in), std::ref(out), std::cref(graphs));
  commands["outbound"] = std::bind(outBound, std::ref(in), std::ref(out), std::cref(graphs));
  commands["inbound"] = std::bind(inBound, std::ref(in), std::ref(out), std::cref(graphs));
  commands["bind"] = std::bind(bind, std::ref(in), std::ref(graphs));
  commands["cut"] = std::bind(cut, std::ref(in), std::ref(graphs));
  commands["merge"] = std::bind(merge, std::ref(in), std::ref(graphs));
  commands["create"] = std::bind(create, std::ref(in), std::ref(graphs));
  commands["extract"] = std::bind(extract, std::ref(in), std::ref(graphs));
  return commands;
}
