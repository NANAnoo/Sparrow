#include "ResourceManager.h"

#include "IO/SOIL2/stb_image.h"
#include "IO/SOIL2/SOIL2.h"

namespace SPW
{
	void ResourceManager::LoadTextureScope(bool flip, const std::filesystem::path& filePath, const textureLoadCallback &callback)
	{
		stbi_set_flip_vertically_on_load(flip);
		int width, height, bpp;
		unsigned char *data = stbi_load(filePath.string().c_str(), &width, &height, &bpp, 0);
		callback(width, height, bpp, data);
		if (data) {
			stbi_image_free(data);
		}
	}
}
