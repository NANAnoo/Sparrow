#ifndef SKELETON_ANIMATION_GLSL
#define SKELETON_ANIMATION_GLSL

uniform int currentFrame;
uniform int boneCount;

layout (std430) readonly buffer WeightDictStart {
    int starts[];
} weightDictStart;

layout (std430) readonly buffer WeightDictSize {
    int sizes[];
} weightDictSize;

layout (std430) readonly buffer WeightDictKey {
    int keys[];
} weightDictKey;

layout (std430) readonly buffer WeightDictValue {
    float weights[];
} weightDictValue;

layout (std430) readonly buffer WeightMatries {
    mat4 mats[];
} weightMatries;

vec4 animationTransForm(vec4 position, unsigned int index) {
    int start = weightDictStart.starts[index];
    int end = start + weightDictSize.sizes[index];
    vec4 totalPosition = vec4(0.0f);

    for (int i = start; i < end; i ++) {
        int which_bone = weightDictKey.keys[i];
        float weight = weightDictValue.weights[i];
        vec4 localPosition = weightMatries.mats[which_bone] * position;

        totalPosition += localPosition * weight;
    }
    return totalPosition;
}

mat4 animationTransFormMat(unsigned int index) {
    int start = weightDictStart.starts[index];
    int end = start + weightDictSize.sizes[index];
    mat4 res = mat4(0.0f);

    for (int i = start; i < end; i ++) {
        int which_bone = weightDictKey.keys[i];
        float weight = weightDictValue.weights[i];
        res += weightMatries.mats[which_bone] * weight;
    }
    return res;
}

#endif
