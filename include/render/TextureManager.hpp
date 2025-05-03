#pragma once

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
    Magnum::GL::Texture2D& get(std::string_view name);

private:
    Magnum::PluginManager::Manager<Magnum::Trade::AbstractImporter> m_importers;
    std::map<std::string, Magnum::GL::Texture2D> m_textures;
};

} // namespace mc::render
