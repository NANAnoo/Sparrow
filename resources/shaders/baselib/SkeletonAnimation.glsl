#ifndef SKELETON_ANIMATION_GLSL
#define SKELETON_ANIMATION_GLSL

uniform int currentFrame;
uniform int boneCount;

layout (std430) readonly buffer WeightDictStart {
    int starts[];
} weightDictStart;

layout (std430) buffer WeightDictSize {
    int sizes[];
} weightDictSize;

layout (std430) buffer WeightDictKey {
    int keys[];
} weightDictKey;

layout (std430) buffer WeightDictValue {
    float weights[];
} weightDictValue;

layout (std430) buffer WeightMatries {
    mat4 mats[];
} weightMatries;

vec4 animationTransForm(vec4 position, int index) {
    int start = weightDictStart.starts[index];
    int end = start + weightDictSize.sizes[index];
    mat4 transform;
    for (int i = start; i < end; i ++) {
        int which_bone = weightDictKey.keys[i];
        float weight = weightDictValue.weights[i];
        transform += weightMatries.mats[boneCount * currentFrame + which_bone] * weight;
    }
    return transform * position;
}
#endif
