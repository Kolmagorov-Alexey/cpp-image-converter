#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>
#include <iostream>

using namespace std;

namespace img_lib {

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

PACKED_STRUCT_BEGIN BitmapFileHeader {
  
    uint16_t bfType = 0x4D42; // {'B', 'M'};
    uint32_t bfSize = 0;
    uint16_t bfReserved1 = 0;
	uint16_t bfReserved2 = 0;
    uint32_t bfOffBits = 0 ; 
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
  
    uint32_t biSize = 0; 
    int32_t biWidth = 0;
    int32_t biHeight = 0;
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = 0 ;
    int32_t biXPelsPerMeter = 11811;
    int32_t biYPelsPerMeter = 11811;
    int32_t biClrUsed = 0;
    int32_t biClrImportant = 0x1000000;
}
PACKED_STRUCT_END

bool SaveBMP(const Path& file, const Image& image) {

	int stride = GetBMPStride(image.GetWidth());
	int width = image.GetWidth();
    int height = image.GetHeight();
    
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
	file_header.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    file_header.bfSize = sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader)+stride*height;
	info_header.biSize= sizeof(BitmapInfoHeader);
	info_header.biSizeImage = stride * height;
    info_header.biWidth = image.GetWidth();
    info_header.biHeight = image.GetHeight();
    
    ofstream out(file, ios::binary);
    
    if (!out.is_open()) { cout << "File open problem"; }
    out.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));
    
    vector<char> buff(stride);
    
    for (int y = image.GetHeight() - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < image.GetWidth(); ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(reinterpret_cast<const char*>(buff.data()), stride);
    }
    return out.good();
}

Image LoadBMP(const Path& file) {

  BitmapFileHeader file_header;
  BitmapInfoHeader info_header;
  ifstream ifs(file, ios::binary);
 
  ifs.read(reinterpret_cast<char*>(&file_header), sizeof(BitmapFileHeader));
  ifs.read(reinterpret_cast<char*>(&info_header), sizeof(BitmapInfoHeader));
    
   int width = info_header.biWidth;
   int height = info_header.biHeight;
   int stride = GetBMPStride(width);
 
  Image image(width, height, Color::Black());
  vector<char> buff(stride); 

     for (int y = height - 1; y >= 0; --y) {
       Color* line = image.GetLine(y);
       ifs.read(buff.data(), stride);
       
       for (int x = 0; x < width; ++x) {
           line[x].b = static_cast<byte>(buff[x * 3 + 0]);
           line[x].g = static_cast<byte>(buff[x * 3 + 1]);
           line[x].r = static_cast<byte>(buff[x * 3 + 2]);
       }
   }
  return image;
}

}  // namespace img_lib
