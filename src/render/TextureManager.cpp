#include "render/TextureManager.hpp"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h>
#include <Magnum/GL/Sampler.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/ImageView.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <MagnumPlugins/StbImageImporter/StbImageImporter.h>

using namespace Magnum;
using namespace Magnum::Trade;
using namespace Corrade::Utility;

namespace mc::render
{

TextureManager::TextureManager(std::string const& texturesDir)
{
    m_importers.loadAndInstantiate("StbImageImporter");

    constexpr Path::ListFlags flags =
        Path::ListFlag::SkipDotAndDotDot |
        Path::ListFlag::SkipDirectories;

    auto listOptional = Path::list(texturesDir, flags);
    if (!listOptional) return;

    for (auto& filenameView : *listOptional)
    {
        auto filename = std::string{filenameView};
        if (!String::endsWith(filename, ".png")) continue;

        std::string name = filename.substr(0, filename.size() - 4);
        std::string fullPath = Path::join(texturesDir, filename);

        auto importer = m_importers.instantiate("StbImageImporter");
        CORRADE_INTERNAL_ASSERT(importer->openFile(fullPath));
        auto image = importer->image2D(0);
        CORRADE_INTERNAL_ASSERT(image);

        GL::Texture2D texture;
        texture.setStorage(1, GL::TextureFormat::RGBA8, image->size())
            .setSubImage(0, {}, *image)
            .setMinificationFilter(GL::SamplerFilter::Nearest)
            .setMagnificationFilter(GL::SamplerFilter::Nearest)
            .setWrapping(GL::SamplerWrapping::ClampToEdge);

        m_textures.emplace(std::move(name), std::move(texture));
    }
}

GL::Texture2D& TextureManager::get(std::string_view name)
{
    return m_textures.at(std::string{name});
}

} // namespace mc::render
