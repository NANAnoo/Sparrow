#pragma once

#include "AnimDefinitions.h"
#include <vector>
class Skeleton
{
public:
	Skeleton() = default;
	std::vector<std::shared_ptr<AnimationClip>> m_animClips;
	std::vector<std::shared_ptr<BoneInfo>> m_Bones;
};