/** @file  vector3.inl
 *  @brief Inline implementation of aiVector3t<TReal> operators
 */
#pragma once
#ifndef AI_VECTOR3D_INL_INC
#define AI_VECTOR3D_INL_INC

#ifdef __cplusplus
#include <assimp/vector3.h>

#include <cmath>

// ------------------------------------------------------------------------------------------------
/** Transformation of a vector by a 3x3 matrix */
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator * (const aiMatrix3x3t<TReal>& pMatrix, const aiVector3t<TReal>& pVector) {
    aiVector3t<TReal> res;
    res.x = pMatrix.a1 * pVector.x + pMatrix.a2 * pVector.y + pMatrix.a3 * pVector.z;
    res.y = pMatrix.b1 * pVector.x + pMatrix.b2 * pVector.y + pMatrix.b3 * pVector.z;
    res.z = pMatrix.c1 * pVector.x + pMatrix.c2 * pVector.y + pMatrix.c3 * pVector.z;
    return res;
}

// ------------------------------------------------------------------------------------------------
/** Transformation of a vector by a 4x4 matrix */
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator * (const aiMatrix4x4t<TReal>& pMatrix, const aiVector3t<TReal>& pVector) {
    aiVector3t<TReal> res;
    res.x = pMatrix.a1 * pVector.x + pMatrix.a2 * pVector.y + pMatrix.a3 * pVector.z + pMatrix.a4;
    res.y = pMatrix.b1 * pVector.x + pMatrix.b2 * pVector.y + pMatrix.b3 * pVector.z + pMatrix.b4;
    res.z = pMatrix.c1 * pVector.x + pMatrix.c2 * pVector.y + pMatrix.c3 * pVector.z + pMatrix.c4;
    return res;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
template <typename TOther>
aiVector3t<TReal>::operator aiVector3t<TOther> () const {
    return aiVector3t<TOther>(static_cast<TOther>(x),static_cast<TOther>(y),static_cast<TOther>(z));
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE void aiVector3t<TReal>::Set( TReal pX, TReal pY, TReal pZ) {
    x = pX;
    y = pY;
    z = pZ;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE TReal aiVector3t<TReal>::SquareLength() const {
    return x*x + y*y + z*z;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE TReal aiVector3t<TReal>::Length() const {
    return std::sqrt( SquareLength());
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
 aiVector3t<TReal>& aiVector3t<TReal>::Normalize() {
    const TReal l = Length();
    if (l == 0) {
        return *this;
    }
    *this /= Length();

    return *this;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE aiVector3t<TReal>& aiVector3t<TReal>::NormalizeSafe() {
    TReal len = Length();
    if ( len > static_cast< TReal >( 0 ) ) {
        *this /= len;
    }
    return *this;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE const aiVector3t<TReal>& aiVector3t<TReal>::operator += (const aiVector3t<TReal>& o) {
    x += o.x;
    y += o.y;
    z += o.z;

    return *this;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE const aiVector3t<TReal>& aiVector3t<TReal>::operator -= (const aiVector3t<TReal>& o) {
    x -= o.x;
    y -= o.y;
    z -= o.z;

    return *this;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE const aiVector3t<TReal>& aiVector3t<TReal>::operator *= (TReal f) {
    x *= f;
    y *= f;
    z *= f;

    return *this;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE const aiVector3t<TReal>& aiVector3t<TReal>::operator /= (TReal f) {
    if ( f == static_cast<TReal>(0.0)) {
        return *this;
    }
    const TReal invF = (TReal) 1.0 / f;
    x *= invF;
    y *= invF;
    z *= invF;

    return *this;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE aiVector3t<TReal>& aiVector3t<TReal>::operator *= (const aiMatrix3x3t<TReal>& mat){
    return (*this =  mat * (*this));
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE aiVector3t<TReal>& aiVector3t<TReal>::operator *= (const aiMatrix4x4t<TReal>& mat){
    return (*this = mat * (*this));
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE TReal aiVector3t<TReal>::operator[](unsigned int i) const {
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            break;
    }
    return x;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE TReal& aiVector3t<TReal>::operator[](unsigned int i) {
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            break;
    }
    return x;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE bool aiVector3t<TReal>::operator== (const aiVector3t<TReal>& other) const {
    return x == other.x && y == other.y && z == other.z;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE bool aiVector3t<TReal>::operator!= (const aiVector3t<TReal>& other) const {
    return x != other.x || y != other.y || z != other.z;
}
// ---------------------------------------------------------------------------
template<typename TReal>
FORCE_INLINE bool aiVector3t<TReal>::Equal(const aiVector3t<TReal>& other, TReal epsilon) const {
    return
        std::abs(x - other.x) <= epsilon &&
        std::abs(y - other.y) <= epsilon &&
        std::abs(z - other.z) <= epsilon;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE bool aiVector3t<TReal>::operator < (const aiVector3t<TReal>& other) const {
    return x != other.x ? x < other.x : y != other.y ? y < other.y : z < other.z;
}
// ------------------------------------------------------------------------------------------------
template <typename TReal>
FORCE_INLINE const aiVector3t<TReal> aiVector3t<TReal>::SymMul(const aiVector3t<TReal>& o) {
    return aiVector3t<TReal>(x*o.x,y*o.y,z*o.z);
}
// ------------------------------------------------------------------------------------------------
// symmetric addition
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator + (const aiVector3t<TReal>& v1, const aiVector3t<TReal>& v2) {
    return aiVector3t<TReal>( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
// ------------------------------------------------------------------------------------------------
// symmetric subtraction
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator - (const aiVector3t<TReal>& v1, const aiVector3t<TReal>& v2) {
    return aiVector3t<TReal>( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
// ------------------------------------------------------------------------------------------------
// scalar product
template <typename TReal>
FORCE_INLINE TReal operator * (const aiVector3t<TReal>& v1, const aiVector3t<TReal>& v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
// ------------------------------------------------------------------------------------------------
// scalar multiplication
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator * ( TReal f, const aiVector3t<TReal>& v) {
    return aiVector3t<TReal>( f*v.x, f*v.y, f*v.z);
}
// ------------------------------------------------------------------------------------------------
// and the other way around
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator * ( const aiVector3t<TReal>& v, TReal f) {
    return aiVector3t<TReal>( f*v.x, f*v.y, f*v.z);
}
// ------------------------------------------------------------------------------------------------
// scalar division
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator / ( const aiVector3t<TReal>& v, TReal f) {
    return v * (1/f);
}
// ------------------------------------------------------------------------------------------------
// vector division
template <typename TReal>
FORCE_INLINE aiVector3t<TReal> operator / ( const aiVector3t<TReal>& v, const aiVector3t<TReal>& v2) {
    return aiVector3t<TReal>(v.x / v2.x,v.y / v2.y,v.z / v2.z);
}
// ------------------------------------------------------------------------------------------------
// cross product
template<typename TReal>
FORCE_INLINE aiVector3t<TReal> operator ^ ( const aiVector3t<TReal>& v1, const aiVector3t<TReal>& v2) {
    return aiVector3t<TReal>( v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}
// ------------------------------------------------------------------------------------------------
// vector negation
template<typename TReal>
FORCE_INLINE aiVector3t<TReal> operator - ( const aiVector3t<TReal>& v) {
    return aiVector3t<TReal>( -v.x, -v.y, -v.z);
}

// ------------------------------------------------------------------------------------------------

#endif // __cplusplus
#endif // AI_VECTOR3D_INL_INC
