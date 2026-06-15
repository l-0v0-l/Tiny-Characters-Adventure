#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include "OpenCVLoader.h"

class AssetManager {
private:
    std::map<std::string, sf::Texture> textures;
    AssetManager() = default;

public:
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    sf::Texture& getTexture(const std::string& filename) {
        auto it = textures.find(filename);
        if (it != textures.end()) {
            return it->second;
        }

        if (filename.empty()) {
            std::cout << "[AssetManager] Empty path detected. Diverting to fallback.\n";
            return getTexture("assets/heavy_bullet.png");
        }

        RawImageData rawData = cvLoadImageRaw(filename);

        if (rawData.success && !rawData.pixels.empty()) {
            sf::Texture tex;
            if (tex.resize({ rawData.width, rawData.height })) {
                tex.update(rawData.pixels.data());
                textures[filename] = std::move(tex);
                return textures[filename];
            }
        }

        std::cout << "[AssetManager] ERROR: OpenCV cannot read [" << filename << "]. Redirecting to safe backup texture!\n";

        if (filename != "assets/heavy_bullet.png") {
            textures[filename] = getTexture("assets/heavy_bullet.png");
            return textures[filename];
        }

        static sf::Texture absoluteStaticTex;
        if (absoluteStaticTex.getSize().x == 0) {
            sf::Image img(sf::Vector2u{ 32, 32 }, sf::Color::Magenta);
            if (absoluteStaticTex.resize(img.getSize())) {
                absoluteStaticTex.update(img.getPixelsPtr());
            }
        }
        return absoluteStaticTex;
    }
};