#pragma once
#include <vector>

class Plot {
public:
  Plot(std::size_t width, std::size_t height): m_width(width), m_height(height){};
  void draw() const;
  void set_data(std::vector<std::pair<double, double>> *data) { m_data = data; };

private:
  const std::size_t m_width{};
  const std::size_t m_height{};
  std::vector<std::pair<double, double>> *m_data{}; 
};