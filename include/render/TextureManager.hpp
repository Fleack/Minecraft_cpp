#pragma once

#include "render/BlockTextureMapper.hpp"

#include <map>
#include <string>
#include <string_view>

#include <Corrade/PluginManager/Manager.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Trade/AbstractImporter.h>

namespace mc::render
{

class TextureManager
{
public:
    explicit TextureManager(std::string const& texturesDir);
    Magnum::GL::Texture2D& get(texture_id id);

private:
    Magnum::PluginManager::Manager<Magnum::Trade::AbstractImporter> m_importers;
    std::map<texture_id, Magnum::GL::Texture2D> m_textures;
};

} // namespace mc::render
