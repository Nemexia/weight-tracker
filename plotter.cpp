#include <iostream>
#include <fstream>
#include <cstdint>

#pragma pack(push, 1)  // Exact byte alignment
struct BMPFileHeader {
    uint16_t bfType = 0x4D42;     // 'BM'
    uint32_t bfSize;              // Size of file in bytes
    uint16_t bfReserved1 = 0;
    uint16_t bfReserved2 = 0;
    uint32_t bfOffBits = 54;      // Pixel data offset
};
struct BMPInfoHeader {
    uint32_t biSize = 40;         // Header size
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24;     // 24-bit bitmap
    uint32_t biCompression = 0;
    uint32_t biSizeImage;         // Image size
    int32_t biXPelsPerMeter = 2835;
    int32_t biYPelsPerMeter = 2835;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;
};
#pragma pack(pop)

constexpr int width{800};
constexpr int height{600};
using Pixel = std::array<unsigned char, 3>;
using Graph = std::array<std::array<Pixel, width>, height>;

void plot(Graph graph)
{
    // Each row padded to multiple of 4
    constexpr int rowSize{(width * 3 + 3) & ~3};
    constexpr int dataSize{rowSize * height};

    BMPFileHeader fileHeader;
    fileHeader.bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + dataSize;

    BMPInfoHeader infoHeader;
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biSizeImage = dataSize;

    std::ofstream out("graph.bmp", std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open file\n";
        return;
    }

    out.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    out.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    // Pixel data (bottom-up)
    for (auto const &row: graph) {
        for (auto const &pixel: row) {
            out.write(reinterpret_cast<const char*>(pixel.data()), 3);
        }
        // Padding
        int padding = rowSize - width * 3;
        unsigned char pad[3] = { 0, 0, 0 };
        out.write(reinterpret_cast<char*>(pad), padding);
    }

    out.close();
    std::cout << "BMP image created as raw_bmp_image.bmp\n";
}

int main() {
    Graph gg{};
    plot(gg);
    return 0;
}
