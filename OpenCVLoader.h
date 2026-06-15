#pragma once
#include <string>
#include <vector>

// 這個檔案是純粹的資料結構，絕對不 include SFML，防止編譯器混淆
struct RawImageData {
    std::vector<unsigned char> pixels;
    unsigned int width = 0;
    unsigned int height = 0;
    bool success = false;
};

// 讓 OpenCV 在背後默默讀圖的函數宣告
RawImageData cvLoadImageRaw(const std::string& filename);