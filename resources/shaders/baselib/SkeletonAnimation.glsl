#ifndef SKELETON_ANIMATION_GLSL
#define SKELETON_ANIMATION_GLSL

layout (std430) uniform WeightDictStart {
    int array[];
};

layout (std430) uniform WeightDictSize {
    int array[];
};

layout (std430) uniform WeightDictKey {
    int array[];
};

layout (std430) uniform WeightDictValue {
    float array[];
};

layout (std430) uniform WeightMatries {
    mat4 weights[];
};

const int max = 10;
vec4 animationTransForm(vec4 position, int index) {
    int start = WeightDictStart.array[index];
    int end = start + WeightDictSize.array[index];
    mat4 transform;
    for (int i = start; i < end && i < max + start; i ++) {
        int mat_index = WeightDictKey[i];
        float weight = WeightDictValue[i];
        transform += WeightMatries[mat_index] * weight;
    }
    return position * transform;
}

#endif
