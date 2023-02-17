/*
 * Created by elecfrog@du
 * 2023/02/16
 * Load .obj .gltf .fbx files
 * Incomplete form wait to implement
 */
#include "TinyObjLoader/tiny_obj_loader.h"

#include "IO/RHI/Texture.h"
#include "IO/Model.h"
#include "IO/Mesh.h"
#include "IO/Material.h"

#include "Maths/MathUtils.hpp"
#include "Utils/MacroUtils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace SPW
{
void Model::LoadOBJ(const FilePath& _filePath)
{
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;

	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, 
                        _filePath.u8string().c_str(), 
                        _filePath.parent_path().u8string().c_str(), 
                        true); // Enable Trianglization to Support OpenGL
	if(!success)
	{
		//TODO: SPW_LOG_CRITICAL(ERROR);
		throw std::logic_error(err.c_str());
	}

	bool singleMesh = shapes.size() == 1;

	if (attrib.texcoord_ws.empty())
	{
		BP();
	}

	for (unsigned int shapeIndex = 0; shapeIndex < shapes.size(); ++shapeIndex)
	{
		auto& shape = shapes[shapeIndex];
		SharedPtr<Mesh> tmp = std::make_shared<Mesh>();

		//TODO: Using uniqueVertices;
		// std::unordered_map<AttribVertex, Count> uniqueVertices;

		// define the index offset
		Count index_offset = 0;
		// For each face
		for (Count f = 0; f < shape.mesh.num_face_vertices.size(); f++) 
		{
			Count fnum = shape.mesh.num_face_vertices[f];
		
			// For each vertex in the face
			for (Count v = 0; v < fnum; v++)
			{
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

				// check the validation of UVs
				glm::vec2 currentUV = (int(idx.texcoord_index) == -1 || attrib.texcoords.empty()) ? 
					glm::vec2(0.0f, 0.0f) :
					glm::vec2{ attrib.texcoords[2 * idx.texcoord_index], 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1] };

				// check the validation of Normals
				glm::vec3 currentNormal = (attrib.normals.empty()) ?
					glm::vec3(0.0f, 0.0f, 0.0f) :
				// TODO: GenNormals Methods
					glm::vec3{ attrib.normals[3 * idx.normal_index], attrib.normals[3 * idx.normal_index + 1],attrib.normals[3 * idx.normal_index + 2] };


				// TODO: Tangent, Color APIs in AttribVertex
                
                // check the validation of Colors
                glm::vec4 currentColor = glm::vec4(0.0f);

                // TODO: Color Correction
                // if(shape.mesh.material_ids[0] >= 0)
                // {
                //     tinyobj::material_t* mp = &materials[shape.mesh.material_ids[0]];
                //     currentColor                  = glm::vec4(mp->diffuse[0], mp->diffuse[1], mp->diffuse[2], 1.0f);
                // }

				tmp->m_Vertices.emplace_back(
					AttribVertex{
						glm::vec3{attrib.vertices[3 * idx.vertex_index], attrib.vertices[3 * idx.vertex_index + 1],attrib.vertices[3 * idx.vertex_index + 2]},
						currentColor,
						currentUV,
                        currentNormal,
                        glm::vec3{0.f}
					});

				tmp->m_Indices.emplace_back(index_offset + v);
			}
			index_offset += fnum;
		}

        // TODO: Material Loading Process Correction
		MaterialProperties tmp_prop;
		tmp_prop.diffuse = ToGLMVec3(materials[shapeIndex].diffuse);
		tmp_prop.specular = ToGLMVec3(materials[shapeIndex].specular);
		tmp_prop.ambient = ToGLMVec3(materials[shapeIndex].ambient);
		tmp_prop.shininess = materials[shapeIndex].shininess;
		tmp_prop.IOR = materials[shapeIndex].ior;
		tmp_prop.illum = materials[shapeIndex].illum;
		std::shared_ptr<Material> tmpMaterial = std::make_shared<Material>(tmp_prop);
		tmp->m_Material = std::move(tmpMaterial);

		m_Meshes.emplace_back(tmp);
	}
}
}
