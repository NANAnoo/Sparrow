
/** @file Defines a post processing step to calculate tangents and
    bi-tangents on all imported meshes.*/
#ifndef AI_CALCTANGENTSPROCESS_H_INC
#define AI_CALCTANGENTSPROCESS_H_INC

#include "Common/BaseProcess.h"
#include <glm/glm.hpp>

struct aiMesh;

namespace Assimp
{

// ---------------------------------------------------------------------------
/** The CalcTangentsProcess calculates the tangent and bitangent for any vertex
 * of all meshes. It is expected to be run before the JoinVerticesProcess runs
 * because the joining of vertices also considers tangents and bitangents for
 * uniqueness.
 */
class ASSIMP_API_WINONLY CalcTangentsProcess : public BaseProcess
{
public:

    CalcTangentsProcess();
    ~CalcTangentsProcess();

public:
    // -------------------------------------------------------------------
    /** Returns whether the processing step is present in the given flag.
    * @param pFlags The processing flags the importer was called with.
    *   A bitwise combination of #aiPostProcessSteps.
    * @return true if the process is present in this flag fields,
    *   false if not.
    */
    bool IsActive( unsigned int pFlags) const;

    // -------------------------------------------------------------------
    /** Called prior to ExecuteOnScene().
    * The function is a request to the process to update its configuration
    * basing on the Importer's configuration property list.
    */
    void SetupProperties(const Importer* pImp);


    // setter for configMaxAngle
    inline void SetMaxSmoothAngle(float f)
    {
        configMaxAngle =f;
    }

protected:

    // -------------------------------------------------------------------
    /** Calculates tangents and bitangents for a specific mesh.
    * @param pMesh The mesh to process.
    * @param meshIndex Index of the mesh
    */
    bool ProcessMesh( aiMesh* pMesh, unsigned int meshIndex);

    // -------------------------------------------------------------------
    /** Executes the post processing step on the given imported data.
    * @param pScene The imported data to work at.
    */
    void Execute( aiScene* pScene);

private:

    /** Configuration option: maximum smoothing angle, in radians*/
    float configMaxAngle;
    unsigned int configSourceUV;
};

} // end of namespace Assimp

#endif // AI_CALCTANGENTSPROCESS_H_INC
