/** @file Importer.h mostly internal stuff for use by #Assimp::Importer */
#pragma once
#ifndef INCLUDED_AI_IMPORTER_H
#define INCLUDED_AI_IMPORTER_H

#include <exception>
#include <map>
#include <vector>
#include <string>
#include <assimp/matrix4x4.h>

struct aiScene;

namespace Assimp    {
    class ProgressHandler;
    class IOSystem;
    class BaseImporter;
    class BaseProcess;
    class SharedPostProcessInfo;


//! @cond never
// ---------------------------------------------------------------------------
/** @brief Internal PIMPL implementation for Assimp::Importer
 *
 *  Using this idiom here allows us to drop the dependency from
 *  std::vector and std::map in the public headers. Furthermore we are dropping
 *  any STL interface problems caused by mismatching STL settings. All
 *  size calculation are now done by us, not the app heap. */
class ImporterPimpl {
public:
    // Data type to store the key hash
    typedef unsigned int KeyType;

    // typedefs for our configuration maps.
    typedef std::map<KeyType, int> IntPropertyMap;
    typedef std::map<KeyType, ai_real> FloatPropertyMap;
    typedef std::map<KeyType, std::string> StringPropertyMap;
    typedef std::map<KeyType, aiMatrix4x4> MatrixPropertyMap;
    typedef std::map<KeyType, void*> PointerPropertyMap;

    /** IO handler to use for all file accesses. */
    IOSystem* mIOHandler;
    bool mIsDefaultHandler;

    /** Progress handler for feedback. */
    ProgressHandler* mProgressHandler;
    bool mIsDefaultProgressHandler;

    /** Format-specific importer worker objects - one for each format we can read.*/
    std::vector< BaseImporter* > mImporter;

    /** Post processing steps we can apply at the imported data. */
    std::vector< BaseProcess* > mPostProcessingSteps;

    /** The imported data, if ReadFile() was successful, nullptr otherwise. */
    aiScene* mScene;

    /** The error description, if there was one. In the case of an exception,
     *  mException will carry the full details. */
    std::string mErrorString;

    /** Any exception which occurred */
    std::exception_ptr mException;

    /** List of integer properties */
    IntPropertyMap mIntProperties;

    /** List of floating-point properties */
    FloatPropertyMap mFloatProperties;

    /** List of string properties */
    StringPropertyMap mStringProperties;

    /** List of Matrix properties */
    MatrixPropertyMap mMatrixProperties;

    /** List of pointer properties */
    PointerPropertyMap mPointerProperties;

    /** Used for testing - extra verbose mode causes the ValidateDataStructure-Step
     *  to be executed before and after every single post-process step */
    bool bExtraVerbose;

    /** Used by post-process steps to share data */
    SharedPostProcessInfo* mPPShared;

    /// The default class constructor.
    ImporterPimpl() AI_NO_EXCEPT;
};

inline
ImporterPimpl::ImporterPimpl() AI_NO_EXCEPT :
        mIOHandler( nullptr ),
        mIsDefaultHandler( false ),
        mProgressHandler( nullptr ),
        mIsDefaultProgressHandler( false ),
        mImporter(),
        mPostProcessingSteps(),
        mScene( nullptr ),
        mErrorString(),
        mException(),
        mIntProperties(),
        mFloatProperties(),
        mStringProperties(),
        mMatrixProperties(),
        mPointerProperties(),
        bExtraVerbose( false ),
        mPPShared( nullptr ) {
    // empty
}
//! @endcond

struct BatchData;

// ---------------------------------------------------------------------------
/** FOR IMPORTER PLUGINS ONLY: A helper class to the pleasure of importers
 *  that need to load many external meshes recursively.
 *
 *  The class uses several threads to load these meshes (or at least it
 *  could, this has not yet been implemented at the moment).
 *
 *  @note The class may not be used by more than one thread*/
class ASSIMP_API BatchLoader {
public:
    //! @cond never
    // -------------------------------------------------------------------
    /** Wraps a full list of configuration properties for an importer.
     *  Properties can be set using SetGenericProperty */
    struct PropertyMap {
        ImporterPimpl::IntPropertyMap     ints;
        ImporterPimpl::FloatPropertyMap   floats;
        ImporterPimpl::StringPropertyMap  strings;
        ImporterPimpl::MatrixPropertyMap  matrices;

        bool operator == (const PropertyMap& prop) const {
            // fixme: really isocpp? gcc complains
            return ints == prop.ints && floats == prop.floats && strings == prop.strings && matrices == prop.matrices;
        }

        bool empty () const {
            return ints.empty() && floats.empty() && strings.empty() && matrices.empty();
        }
    };
    //! @endcond

    // -------------------------------------------------------------------
    /** Construct a batch loader from a given IO system to be used
     *  to access external files
     */
    explicit BatchLoader(IOSystem* pIO, bool validate = false );

    // -------------------------------------------------------------------
    /** The class destructor.
     */
    ~BatchLoader();

    // -------------------------------------------------------------------
    /** Sets the validation step. True for enable validation during postprocess.
     *  @param  enable  True for validation.
     */
    void setValidation( bool enabled );

    // -------------------------------------------------------------------
    /** Returns the current validation step.
     *  @return The current validation step.
     */
    bool getValidation() const;

    // -------------------------------------------------------------------
    /** Add a new file to the list of files to be loaded.
     *  @param file File to be loaded
     *  @param steps Post-processing steps to be executed on the file
     *  @param map Optional configuration properties
     *  @return 'Load request channel' - an unique ID that can later
     *    be used to access the imported file data.
     *  @see GetImport */
    unsigned int AddLoadRequest (
        const std::string& file,
        unsigned int steps = 0,
            const PropertyMap *map = nullptr
        );

    // -------------------------------------------------------------------
    /** Get an imported scene.
     *  This polls the import from the internal request list.
     *  If an import is requested several times, this function
     *  can be called several times, too.
     *
     *  @param which LRWC returned by AddLoadRequest().
     *  @return nullptr if there is no scene with this file name
     *  in the queue of the scene hasn't been loaded yet. */
    aiScene* GetImport(
        unsigned int which
        );

    // -------------------------------------------------------------------
    /** Waits until all scenes have been loaded. This returns
     *  immediately if no scenes are queued.*/
    void LoadAll();

private:
    // No need to have that in the public API ...
    BatchData *m_data;
};

} // Namespace Assimp

#endif // INCLUDED_AI_IMPORTER_H
