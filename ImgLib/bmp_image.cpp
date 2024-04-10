#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {
    
// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

PACKED_STRUCT_BEGIN BitmapFileHeader {

    char sign_[2] = {'B', 'M'};
    uint32_t size_ = 0;
    uint32_t reserve_ = 0;
    uint32_t offset_ = 54;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t header_size = 40;
    int32_t width_ = 0;
    int32_t height_ = 0;
    uint16_t planes_ = 1;
    uint16_t bits_ = 24;
    uint32_t compression_type_ = 0;
    uint32_t size_;
    int32_t hor_resolution_ = 11811;
    int32_t ver_resolution_ = 11811;
    int32_t used_colors_ = 0;
    int32_t necessary_colors_ = 0x1000000;
}
PACKED_STRUCT_END



bool SaveBMP(const Path& file, const Image& image) {
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const int row_stride = GetBMPStride(image.GetWidth());

    BitmapFileHeader header;
    BitmapInfoHeader info;
    
    ofstream out(file, ios::binary);
    header.size_ = h * row_stride + header.offset_;
    info.width_ = w;
    info.height_ = h;
    info.size_ = h * row_stride;
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out.write(reinterpret_cast<const char*>(&info), sizeof(info));
    
    
    vector<char> buff(row_stride);
    
    for (int y = h - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < w; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(reinterpret_cast<const char*>(buff.data()), buff.size());
    }

    return out.good();

}

Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);
    BitmapFileHeader header;
    BitmapInfoHeader info;

    ifs.read(reinterpret_cast<char*>(&header), sizeof(BitmapFileHeader));
    ifs.read(reinterpret_cast<char*>(&info), sizeof(BitmapInfoHeader));

    int w = info.width_;
    int h = info.height_;
    int row_stride = GetBMPStride(w);

    std::vector<char> buff(row_stride);
    Image result(w, h, Color::Black());
    
    for(int y = h - 1; y >= 0; --y){
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), row_stride);
        for(int x = 0; x < w; ++x){
            line[x].r = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].b = static_cast<byte>(buff[x * 3 + 2]);
        }
    }
    return result;
}

}  // namespace img_lib
