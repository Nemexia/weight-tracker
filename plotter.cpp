#include "plotter.h"
#include <iostream>

void Plot::draw() const {
    std::vector<std::vector<char>> canvas;
    canvas.resize(m_height, std::vector<char>(m_width, ' '));
    // Fill the canvas with data points
    for (const auto& point : m_data) {
        int x = static_cast<int>(point.first * m_width);
        int y = static_cast<int>(point.second * m_height);
        if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
            canvas[y][x] = '*'; // Mark the point on the canvas
        }
    }
    // Print the canvas to the console
    for (int y = m_height - 1; y >= 0; --y) {
        for (int x = 0; x < m_width; ++x) {
            std::cout << canvas[y][x];
        }
        std::cout << '\n';
    }
    // Print the x-axis
    for (int x = 0; x < m_width; ++x) {
        std::cout << '-';
    }
    std::cout << '\n';
}

