#include "render/TextureAtlas.hpp"

#include "core/Logger.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

#include <stb_image.h>

namespace fs = std::filesystem;

namespace mc::render
{
TextureAtlas::TextureAtlas(std::size_t tileSize) : m_tileSize(tileSize)
{
    LOG(INFO, "Initializing TextureAtlas with tile size: {}", tileSize);
}

TextureAtlas::~TextureAtlas()
{
    glDeleteTextures(1, &m_textureId);
    LOG(INFO, "Deleted TextureAtlas texture with ID: {}", m_textureId);
}

void TextureAtlas::loadFromDirectory(std::string const& path)
{
    LOG(INFO, "Loading textures from directory: {}", path);
    std::vector<std::string> files = collectPngFiles(path);
    LOG(INFO, "Found {} PNG files in directory", files.size());
    calculateAtlasSize(files.size());
    LOG(INFO, "Calculated atlas size: {}x{}", m_atlasSize, m_atlasSize);
    std::vector<uint8_t> atlasData = generateAtlasData(files);
    uploadToGpu(atlasData);
    LOG(INFO, "TextureAtlas uploaded to GPU with texture ID: {}", m_textureId);
}

void TextureAtlas::bind(uint32_t unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

glm::vec2 TextureAtlas::getUv(std::string const& name) const
{
    auto it = m_uvMap.find(name);
    if (it != m_uvMap.end()) return it->second;
    LOG(WARN, "UV for texture '{}' not found, returning default (0,0)", name);
    return {0.0f, 0.0f};
}

std::vector<std::string> TextureAtlas::collectPngFiles(std::string const& path) const
{
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(path))
    {
        if (entry.path().extension() == ".png")
        {
            files.push_back(entry.path().string());
        }
    }
    LOG(INFO, "Collected {} PNG files from directory: {}", files.size(), path);
    return files;
}

void TextureAtlas::calculateAtlasSize(std::size_t textureCount)
{
    m_atlasSize = static_cast<std::size_t>(std::ceil(std::sqrt(textureCount)));
    LOG(INFO, "Calculated atlas size: {} for {} textures", m_atlasSize, textureCount);
}

std::vector<uint8_t> TextureAtlas::generateAtlasData(std::vector<std::string> const& files)
{
    const uint64_t atlasPixels = m_atlasSize * m_tileSize;
    constexpr uint8_t channels = 4;
    std::vector<uint8_t> atlasData(atlasPixels * atlasPixels * channels, 0);

    int i = 0;
    for (const auto& file : files)
    {
        int width;
        int height;
        int ch;
        stbi_uc* data = stbi_load(file.c_str(), &width, &height, &ch, STBI_rgb_alpha);
        if (!data)
        {
            LOG(WARN, "Failed to load texture: {}", file);
            continue;
        }

        if (static_cast<std::size_t>(width) != m_tileSize || static_cast<std::size_t>(height) != m_tileSize)
        {
            LOG(WARN, "Texture size mismatch in file: {}. Expected: {}x{}, Actual: {}x{}", file, m_tileSize, m_tileSize, width, height);
            stbi_image_free(data);
            continue;
        }

        const int tileX = i % m_atlasSize;
        const int tileY = i / m_atlasSize;

        m_uvMap[fs::path(file).stem().string()] = {
            static_cast<float>(tileX) / m_atlasSize,
            static_cast<float>(tileY) / m_atlasSize,
        };
        copyTileToAtlas(data, tileX, tileY, atlasData, atlasPixels, channels);

        stbi_image_free(data);
        ++i;
    }

    return atlasData;
}

void TextureAtlas::copyTileToAtlas(const stbi_uc* data, int tileX, int tileY, std::vector<uint8_t>& atlasData, int atlasPixels, int channels) const
{
    for (std::size_t y = 0; y < m_tileSize; ++y)
    {
        for (std::size_t x = 0; x < m_tileSize; ++x)
        {
            int srcIdx = (y * m_tileSize + x) * channels;
            int dstIdx = ((tileY * m_tileSize + y) * atlasPixels + (tileX * m_tileSize + x)) * channels;
            std::memcpy(&atlasData[dstIdx], &data[srcIdx], channels);
        }
    }
}

void TextureAtlas::uploadToGpu(const std::vector<uint8_t>& atlasData)
{
    const uint64_t atlasPixels = m_atlasSize * m_tileSize;

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasPixels, atlasPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    LOG(INFO, "Uploaded TextureAtlas to GPU with ID: {}", m_textureId);
}
} // namespace mc::render
