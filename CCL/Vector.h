#pragma once

#include <DirectXMath.h>

// Vec4
struct Vec4 : public DirectX::XMFLOAT4 {
    Vec4(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
    Vec4(float all) : Vec4(all, all, all, all) {}
    Vec4() : Vec4(0.0f, 0.0f, 0.0f, 0.0f) {}

    // TODO
    // operator

    Vec4 operator+  (Vec4 v) const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorAdd(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec4 operator+= (Vec4 v) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorAdd(this->GetXM(), v.GetXM()));
        return *this;
    }
    // x++
    // ++x
    Vec4 operator-  (Vec4 v) const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorSubtract(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec4 operator-= (Vec4 v) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorSubtract(this->GetXM(), v.GetXM()));
        return *this;
    }
    // x--
    // --x
    Vec4 operator*  (Vec4 v) const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorMultiply(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec4 operator*= (Vec4 v) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorMultiply(this->GetXM(), v.GetXM()));
        return *this;
    }
    Vec4 operator/  (Vec4 v) const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorDivide(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec4 operator/= (Vec4 v) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorDivide(this->GetXM(), v.GetXM()));
        return *this;
    }

    Vec4 operator+  (float f) const {
        Vec4 r = Vec4(f);
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorAdd(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec4 operator+= (float f) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorAdd(this->GetXM(), Vec4(f).GetXM()));
        return *this;
    }
    Vec4 operator-  (float f) const {
        Vec4 r = Vec4(f);
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorSubtract(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec4 operator-= (float f) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorSubtract(this->GetXM(), Vec4(f).GetXM()));
        return *this;
    }
    Vec4 operator*  (float f) const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorScale(this->GetXM(), f));
        return r;
    }
    Vec4 operator*= (float f) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorScale(this->GetXM(), f));
        return *this;
    }
    Vec4 operator/  (float f) const {
        Vec4 r = Vec4(f);
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorDivide(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec4 operator/= (float f) {
        DirectX::XMStoreFloat4(this, DirectX::XMVectorDivide(this->GetXM(), Vec4(f).GetXM()));
        return *this;
    }

    Vec4 operator-() {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMVectorNegate(this->GetXM()));
        return r;
    }

    bool operator==(Vec4 v) const {
        return (this->x == v.x && this->y == v.y && this->z == v.z && this->w == v.w);
    }

    float LengthSq() const {
        float r;
        r = DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(this->GetXM()));
        return r;
    }
    float Length() const {
        float r;
        r = DirectX::XMVectorGetX(DirectX::XMVector4Length(this->GetXM()));
        return r;
    }
    Vec4 Normalize() const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMVector4Normalize(this->GetXM()));
        return r;
    }
    DirectX::XMVECTOR GetXM() const {
        return DirectX::XMLoadFloat4(this);
    }
    static Vec4 QuaternionIdentity() { return Vec4(0.0f, 0.0, 0.0f, 1.0f); }
};

inline float Vector4Dot(Vec4 v0, Vec4 v1) {
    return DirectX::XMVectorGetX(DirectX::XMVector4Dot(v0.GetXM(), v1.GetXM()));
}
inline Vec4 Vector4Cross(Vec4 v0, Vec4 v1, Vec4 v2) {
    Vec4 r;
    DirectX::XMStoreFloat4(&r, DirectX::XMVector4Cross(v0.GetXM(), v1.GetXM(), v2.GetXM()));
    return r;
}

inline Vec4 operator+   (float f, Vec4 v) { return v + f; }
inline Vec4 operator+=  (float f, Vec4 v) { return v += f; }
inline Vec4 operator-   (float f, Vec4 v) { return v - f; }
inline Vec4 operator-=  (float f, Vec4 v) { return v -= f; }

// Vec3
struct Vec3 : public DirectX::XMFLOAT3 {
    Vec3(float _x, float _y, float _z) : XMFLOAT3(_x, _y, _z) {}
    Vec3(float all) : Vec3(all, all, all) {}
    Vec3() : Vec3(0.0f, 0.0f, 0.0f) {}

    Vec3 operator+  (Vec3 v) const {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorAdd(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec3 operator+= (Vec3 v) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorAdd(this->GetXM(), v.GetXM()));
        return *this;
    }
    // x++
    // ++x
    Vec3 operator-  (Vec3 v) const {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorSubtract(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec3 operator-= (Vec3 v) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorSubtract(this->GetXM(), v.GetXM()));
        return *this;
    }
    // x--
    // --x
    Vec3 operator*  (Vec3 v) const {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorMultiply(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec3 operator*= (Vec3 v) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorMultiply(this->GetXM(), v.GetXM()));
        return *this;
    }
    Vec3 operator/  (Vec3 v) const {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorDivide(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec3 operator/= (Vec3 v) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorDivide(this->GetXM(), v.GetXM()));
        return *this;
    }

    Vec3 operator+  (float f) const {
        Vec3 r = Vec3(f);
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorAdd(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec3 operator+= (float f) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorAdd(this->GetXM(), Vec3(f).GetXM()));
        return *this;
    }
    Vec3 operator-  (float f) const {
        Vec3 r = Vec3(f);
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorSubtract(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec3 operator-= (float f) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorSubtract(this->GetXM(), Vec3(f).GetXM()));
        return *this;
    }
    Vec3 operator*  (float f) const {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorScale(this->GetXM(), f));
        return r;
    }
    Vec3 operator*= (float f) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorScale(this->GetXM(), f));
        return *this;
    }
    Vec3 operator/  (float f) const {
        Vec3 r = Vec3(f);
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorDivide(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec3 operator/= (float f) {
        DirectX::XMStoreFloat3(this, DirectX::XMVectorDivide(this->GetXM(), Vec3(f).GetXM()));
        return *this;
    }

    Vec3 operator-() {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMVectorNegate(this->GetXM()));
        return r;
    }

    bool operator==(Vec3 v) const {
        return (this->x == v.x && this->y == v.y && this->z == v.z);
    }

    float LengthSq() const {
        float r;
        r = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(this->GetXM()));
        return r;
    }
    float Length() const {
        float r;
        r = DirectX::XMVectorGetX(DirectX::XMVector3Length(this->GetXM()));
        return r;
    }
    Vec3 Normalize() const {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMVector3Normalize(this->GetXM()));
        return r;
    }
    DirectX::XMVECTOR GetXM() const {
        return DirectX::XMLoadFloat3(this);
    }
    Vec4 CastToVec4() const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMLoadFloat3(this));
        return r;
    }
};

inline float Vector3Dot(Vec3 v0, Vec3 v1) {
    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0.GetXM(), v1.GetXM()));
}
inline Vec3 Vector3Cross(Vec3 v0, Vec3 v1) {
    Vec3 r;
    DirectX::XMStoreFloat3(&r, DirectX::XMVector3Cross(v0.GetXM(), v1.GetXM()));
    return r;
}

inline Vec3 operator+   (float f, Vec3 v) { return v + f; }
inline Vec3 operator+=  (float f, Vec3 v) { return v += f; }
inline Vec3 operator-   (float f, Vec3 v) { return v - f; }
inline Vec3 operator-=  (float f, Vec3 v) { return v -= f; }


// Vec2
struct Vec2 : public DirectX::XMFLOAT2 {
    Vec2(float _x, float _y) : XMFLOAT2(_x, _y) {}
    Vec2(float all) : Vec2(all, all) {}
    Vec2() : Vec2(0.0f, 0.0f) {}

    Vec2 operator+  (Vec2 v) const {
        Vec2 r;
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorAdd(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec2 operator+= (Vec2 v) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorAdd(this->GetXM(), v.GetXM()));
        return *this;
    }
    // x++
    // ++x
    Vec2 operator-  (Vec2 v) const {
        Vec2 r;
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorSubtract(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec2 operator-= (Vec2 v) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSubtract(this->GetXM(), v.GetXM()));
        return *this;
    }
    // x--
    // --x
    Vec2 operator*  (Vec2 v) const {
        Vec2 r;
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorMultiply(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec2 operator*= (Vec2 v) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorMultiply(this->GetXM(), v.GetXM()));
        return *this;
    }
    Vec2 operator/  (Vec2 v) const {
        Vec2 r;
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorDivide(this->GetXM(), v.GetXM()));
        return r;
    }
    Vec2 operator/= (Vec2 v) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorDivide(this->GetXM(), v.GetXM()));
        return *this;
    }

    Vec2 operator+  (float f) const {
        Vec2 r = Vec2(f);
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorAdd(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec2 operator+= (float f) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorAdd(this->GetXM(), Vec2(f).GetXM()));
        return *this;
    }
    Vec2 operator-  (float f) const {
        Vec2 r = Vec2(f);
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorSubtract(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec2 operator-= (float f) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorSubtract(this->GetXM(), Vec2(f).GetXM()));
        return *this;
    }
    Vec2 operator*  (float f) const {
        Vec2 r;
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorScale(this->GetXM(), f));
        return r;
    }
    Vec2 operator*= (float f) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorScale(this->GetXM(), f));
        return *this;
    }
    Vec2 operator/  (float f) const {
        Vec2 r = Vec2(f);
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorDivide(this->GetXM(), r.GetXM()));
        return r;
    }
    Vec2 operator/= (float f) {
        DirectX::XMStoreFloat2(this, DirectX::XMVectorDivide(this->GetXM(), Vec2(f).GetXM()));
        return *this;
    }

    Vec2 operator-() {
        Vec2 r;
        DirectX::XMStoreFloat2(&r, DirectX::XMVectorNegate(this->GetXM()));
        return r;
    }

    bool operator==(Vec2 v) const {
        return (this->x == v.x && this->y == v.y);
    }

    float LengthSq() const {
        float r;
        r = DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(this->GetXM()));
        return r;
    }
    float Length() const {
        float r;
        r = DirectX::XMVectorGetX(DirectX::XMVector2Length(this->GetXM()));
        return r;
    }
    Vec2 Normalize() const {
        Vec2 r;
        DirectX::XMStoreFloat2(&r, DirectX::XMVector2Normalize(this->GetXM()));
        return r;
    }
    DirectX::XMVECTOR GetXM() const {
        return DirectX::XMLoadFloat2(this);
    }
    Vec3 CastToVec3() const {
        Vec3 r;
        DirectX::XMStoreFloat3(&r, DirectX::XMLoadFloat2(this));
        return r;
    }
    Vec4 CastToVec4() const {
        Vec4 r;
        DirectX::XMStoreFloat4(&r, DirectX::XMLoadFloat2(this));
        return r;
    }
};

inline float Vector2Dot(Vec2 v0, Vec2 v1) {
    return DirectX::XMVectorGetX(DirectX::XMVector2Dot(v0.GetXM(), v1.GetXM()));
}
inline Vec2 Vector2Cross(Vec2 v0, Vec2 v1) {
    Vec2 r;
    DirectX::XMStoreFloat2(&r, DirectX::XMVector2Cross(v0.GetXM(), v1.GetXM()));
    return r;
}

inline Vec2 operator+   (float f, Vec2 v) { return v + f; }
inline Vec2 operator+=  (float f, Vec2 v) { return v += f; }
inline Vec2 operator-   (float f, Vec2 v) { return v - f; }
inline Vec2 operator-=  (float f, Vec2 v) { return v -= f; }

// Matrix
class Matrix : public DirectX::XMFLOAT4X4 {
public:
    Vec3 GetRight() { return Vec3(m[0][0], m[0][1], m[0][2]).Normalize(); }
    Vec3 GetUp()    { return Vec3(m[1][0], m[1][1], m[1][2]).Normalize(); }
    Vec3 GetFront() { return Vec3(m[2][0], m[2][1], m[2][2]).Normalize(); }
    Vec3 GetTrans() { return Vec3(m[3][0], m[3][1], m[3][2]); }

    Matrix Inverse() const {
        Matrix r;
        DirectX::XMStoreFloat4x4(&r, DirectX::XMMatrixInverse(nullptr, this->GetXM()));
        return r;
    }
    DirectX::XMMATRIX GetXM() const { return DirectX::XMLoadFloat4x4(this); }


    Matrix operator* (const Matrix& mat) const {
        using namespace DirectX;
        Matrix r;
        XMStoreFloat4x4(&r, this->GetXM() * mat.GetXM());
        return r;
    }
    Matrix operator*= (const Matrix& mat) {
        using namespace DirectX;
        XMStoreFloat4x4(this, this->GetXM() * mat.GetXM());
        return *this;
    }

    Matrix(
        float _11_, float _12_, float _13_, float _14_,
        float _21_, float _22_, float _23_, float _24_,
        float _31_, float _32_, float _33_, float _34_,
        float _41_, float _42_, float _43_, float _44_) 
        : DirectX::XMFLOAT4X4(
        _11_, _12_, _13_, _14_,
        _21_, _22_, _23_, _24_,
        _31_, _32_, _33_, _34_,
        _41_, _42_, _43_, _44_
    ) {}
    Matrix() { DirectX::XMStoreFloat4x4(this, DirectX::XMMatrixIdentity()); }
    Matrix(Vec3 right, Vec3 up, Vec3 front, Vec3 translation)
        : DirectX::XMFLOAT4X4(
            right.x,        right.y,        right.z,        0.0f,
            up.x,           up.y,           up.z,           0.0f,
            front.x,        front.y,        front.z,        0.0f,
            translation.x,  translation.y,  translation.z,  1.0f
        ) {}
    Matrix(Vec4 right, Vec4 up, Vec4 front, Vec4 translation) 
        : DirectX::XMFLOAT4X4(
            right.x,        right.y,        right.z,        right.w,
            up.x,           up.y,           up.z,           up.w,
            front.x,        front.y,        front.z,        front.w,
            translation.x,  translation.y,  translation.z,  translation.w
        ) {}
};
class Transform {
public:
    Matrix transform;

    union Rotation {
        Vec4 pitchYawRoll;
        Vec4 quaternion;

        Vec4 Get() { return pitchYawRoll; }
        Rotation(Vec4 pitchYawRoll) : pitchYawRoll(pitchYawRoll) {}
    } rotation;
    Vec3 scale;
    Vec3 translation;

public:
    Transform(bool initializeCalcUseRPY = true, Vec3 scale = Vec3(1.0f), Vec4 rotation = Vec4(0.0f, 0.0f, 0.0f, 1.0f), Vec3 translation = Vec3(0.0f))
        : scale(scale), rotation(rotation), translation(translation) {
        if (initializeCalcUseRPY) { UpdateTransformRPY(); }
        else { UpdateTransformQuaternion(); }
    }

public:
    void UpdateTransformRPY() {
        using namespace DirectX;

        XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
        XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.Get().x, rotation.Get().y, rotation.Get().z);
        XMMATRIX T = XMMatrixTranslation(translation.x, translation.y, translation.z);

        DirectX::XMStoreFloat4x4(&transform, S * R * T);
    }
    void UpdateTransformQuaternion() {
        using namespace DirectX;

        XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
        XMMATRIX R = XMMatrixRotationQuaternion(rotation.Get().GetXM());
        XMMATRIX T = XMMatrixTranslation(translation.x, translation.y, translation.z);

        DirectX::XMStoreFloat4x4(&transform, S * R * T);
    }
public:
    static const Matrix coodinateSystemTransforms[8];

};