#ifndef SKELETON_ANIMATION_GLSL
#define SKELETON_ANIMATION_GLSL

uniform int currentFrame;

layout (std430) buffer WeightDictStart {
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
    int boneNum;
    mat4 mats[];
} weightMatries;

const int max = 10;
vec4 animationTransForm(vec4 position, int index) {
    int start = weightDictStart.starts[index];
    int end = start + weightDictSize.sizes[index];
    mat4 transform;
    for (int i = start; i < end && i < max + start; i ++) {
        int mat_index = weightDictKey.keys[i];
        float weight = weightDictValue.weights[i];
        transform += weightMatries.mats[weightMatries.boneNum * currentFrame + mat_index] * weight;
    }
    return position * transform;
}
#endif
