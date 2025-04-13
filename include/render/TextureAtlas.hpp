#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <stb_image.h>
#include <glad/gl.h>
#include <glm/glm.hpp>

namespace mc::render
{
/**
 * @brief Manages a 2D texture atlas built from individual PNG tiles.
 *
 * Loads textures from a directory, packs them into a single OpenGL texture,
 * and provides UV mapping for each named tile.
 */
class TextureAtlas
{
public:
    /**
     * @brief Constructs the texture atlas with a fixed tile size.
     *
     * @param tileSize Size (in pixels) of each tile (must be square).
     */
    explicit TextureAtlas(std::size_t tileSize = 32);
    ~TextureAtlas();

    /**
     * @brief Loads all PNG files from a directory and creates the atlas.
     *
     * @param path Directory containing .png files (each file = one tile).
     */
    void loadFromDirectory(std::string const& path);

    /**
     * @brief Binds the atlas texture to a texture unit.
     *
     * @param unit Texture unit (e.g. 0 for GL_TEXTURE0).
     */
    void bind(uint32_t unit = 0) const;

    /**
     * @brief Returns the UV offset for a given tile name.
     *
     * @param name Tile name (file name without extension).
     *
     * @return Top-left UV coordinate of the tile (normalized [0, 1]).
     */
    [[nodiscard]] glm::vec2 getUv(std::string const& name) const;

    [[nodiscard]] GLuint getId() const { return m_textureId; }
    [[nodiscard]] std::size_t getTileSize() const { return m_tileSize; }
    [[nodiscard]] std::size_t getAtlasSize() const { return m_atlasSize; }

private:
    /**
     * @brief Collects all .png file paths from the given directory.
     *
     * @param path Target directory.
     *
     * @return List of full file paths.
     */
    std::vector<std::string> collectPngFiles(std::string const& path) const;

    /**
     * @brief Calculates the atlas side length (in tiles) as the smallest square.
     *
     * @param textureCount Number of tiles to pack.
     */
    void calculateAtlasSize(std::size_t textureCount);

    /**
     * @brief Generates pixel data for the combined atlas.
     *
     * @param files List of tile image paths.
     *
     * @return Raw RGBA pixel buffer representing the atlas.
     */
    std::vector<uint8_t> generateAtlasData(std::vector<std::string> const& files);

    /**
     * @brief Copies a tile into the correct atlas location.
     *
     * @param data Raw image data from stb_image (RGBA).
     * @param tileX Tile index (X-axis in atlas grid).
     * @param tileY Tile index (Y-axis in atlas grid).
     * @param atlasData Target atlas buffer.
     * @param atlasPixels Atlas width/height in pixels.
     * @param channels Number of color channels (must be 4).
     */
    void copyTileToAtlas(stbi_uc const* data, int tileX, int tileY, std::vector<uint8_t>& atlasData, int atlasPixels, int channels) const;

    /**
     * @brief Uploads atlas data to the GPU as an OpenGL texture.
     *
     * @param atlasData Pixel data for the entire atlas (RGBA).
     */
    void uploadToGpu(std::vector<uint8_t> const& atlasData);

private:
    GLuint m_textureId{0}; ///< OpenGL texture ID.
    std::size_t m_tileSize; ///< Size of each tile in pixels.
    std::size_t m_atlasSize{0}; ///< Number of tiles per row/column.
    std::unordered_map<std::string, glm::vec2> m_uvMap; ///< Map of tile names to UV offsets.
};
}
