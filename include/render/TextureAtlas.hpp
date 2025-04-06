#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <stb_image.h>
#include <glad/gl.h>
#include <glm/glm.hpp>

namespace mc::render
{
class TextureAtlas
{
public:
    explicit TextureAtlas(std::size_t tileSize = 32);
    ~TextureAtlas();

    void loadFromDirectory(std::string const& path);
    void bind(uint32_t unit = 0) const;

    [[nodiscard]] glm::vec2 getUV(const std::string& name) const;

    [[nodiscard]] GLuint getID() const { return m_textureID; }
    [[nodiscard]] std::size_t getTileSize() const { return m_tileSize; }
    [[nodiscard]] std::size_t getAtlasSize() const { return m_atlasSize; }

private:
    std::vector<std::string> collectPngFiles(std::string const& path) const;
    void calculateAtlasSize(std::size_t textureCount);
    std::vector<uint8_t> generateAtlasData(std::vector<std::string> const& files);
    void copyTileToAtlas(const stbi_uc* data, int tileX, int tileY, std::vector<uint8_t>& atlasData, int atlasPixels, int channels) const;
    void uploadToGPU(const std::vector<uint8_t>& atlasData);

private:
    GLuint m_textureID = 0;
    std::size_t m_tileSize = 32;
    std::size_t m_atlasSize = 0;
    std::unordered_map<std::string, glm::vec2> m_uvMap;
};
}
