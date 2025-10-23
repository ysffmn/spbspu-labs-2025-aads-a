#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <functional>
#include <UBST/BinarySearchTree.hpp>
#include "KeySum.hpp"
int main(int argc, char* argv[])
{
  using namespace nikonov;

  if (argc != 3)
  {
    std::cerr << "Invalid arguments" << std::endl;
    return 1;
  }
  std::ifstream file(argv[2]);
  if (!file.is_open())
  {
    std::cerr << "File open failed" << std::endl;
    return 1;
  }
  BinarySearchTree< int, std::string > dict;
  int key;
  std::string value;
  while (true)
  {
    if (!(file >> key))
    {
      if (file.eof())
      {
        break;
      }
      else
      {
        std::cerr << "Error: overflow" << std::endl;
        return 1;
      }
    }
    if (!(file >> value))
    {
      std::cerr << "Error: overflow" << std::endl;
      return 1;
    }
    dict[key] = value;
  }

  if (dict.empty())
  {
    std::cout << "<EMPTY>" << std::endl;
    return 0;
  }
  std::string command = argv[1];
  KeySum func;
  std::map<std::string, std::function< KeySum(BinarySearchTree< int, std::string >&, KeySum) > > commandMap;
  commandMap["ascending"] = [](BinarySearchTree< int, std::string >& t, KeySum f)
  {
    return t.traverse_lnr(f);
  };
  commandMap["descending"] = [](BinarySearchTree< int, std::string >& t, KeySum f)
  {
    return t.traverse_rnl(f);
  };
  commandMap["breadth"] = [](BinarySearchTree< int, std::string >& t, KeySum f)
  {
    return t.traverse_breadth(f);
  };
  try
  {
    func = commandMap.at(command)(dict, func);
  }
  catch (const std::out_of_range&)
  {
    std::cerr << "Invalid command" << std::endl;
    return 1;
  }
  catch (const std::overflow_error& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  std::cout << func.res;
  if (!func.elems.empty())
  {
    std::cout << " " << func.elems;
  }
  std::cout << std::endl;

  return 0;
}
