#pragma once
#ifndef AI_GLTFIMPORTER_H_INC
#define AI_GLTFIMPORTER_H_INC

#include <assimp/BaseImporter.h>
#include <assimp/DefaultIOSystem.h>

struct aiNode;

namespace glTF {
    class Asset;

}

namespace Assimp {

/**
 * Load the glTF format.
 * https://github.com/KhronosGroup/glTF/tree/master/specification
 */
class glTFImporter : public BaseImporter {
public:
    glTFImporter();
    ~glTFImporter() override;
    bool CanRead(const std::string &pFile, IOSystem *pIOHandler, bool checkSig) const override;

protected:
    const aiImporterDesc *GetInfo() const override;
    void InternReadFile(const std::string &pFile, aiScene *pScene, IOSystem *pIOHandler) override;

private:
    void ImportEmbeddedTextures(glTF::Asset &a);
    void ImportMaterials(glTF::Asset &a);
    void ImportMeshes(glTF::Asset &a);
    void ImportCameras(glTF::Asset &a);
    void ImportLights(glTF::Asset &a);
    void ImportNodes(glTF::Asset &a);
    void ImportCommonMetadata(glTF::Asset &a);

private:
    std::vector<unsigned int> meshOffsets;
    std::vector<int> embeddedTexIdxs;
    aiScene *mScene;
};

} // namespace Assimp

#endif // AI_GLTFIMPORTER_H_INC
