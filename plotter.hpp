#pragma once

#include <vector>

class Graph {
public:
  void add_graph(std::vector<double> graph);
  void plot() const;

private:
  std::vector<std::vector<double>> graphs{};
};
