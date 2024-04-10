#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

#include <memory>

using namespace std;

namespace interfaces{

enum class Format{
    JPEG,
    PPM,
    BMP,
    UNKNOWN
};

class ImageFormatInterface{
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class JPEGImageInterface : public ImageFormatInterface{
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
        return SaveJPEG(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override{
        return img_lib::LoadJPEG(file);
    }
};

class PPMImageInterface : public ImageFormatInterface{
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
        return SavePPM(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override{
        return img_lib::LoadPPM(file);
    }
};

class BMPImageInterface : public ImageFormatInterface{
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override{
        return SaveBMP(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override{
        return img_lib::LoadBMP(file);
    }
};

} // namespace interfaces

interfaces::Format GetFormatByExtension(const img_lib::Path& input_file){
    const std::string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return interfaces::Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return interfaces::Format::PPM;
    }

    if(ext == ".bmp"){
        return interfaces::Format::BMP; 
    }

    return interfaces::Format::UNKNOWN;
}

unique_ptr<interfaces::ImageFormatInterface> GetFormatInterface(const img_lib::Path& path){
    switch (GetFormatByExtension(path))
    {
    case interfaces::Format::JPEG:
        return make_unique<interfaces::JPEGImageInterface>();
        break;
    case interfaces::Format::PPM:
        return make_unique<interfaces::PPMImageInterface>();
        break;
    case interfaces::Format::BMP:
        return make_unique<interfaces::BMPImageInterface>();
        break;
    default:
        return nullptr;
        break;
    }
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    auto input_interface = GetFormatInterface(in_path);
    auto output_interface = GetFormatInterface(out_path);

    if(!input_interface){
        cerr << "Unknown format of the input file" << endl;
        return 2;
    }

    if(!output_interface){
        cerr << "Unknown format of the output file" << endl;
        return 3;
    }

    img_lib::Image image = input_interface->LoadImage(in_path);

    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }

    if (!output_interface->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    cout << "Successfully converted"sv << endl;
}