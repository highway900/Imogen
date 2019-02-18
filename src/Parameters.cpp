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

#include "Parameters.h"

size_t GetParameterTypeSize(ConTypes paramType)
{
    switch (paramType)
    {
    case Con_Angle:
    case Con_Float:
        return sizeof(float);
    case Con_Angle2:
    case Con_Float2:
        return sizeof(float) * 2;
    case Con_Angle3:
    case Con_Float3:
        return sizeof(float) * 3;
    case Con_Angle4:
    case Con_Color4:
    case Con_Float4:
        return sizeof(float) * 4;
    case Con_Ramp:
        return sizeof(float) * 2 * 8;
    case Con_Ramp4:
        return sizeof(float) * 4 * 8;
    case Con_Enum:
    case Con_Int:
        return sizeof(int);
    case Con_Int2:
        return sizeof(int) * 2;
    case Con_FilenameRead:
    case Con_FilenameWrite:
        return 1024;
    case Con_ForceEvaluate:
        return 0;
    case Con_Bool:
        return sizeof(int);
    case Con_Camera:
        return sizeof(Camera);
    default:
        assert(0);
    }
    return -1;
}

static const char* parameterNames[] = {
    "Float",
    "Float2",
    "Float3",
    "Float4",
    "Color4",
    "Int",
    "Int2",
    "Ramp",
    "Angle",
    "Angle2",
    "Angle3",
    "Angle4",
    "Enum",
    "Structure",
    "FilenameRead",
    "FilenameWrite",
    "ForceEvaluate",
    "Bool",
    "Ramp4",
    "Camera",
    "Any",
};

const char* GetParameterTypeName(ConTypes paramType)
{
    return parameterNames[std::min(int(paramType), int(Con_Any) - 1)];
}

ConTypes GetParameterType(const char* parameterName)
{
    for (size_t i = 0; i < Con_Any; i++)
    {
        if (!_stricmp(parameterNames[i], parameterName))
            return ConTypes(i);
    }
    return Con_Any;
}

size_t GetCurveCountPerParameterType(uint32_t paramType)
{
    switch (paramType)
    {
    case Con_Angle:
    case Con_Float:
        return 1;
    case Con_Angle2:
    case Con_Float2:
        return 2;
    case Con_Angle3:
    case Con_Float3:
        return 3;
    case Con_Angle4:
    case Con_Color4:
    case Con_Float4:
        return 4;
    case Con_Ramp:
        return 0;// sizeof(float) * 2 * 8;
    case Con_Ramp4:
        return 0;// sizeof(float) * 4 * 8;
    case Con_Enum:
        return 1;
    case Con_Int:
        return 1;
    case Con_Int2:
        return 2;
    case Con_FilenameRead:
    case Con_FilenameWrite:
        return 0;
    case Con_ForceEvaluate:
        return 0;
    case Con_Bool:
        return 1;
    case Con_Camera:
        return 7;
    default:
        assert(0);
    }
    return 0;
}

const char* GetCurveParameterSuffix(uint32_t paramType, int suffixIndex)
{
    static const char* suffixes[] = { ".x",".y",".z",".w" };
    static const char* cameraSuffixes[] = { "posX","posY","posZ", "dirX", "dirY", "dirZ", "FOV" };
    switch (paramType)
    {
    case Con_Angle:
    case Con_Float:
        return "";
    case Con_Angle2:
    case Con_Float2:
        return suffixes[suffixIndex];
    case Con_Angle3:
    case Con_Float3:
        return suffixes[suffixIndex];
    case Con_Angle4:
    case Con_Color4:
    case Con_Float4:
        return suffixes[suffixIndex];
    case Con_Ramp:
        return 0;// sizeof(float) * 2 * 8;
    case Con_Ramp4:
        return 0;// sizeof(float) * 4 * 8;
    case Con_Enum:
        return "";
    case Con_Int:
        return "";
    case Con_Int2:
        return suffixes[suffixIndex];
    case Con_FilenameRead:
    case Con_FilenameWrite:
        return 0;
    case Con_ForceEvaluate:
        return 0;
    case Con_Bool:
        return "";
    case Con_Camera:
        return cameraSuffixes[suffixIndex];
    default:
        assert(0);
    }
    return "";
}

uint32_t GetCurveParameterColor(uint32_t paramType, int suffixIndex)
{
    static const uint32_t colors[] = { 0xFF1010F0, 0xFF10F010, 0xFFF01010, 0xFFF0F0F0 };
    static const uint32_t cameraColors[] = { 0xFF1010F0, 0xFF10F010, 0xFFF01010, 0xFF1010F0, 0xFF10F010, 0xFFF01010, 0xFFF0F0F0 };
    switch (paramType)
    {
    case Con_Angle:
    case Con_Float:
        return 0xFF1040F0;
    case Con_Angle2:
    case Con_Float2:
        return colors[suffixIndex];
    case Con_Angle3:
    case Con_Float3:
        return colors[suffixIndex];
    case Con_Angle4:
    case Con_Color4:
    case Con_Float4:
        return colors[suffixIndex];
    case Con_Ramp:
        return 0xFFAAAAAA;// sizeof(float) * 2 * 8;
    case Con_Ramp4:
        return 0xFFAAAAAA;// sizeof(float) * 4 * 8;
    case Con_Enum:
        return 0xFFAAAAAA;
    case Con_Int:
        return 0xFFAAAAAA;
    case Con_Int2:
        return colors[suffixIndex];
    case Con_FilenameRead:
    case Con_FilenameWrite:
        return 0;
    case Con_ForceEvaluate:
        return 0;
    case Con_Bool:
        return 0xFFF0F0F0;
    case Con_Camera:
        return cameraColors[suffixIndex];
    default:
        assert(0);
    }
    return 0xFFAAAAAA;
}

size_t GetParameterOffset(uint32_t type, uint32_t parameterIndex)
{
    const MetaNode& currentMeta = gMetaNodes[type];
    size_t ret = 0;
    int i = 0;
    for (const MetaParameter& param : currentMeta.mParams)
    {
        if (i == parameterIndex)
            break;
        ret += GetParameterTypeSize(param.mType);
        i++;
    }
    return ret;
}

AnimationBase *AllocateAnimation(uint32_t valueType)
{
    switch (valueType)
    {
    case Con_Float: return new Animation<float>;
    case Con_Float2: return new Animation<Vec2>;
    case Con_Float3: return new Animation<Vec3>;
    case Con_Float4: return new Animation<Vec4>;
    case Con_Color4: return new Animation<Vec4>;
    case Con_Int: return new Animation<int>;
    case Con_Int2: return new Animation<iVec2>;
    case Con_Ramp: return new Animation<Vec2>;
    case Con_Angle: return new Animation<float>;
    case Con_Angle2: return new Animation<Vec2>;
    case Con_Angle3: return new Animation<Vec3>;
    case Con_Angle4: return new Animation<Vec4>;
    case Con_Enum: return new Animation<int>;
    case Con_Structure:
    case Con_FilenameRead:
    case Con_FilenameWrite:
    case Con_ForceEvaluate:
        return NULL;
    case Con_Bool: return new Animation<unsigned char>;
    case Con_Ramp4: return new Animation<Vec4>;
    case Con_Camera: return new Animation<Camera>;
    }
    return NULL;
}

CurveType GetCurveTypeForParameterType(ConTypes paramType)
{
    switch (paramType)
    {
    case Con_Float: return CurveSmooth;
    case Con_Float2: return CurveSmooth;
    case Con_Float3: return CurveSmooth;
    case Con_Float4: return CurveSmooth;
    case Con_Color4: return CurveSmooth;
    case Con_Int: return CurveLinear;
    case Con_Int2: return CurveLinear;
    case Con_Ramp: return CurveNone;
    case Con_Angle: return CurveSmooth;
    case Con_Angle2: return CurveSmooth;
    case Con_Angle3: return CurveSmooth;
    case Con_Angle4: return CurveSmooth;
    case Con_Enum: return CurveDiscrete;
    case Con_Structure:
    case Con_FilenameRead:
    case Con_FilenameWrite:
    case Con_ForceEvaluate:
        return CurveNone;
    case Con_Bool: return CurveDiscrete;
    case Con_Ramp4: return CurveNone;
    case Con_Camera: return CurveSmooth;
    }
    return CurveNone;
}

void Camera::ComputeViewProjectionMatrix(float *viewProj, float *viewInverse)
{
    Mat4x4& vp = *(Mat4x4*)viewProj;
    Mat4x4 view, proj;
    view.lookAtRH(mPosition, mPosition + mDirection, Vec4(0.f, 1.f, 0.f, 0.f));
    proj.glhPerspectivef2(53.f, 1.f, 0.01f, 100.f);
    vp = view * proj;

    Mat4x4& vi = *(Mat4x4*)viewInverse;
    vi.LookAt(mPosition, mPosition + mDirection, Vec4(0.f, 1.f, 0.f, 0.f));
}