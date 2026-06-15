#include "OpenCVLoader.h"
#include <opencv2/opencv.hpp> // ⭐ OpenCV 只准出現在這裡！
#include <iostream>

RawImageData cvLoadImageRaw(const std::string& filename) {
    RawImageData data;
    cv::Mat mat = cv::imread(filename, cv::IMREAD_UNCHANGED);

    if (mat.empty()) {
        std::cerr << "[OpenCV Engine] Failed to load image: " << filename << std::endl;
        return data;
    }

    // 修正 OpenCV 的色彩空間 BGR/BGRA 到 SFML 的 RGBA
    if (mat.channels() == 4) {
        cv::cvtColor(mat, mat, cv::COLOR_BGRA2RGBA);
    }
    else if (mat.channels() == 3) {
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGBA);
    }

    data.width = mat.cols;
    data.height = mat.rows;

    size_t dataSize = mat.total() * mat.elemSize();
    data.pixels.resize(dataSize);
    std::memcpy(data.pixels.data(), mat.data, dataSize);

    data.success = true;
    return data;
}