/** @file  UnrealLoader.h
 *  @brief Declaration of the .3d (UNREAL) importer class.
 */
#ifndef INCLUDED_AI_3D_LOADER_H
#define INCLUDED_AI_3D_LOADER_H

#include <assimp/BaseImporter.h>

namespace Assimp {

// ---------------------------------------------------------------------------
/**
 *  @brief Importer class to load UNREAL files (*.3d)
 */
class UnrealImporter : public BaseImporter {
public:
    UnrealImporter();
    ~UnrealImporter();

    // -------------------------------------------------------------------
    /** @brief Returns whether we can handle the format of the given file
     *
     *  See BaseImporter::CanRead() for details.
     **/
    bool CanRead(const std::string &pFile, IOSystem *pIOHandler, bool checkSig) const override;

protected:
    // -------------------------------------------------------------------
    /** @brief Called by Importer::GetExtensionList()
     *
     * @see #BaseImporter::GetInfo for the details
     */
    const aiImporterDesc *GetInfo() const override;

    // -------------------------------------------------------------------
    /** @brief Setup properties for the importer
     *
     * See BaseImporter::SetupProperties() for details
     */
    void SetupProperties(const Importer *pImp) override;

    // -------------------------------------------------------------------
    /** @brief Imports the given file into the given scene structure.
     *
     * See BaseImporter::InternReadFile() for details
     */
    void InternReadFile(const std::string &pFile, aiScene *pScene,
            IOSystem *pIOHandler) override;

private:
    //! frame to be loaded
    uint32_t mConfigFrameID;

    //! process surface flags
    bool mConfigHandleFlags;

}; // !class UnrealImporter

} // end of namespace Assimp

#endif // AI_UNREALIMPORTER_H_INC
