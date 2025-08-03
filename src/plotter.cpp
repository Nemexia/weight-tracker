#include "plotter.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

constexpr int width{800};
constexpr int height{600};
// Each row padded to multiple of 4
constexpr int row_size{(width * 3 + 3) & ~3};
constexpr int data_size{row_size * height};

#pragma pack(push, 1) // Exact byte alignment
struct BMPFileHeader {
  uint16_t bfType{0x4D42}; // 'BM'
  uint32_t bfSize;         // Size of file in bytes
  uint16_t bfReserved1{};
  uint16_t bfReserved2{};
  uint32_t bfOffBits{54}; // Pixel data offset
};
struct BMPInfoHeader {
  uint32_t biSize{40}; // Header size
  int32_t biWidth{width};
  int32_t biHeight{height};
  uint16_t biPlanes{1};
  uint16_t biBitCount{24}; // 24-bit bitmap
  uint32_t biCompression{0};
  uint32_t biSizeImage{data_size}; // Image size
  int32_t biXPelsPerMeter{2835};
  int32_t biYPelsPerMeter{2835};
  uint32_t biClrUsed{};
  uint32_t biClrImportant{0};
};
#pragma pack(pop)

using Pixel = std::array<unsigned char, 3>;
using Plot = std::array<std::array<Pixel, height>, width>;

static void write_bmp(const Plot &graph_to_plot) {

  std::ofstream out("graph.bmp", std::ios::binary);
  if (!out) {
    std::cerr << "Failed to open file\n";
    return;
  }

  {
    BMPFileHeader fileHeader{.bfSize = sizeof(BMPFileHeader) +
                                       sizeof(BMPInfoHeader) + data_size};
    BMPInfoHeader infoHeader;
    out.write(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    out.write(reinterpret_cast<char *>(&infoHeader), sizeof(infoHeader));
  }

  // Pixel data (bottom-up)
  for (int y{}; y < height; ++y) {
    for (int x{}; x < width; ++x) {
      out.write(reinterpret_cast<const char *>(graph_to_plot[x][y].data()), 3);
    }
    // Padding
    const int padding{row_size - width * 3};
    unsigned char pad[3] = {0, 0, 0};
    out.write(reinterpret_cast<char *>(pad), padding);
  }

  out.close();
  std::cout << "BMP image created as graph.bmp\n";
}

void Graph::add_graph(std::vector<double> graph) { graphs.push_back(graph); }

void Graph::plot() const {

  auto graph_to_plot = std::make_unique<Plot>();

  for (auto const &graph : graphs) {
    const double min{*std::min_element(graph.begin(), graph.end())};
    const double max{*std::max_element(graph.begin(), graph.end())};

    for (std::vector<double>::size_type i{0}; i < graph.size(); ++i) {
      const int x{static_cast<int>(
          (static_cast<double>(i) / (graph.size() - 1)) * (width - 1))};
      const int y{
          static_cast<int>((graph[i] - min) / (max - min) * (height - 1))};
      (*graph_to_plot)[x][y] = Pixel{0, 0, 240};
    }
  }

  write_bmp(*graph_to_plot);
}
