/*
 * @date   2023-04-09
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include "glm/glm.hpp"
#include "Asset/Asset.hpp"
#include "Vertex.h"

namespace SPW
{
	struct MeshData : public Asset
	{
		std::vector<Vertex>         vertices;
		std::vector<uint32_t>       indices;
		std::string                 m_MateialID;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("meta", cereal::base_class<Asset>(this)),
				cereal::make_nvp("vertices", vertices),
				cereal::make_nvp("indices", indices),
				cereal::make_nvp("material_id", m_MateialID)
			);
		}
	};
}