// https://github.com/CedricGuillemet/Imogen
//
// The MIT License(MIT)
// 
// Copyright(c) 2018 Cedric Guillemet
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "Utils.h"

struct Camera
{
    Vec4 mPosition;
    Vec4 mDirection;
    Vec4 mUp;
    Vec4 mLens; // fov,....

    Camera Lerp(const Camera& target, float t)
    {
        Camera ret;
        ret.mPosition = ::Lerp(mPosition, target.mPosition, t);
        ret.mDirection = ::Lerp(mDirection, target.mDirection, t);
        ret.mDirection.Normalize();
        ret.mUp = ::Lerp(mUp, target.mUp, t);
        ret.mUp.Normalize();
        ret.mLens = ::Lerp(mLens, target.mLens, t);
        return ret;
    }
    float& operator[] (int index)
    {
        switch (index)
        {
        case 0:
        case 1:
        case 2:
            return mPosition[index];
        case 3:
        case 4:
        case 5:
            return mDirection[index - 3];
        case 6:
            return mDirection[index - 6];
        }
        return mPosition[0];
    }

    void ComputeViewProjectionMatrix(float *viewProj, float *viewInverse);
};
inline Camera Lerp(Camera a, Camera b, float t) { return a.Lerp(b, t); }


enum ConTypes
{
    Con_Float,
    Con_Float2,
    Con_Float3,
    Con_Float4,
    Con_Color4,
    Con_Int,
    Con_Int2,
    Con_Ramp,
    Con_Angle,
    Con_Angle2,
    Con_Angle3,
    Con_Angle4,
    Con_Enum,
    Con_Structure,
    Con_FilenameRead,
    Con_FilenameWrite,
    Con_ForceEvaluate,
    Con_Bool,
    Con_Ramp4,
    Con_Camera,
    Con_Any,
};


struct ParameterBlock
{
    std::vector<unsigned char> mBits;
};

size_t GetParameterTypeSize(ConTypes paramType);
size_t GetParameterOffset(uint32_t type, uint32_t parameterIndex);
size_t GetCurveCountPerParameterType(uint32_t paramType);
const char* GetCurveParameterSuffix(uint32_t paramType, int suffixIndex);
uint32_t GetCurveParameterColor(uint32_t paramType, int suffixIndex);
CurveType GetCurveTypeForParameterType(ConTypes paramType);
