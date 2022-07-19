#pragma once

inline Matrix FbxToMatrix(const FbxAMatrix& fbxMatrix) {
    Matrix r;
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            r.m[row][column] = static_cast<float>(fbxMatrix[row][column]);
        }
    }
    return r;
}
inline Vec4 FbxToVec4(const FbxDouble4& fbxDoouble4) {
    Vec4 r;
    r.x = static_cast<float>(fbxDoouble4[0]);
    r.y = static_cast<float>(fbxDoouble4[1]);
    r.z = static_cast<float>(fbxDoouble4[2]);
    r.w = static_cast<float>(fbxDoouble4[3]);
    return r;
}
inline Vec3 FbxToVec3(const FbxDouble3& fbxDoouble3) {
    Vec3 r;
    r.x = static_cast<float>(fbxDoouble3[0]);
    r.y = static_cast<float>(fbxDoouble3[1]);
    r.z = static_cast<float>(fbxDoouble3[2]);
    return r;
}