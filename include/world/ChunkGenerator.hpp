#pragma once

#include "world/Chunk.hpp"

#include <memory>

#include <FastNoiseLite.h>
#include <concurrencpp/concurrencpp.h>

namespace mc::world
{
/**
 * @brief Procedural terrain generator for voxel chunks.
 *
 * Uses FastNoiseLite to generate natural-looking terrain with height variation.
 * Designed to generate chunks column-by-column using 2D noise functions.
 */
class ChunkGenerator
{
public:
    /**
     * @brief Constructs a new ChunkGenerator with default noise settings.
     *
     * Initializes the noise generator with:
     * - OpenSimplex2 noise
     * - FBm fractal type
     * - 5 octaves, lacunarity of 2.0, gain of 0.5
     * - Low frequency for smooth large-scale features
     */
    ChunkGenerator();

    /**
     * @brief Fills a chunk with procedurally generated block data.
     *
     * Generates terrain height based on noise,
     * then fills block columns depending on the height.
     *
     * @param chunk A result which is available
     */
    concurrencpp::lazy_result<void> generate(Chunk& chunk, std::shared_ptr<concurrencpp::executor> executor) const;

private:
    FastNoiseLite m_noise{}; ///< Noise generator used for terrain shaping.
};
}
