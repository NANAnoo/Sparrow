// internal headers
#include "CalcTangentsProcess.h"
#include "ProcessHelper.h"
#include <assimp/TinyFormatter.h>
#include <assimp/qnan.h>

using namespace Assimp;

// ------------------------------------------------------------------------------------------------
// Constructor to be privately used by Importer
CalcTangentsProcess::CalcTangentsProcess() :
        configMaxAngle(float(AI_DEG_TO_RAD(45.f))), configSourceUV(0) {
    // nothing to do here
}

// ------------------------------------------------------------------------------------------------
// Destructor, private as well
CalcTangentsProcess::~CalcTangentsProcess() = default;

// ------------------------------------------------------------------------------------------------
// Returns whether the processing step is present in the given flag field.
bool CalcTangentsProcess::IsActive(unsigned int pFlags) const {
    return (pFlags & aiProcess_CalcTangentSpace) != 0;
}

// ------------------------------------------------------------------------------------------------
// Executes the post processing step on the given imported data.
void CalcTangentsProcess::SetupProperties(const Importer *pImp) {
    ai_assert(nullptr != pImp);

    // get the current value of the property
    configMaxAngle = pImp->GetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 45.f);
    configMaxAngle = std::max(std::min(configMaxAngle, 45.0f), 0.0f);
    configMaxAngle = AI_DEG_TO_RAD(configMaxAngle);

    configSourceUV = pImp->GetPropertyInteger(AI_CONFIG_PP_CT_TEXTURE_CHANNEL_INDEX, 0);
}

// ------------------------------------------------------------------------------------------------
// Executes the post processing step on the given imported data.
void CalcTangentsProcess::Execute(aiScene *pScene) {
    ai_assert(nullptr != pScene);

    ASSIMP_LOG_DEBUG("CalcTangentsProcess begin");

    bool bHas = false;
    for (unsigned int a = 0; a < pScene->mNumMeshes; a++) {
        if (ProcessMesh(pScene->mMeshes[a], a)) bHas = true;
    }

    if (bHas) {
        ASSIMP_LOG_INFO("CalcTangentsProcess finished. Tangents have been calculated");
    } else {
        ASSIMP_LOG_DEBUG("CalcTangentsProcess finished");
    }
}

inline glm::vec3 ToGLM3(const aiVector3D& aiVec3D) {
    return glm::vec3(aiVec3D.x, aiVec3D.y, aiVec3D.z);
}
        // ------------------------------------------------------------------------------------------------
// Calculates tangents and bi-tangents for the given mesh
bool CalcTangentsProcess::ProcessMesh(aiMesh *pMesh, unsigned int meshIndex) {
    // we assume that the mesh is still in the verbose vertex format where each face has its own set
    // of vertices and no vertices are shared between faces. Sadly I don't know any quick test to
    // assert() it here.
    // assert( must be verbose, dammit);

    if (pMesh->mTangents) // this implies that mBitangents is also there
        return false;

    // If the mesh consists of lines and/or points but not of
    // triangles or higher-order polygons the normal vectors
    // are undefined.
    if (!(pMesh->mPrimitiveTypes & (aiPrimitiveType_TRIANGLE | aiPrimitiveType_POLYGON))) {
        ASSIMP_LOG_INFO("Tangents are undefined for line and point meshes");
        return false;
    }

    // what we can check, though, is if the mesh has normals and texture coordinates. That's a requirement
    if (pMesh->mNormals == nullptr) {
        ASSIMP_LOG_ERROR("Failed to compute tangents; need normals");
        return false;
    }
    if (configSourceUV >= AI_MAX_NUMBER_OF_TEXTURECOORDS || !pMesh->mTextureCoords[configSourceUV]) {
        ASSIMP_LOG_ERROR("Failed to compute tangents; need UV data in channel", configSourceUV);
        return false;
    }

    const float angleEpsilon = 0.9999f;

    std::vector<bool> vertexDone(pMesh->mNumVertices, false);
    const float qnan = get_qnan();

    // create space for the tangents and bitangents
    pMesh->mTangents = new aiVector3D[pMesh->mNumVertices];
    pMesh->mBitangents = new aiVector3D[pMesh->mNumVertices];

    const aiVector3D *meshPos = pMesh->mVertices;
    const aiVector3D *meshNorm = pMesh->mNormals;
    const aiVector3D *meshTex = pMesh->mTextureCoords[configSourceUV];
    aiVector3D *meshTang = pMesh->mTangents;
    aiVector3D *meshBitang = pMesh->mBitangents;

    // calculate the tangent and bitangent for every face
    for (unsigned int a = 0; a < pMesh->mNumFaces; a++) {
        const aiFace &face = pMesh->mFaces[a];
        if (face.mNumIndices < 3) {
            // There are less than three indices, thus the tangent vector
            // is not defined. We are finished with these vertices now,
            // their tangent vectors are set to qnan.
            for (unsigned int i = 0; i < face.mNumIndices; ++i) {
                unsigned int idx = face.mIndices[i];
                vertexDone[idx] = true;
                meshTang[idx] = aiVector3D(qnan);
                meshBitang[idx] = aiVector3D(qnan);
            }

            continue;
        }

        // triangle or polygon... we always use only the first three indices. A polygon
        // is supposed to be planar anyways....
        // FIXME: (thom) create correct calculation for multi-vertex polygons maybe?
        const unsigned int p0 = face.mIndices[0], p1 = face.mIndices[1], p2 = face.mIndices[2];

        // position differences p1->p2 and p1->p3
        aiVector3D v = meshPos[p1] - meshPos[p0], w = meshPos[p2] - meshPos[p0];

        // texture offset p1->p2 and p1->p3
        float sx = meshTex[p1].x - meshTex[p0].x, sy = meshTex[p1].y - meshTex[p0].y;
        float tx = meshTex[p2].x - meshTex[p0].x, ty = meshTex[p2].y - meshTex[p0].y;
        float dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;
        // when t1, t2, t3 in same position in UV space, just use default UV direction.
        if (sx * ty == sy * tx) {
            sx = 0.0;
            sy = 1.0;
            tx = 1.0;
            ty = 0.0;
        }

        // tangent points in the direction where to positive X axis of the texture coord's would point in model space
        // bitangent's points along the positive Y axis of the texture coord's, respectively
        glm::vec3 tangent{};
        glm::vec3 bitangent{};

        // aiVector3D tangent, bitangent;
        tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
        tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
        tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
        bitangent.x = (- w.x * sx + v.x * tx) * dirCorrection;
        bitangent.y = (- w.y * sx + v.y * tx) * dirCorrection;
        bitangent.z = (- w.z * sx + v.z * tx) * dirCorrection;

        // store for every vertex of that face
        for (unsigned int b = 0; b < face.mNumIndices; ++b) {
            unsigned int p = face.mIndices[b];

            // project tangent and bitangent into the plane formed by the vertex' normal
            glm::vec3 localTangent = tangent - ToGLM3(meshNorm[p]) * (tangent * ToGLM3(meshNorm[p]));
            glm::vec3 localBitangent = bitangent - ToGLM3(meshNorm[p]) * (bitangent * ToGLM3(meshNorm[p])) - localTangent * (bitangent * localTangent);

            glm::normalize(localTangent);
            glm::normalize(localBitangent);

            // reconstruct tangent/bitangent according to normal and bitangent/tangent when it's infinite or NaN.
            bool invalid_tangent = is_special_float(localTangent.x) || is_special_float(localTangent.y) || is_special_float(localTangent.z);
            bool invalid_bitangent = is_special_float(localBitangent.x) || is_special_float(localBitangent.y) || is_special_float(localBitangent.z);
            if (invalid_tangent != invalid_bitangent) {
                if (invalid_tangent) {
                    localTangent = glm::cross(ToGLM3(meshNorm[p]), localBitangent);
                    glm::normalize(localTangent);
                } else {
                    localBitangent = glm::cross(ToGLM3(meshNorm[p]), localTangent);
                    glm::normalize(localBitangent);
                }
            }

            // and write it into the mesh.
            meshTang[p] = aiVector3D(localTangent);
            meshBitang[p] = aiVector3D(localBitangent);
        }
    }

    // create a helper to quickly find locally close vertices among the vertex array
    // FIX: check whether we can reuse the SpatialSort of a previous step
    SpatialSort *vertexFinder = nullptr;
    SpatialSort _vertexFinder;
    float posEpsilon = 10e-6f;
    if (shared) {
        std::vector<std::pair<SpatialSort, float>> *avf;
        shared->GetProperty(AI_SPP_SPATIAL_SORT, avf);
        if (avf) {
            std::pair<SpatialSort, float> &blubb = avf->operator[](meshIndex);
            vertexFinder = &blubb.first;
            posEpsilon = blubb.second;
            ;
        }
    }
    if (!vertexFinder) {
        _vertexFinder.Fill(pMesh->mVertices, pMesh->mNumVertices, sizeof(aiVector3D));
        vertexFinder = &_vertexFinder;
        posEpsilon = ComputePositionEpsilon(pMesh);
    }
    std::vector<unsigned int> verticesFound;

    const float fLimit = std::cos(configMaxAngle);
    std::vector<unsigned int> closeVertices;

    // in the second pass we now smooth out all tangents and bitangents at the same local position
    // if they are not too far off.
    for (unsigned int a = 0; a < pMesh->mNumVertices; a++) {
        if (vertexDone[a])
            continue;

        const aiVector3D &origPos = pMesh->mVertices[a];
        const aiVector3D &origNorm = pMesh->mNormals[a];
        const aiVector3D &origTang = pMesh->mTangents[a];
        const aiVector3D &origBitang = pMesh->mBitangents[a];
        closeVertices.resize(0);

        // find all vertices close to that position
        vertexFinder->FindPositions(origPos, posEpsilon, verticesFound);

        closeVertices.reserve(verticesFound.size() + 5);
        closeVertices.push_back(a);

        // look among them for other vertices sharing the same normal and a close-enough tangent/bitangent
        for (unsigned int b = 0; b < verticesFound.size(); b++) {
            unsigned int idx = verticesFound[b];
            if (vertexDone[idx])
                continue;
            if (meshNorm[idx] * origNorm < angleEpsilon)
                continue;
            if (meshTang[idx] * origTang < fLimit)
                continue;
            if (meshBitang[idx] * origBitang < fLimit)
                continue;

            // it's similar enough -> add it to the smoothing group
            closeVertices.push_back(idx);
            vertexDone[idx] = true;
        }

        // smooth the tangents and bitangents of all vertices that were found to be close enough
        aiVector3D smoothTangent(0, 0, 0), smoothBitangent(0, 0, 0);
        for (unsigned int b = 0; b < closeVertices.size(); ++b) {
            smoothTangent += meshTang[closeVertices[b]];
            smoothBitangent += meshBitang[closeVertices[b]];
        }
        smoothTangent.Normalize();
        smoothBitangent.Normalize();

        // and write it back into all affected tangents
        for (unsigned int b = 0; b < closeVertices.size(); ++b) {
            meshTang[closeVertices[b]] = smoothTangent;
            meshBitang[closeVertices[b]] = smoothBitangent;
        }
    }
    return true;
}
