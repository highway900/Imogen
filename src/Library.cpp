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

#include <algorithm>
#include <iostream>
#include <fstream>
#include "Library.h"
#include "imgui.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

int Log(const char *szFormat, ...);

enum : uint32_t
{
    v_initial,
    v_materialComment,
    v_thumbnail,
    v_nodeImage,
    v_rugs,
    v_nodeTypeName,
    v_frameStartEnd,
    v_animation,
    v_pinnedParameters,
    v_lastVersion
};
#define ADD(_fieldAdded, _fieldName) if (dataVersion >= _fieldAdded){ Ser(_fieldName); }
#define ADD_LOCAL(_localAdded, _type, _localName, _defaultValue) \
    _type _localName = (_defaultValue); \
    if (dataVersion >= (_localAdded)) { Ser(_localName)); }
#define REM(_fieldAdded, _fieldRemoved, _type, _fieldName, _defaultValue) \
    _type _fieldName = (_defaultValue); \
    if (dataVersion >= (_fieldAdded) && dataVersion < (_fieldRemoved)) { Ser(_fieldName); }
#define VERSION_IN_RANGE(_from, _to) \
    (dataVersion >= (_from) && dataVersion < (_to))

template<bool doWrite> struct Serialize
{
    Serialize(const char *szFilename)
    {
        fp = fopen(szFilename, doWrite ? "wb" : "rb");
    }

    ~Serialize()
    {
        if (fp)
            fclose(fp);
    }

    template<typename T> void Ser(T& data)
    {
        if (doWrite)
            fwrite(&data, sizeof(T), 1, fp);
        else
            fread(&data, sizeof(T), 1, fp);
    }

    void Ser(std::string& data)
    {
        if (doWrite)
        {
            uint32_t len = uint32_t(strlen(data.c_str()));// uint32_t(data.length());
            fwrite(&len, sizeof(uint32_t), 1, fp);
            fwrite(data.c_str(), len, 1, fp);
        }
        else
        {
            uint32_t len;
            fread(&len, sizeof(uint32_t), 1, fp);
            data.resize(len);
            fread(&data[0], len, 1, fp);
            data = std::string(data.c_str(), strlen(data.c_str()));
        }
    }

    template<typename T> void Ser(std::vector<T>& data)
    {
        uint32_t count = uint32_t(data.size());
        Ser(count);
        data.resize(count);
        for (auto& item : data)
            Ser(&item);
    }

    template<typename T> void SerArray(std::vector<T>& data)
    {
        uint32_t count = uint32_t(data.size());
        Ser(count);
        if (!count)
            return;
        if (doWrite)
        {
            fwrite(data.data(), count*sizeof(T), 1, fp);
        }
        else
        {
            data.resize(count);
            fread(&data[0], count * sizeof(T), 1, fp);
        }
    }

    void Ser(std::vector<uint8_t>& data)
    {
        SerArray(data);
    }

    void Ser(std::vector<uint32_t>& data)
    {
        SerArray(data);
    }

    void Ser(std::vector<int32_t>& data)
    {
        SerArray(data);
    }

    void Ser(AnimationBase *animBase)
    {
        ADD(v_animation, animBase->mFrames);
        if (doWrite)
        {
            fwrite(animBase->GetData(), animBase->GetValuesByteLength(), 1, fp);
        }
        else
        {
            animBase->Allocate(animBase->mFrames.size());
            fread(animBase->GetData(), animBase->GetValuesByteLength(), 1, fp);
        }
    }

    void Ser(AnimTrack *animTrack)
    {
        ADD(v_animation, animTrack->mNodeIndex);
        ADD(v_animation, animTrack->mParamIndex);
        ADD(v_animation, animTrack->mValueType);
        if (!doWrite)
        {
            animTrack->mAnimation = AllocateAnimation(animTrack->mValueType);
        }
        ADD(v_animation, animTrack->mAnimation);
    }

    void Ser(InputSampler *inputSampler)
    {
        ADD(v_initial, inputSampler->mWrapU);
        ADD(v_initial, inputSampler->mWrapV);
        ADD(v_initial, inputSampler->mFilterMin);
        ADD(v_initial, inputSampler->mFilterMag);
    }

    void Ser(MaterialNode *materialNode)
    {
        ADD(v_initial, materialNode->mType);
        ADD(v_nodeTypeName, materialNode->mTypeName);
        ADD(v_initial, materialNode->mPosX);
        ADD(v_initial, materialNode->mPosY);
        ADD(v_initial, materialNode->mInputSamplers);
        ADD(v_initial, materialNode->mParameters);
        ADD(v_nodeImage, materialNode->mImage);
        ADD(v_frameStartEnd, materialNode->mFrameStart);
        ADD(v_frameStartEnd, materialNode->mFrameEnd);
    }

    void Ser(MaterialNodeRug *materialNodeRug)
    {
        ADD(v_rugs, materialNodeRug->mPosX);
        ADD(v_rugs, materialNodeRug->mPosY);
        ADD(v_rugs, materialNodeRug->mSizeX);
        ADD(v_rugs, materialNodeRug->mSizeY);
        ADD(v_rugs, materialNodeRug->mColor);
        ADD(v_rugs, materialNodeRug->mComment);
    }

    void Ser(MaterialConnection *materialConnection)
    {
        ADD(v_initial, materialConnection->mInputNode);
        ADD(v_initial, materialConnection->mOutputNode);
        ADD(v_initial, materialConnection->mInputSlot);
        ADD(v_initial, materialConnection->mOutputSlot);
    }

    void Ser(Material *material)
    {
        ADD(v_initial, material->mName);
        REM(v_materialComment, v_rugs, std::string, (material->mComment), "");
        ADD(v_initial, material->mMaterialNodes);
        ADD(v_initial, material->mMaterialConnections);
        ADD(v_thumbnail, material->mThumbnail);
        ADD(v_rugs, material->mMaterialRugs);
        ADD(v_animation, material->mAnimTrack);
        ADD(v_animation, material->mFrameMin);
        ADD(v_animation, material->mFrameMax);
        ADD(v_pinnedParameters, material->mPinnedParameters);
    }

    bool Ser(Library *library)
    {
        if (!fp)
            return false;
        if (doWrite)
            dataVersion = v_lastVersion-1;
        Ser(dataVersion);
        if (dataVersion > v_lastVersion)
            return false; // no forward compatibility
        ADD(v_initial, library->mMaterials);
        return true;
    }

    FILE *fp;
    uint32_t dataVersion;
};

typedef Serialize<true> SerializeWrite;
typedef Serialize<false> SerializeRead;

void LoadLib(Library *library, const char *szFilename)
{
    SerializeRead loadSer(szFilename);
    loadSer.Ser(library);

    for (auto& material : library->mMaterials)
    {
        material.mThumbnailTextureId = 0;
        material.mRuntimeUniqueId = GetRuntimeId();
        for (auto& node : material.mMaterialNodes)
        {
            node.mRuntimeUniqueId = GetRuntimeId();
            if (loadSer.dataVersion >= v_nodeTypeName)
            {
                node.mType = uint32_t(GetMetaNodeIndex(node.mTypeName));
            }
        }
    }
}

void SaveLib(Library *library, const char *szFilename)
{
    SerializeWrite(szFilename).Ser(library);
}

unsigned int GetRuntimeId()
{
    static unsigned int runtimeId = 0;
    return ++runtimeId;
}


std::vector<MetaNode> gMetaNodes;
std::map<std::string, size_t> gMetaNodesIndices;

size_t GetMetaNodeIndex(const std::string& metaNodeName)
{
    auto iter = gMetaNodesIndices.find(metaNodeName.c_str());
    if (iter == gMetaNodesIndices.end())
    {
        Log("Node type %s not find in the library!\n", metaNodeName.c_str());
        return -1;
    }
    return iter->second;
}

void LoadMetaNodes(const std::vector<std::string>& metaNodeFilenames)
{
    static const uint32_t hcTransform = IM_COL32(200, 200, 200, 255);
    static const uint32_t hcGenerator = IM_COL32(150, 200, 150, 255);
    static const uint32_t hcMaterial = IM_COL32(150, 150, 200, 255);
    static const uint32_t hcBlend = IM_COL32(200, 150, 150, 255);
    static const uint32_t hcFilter = IM_COL32(200, 200, 150, 255);
    static const uint32_t hcNoise = IM_COL32(150, 250, 150, 255);
    static const uint32_t hcPaint = IM_COL32(100, 250, 180, 255);

    for (auto& filename : metaNodeFilenames)
    {
        std::vector<MetaNode> metaNodes = ReadMetaNodes(filename.c_str());
        if (metaNodes.empty())
        {
            IMessageBox("Errors while parsing nodes definitions.\nCheck logs.", "Node Parsing Error!");
            exit(-1);
        }
        gMetaNodes.insert(gMetaNodes.end(), metaNodes.begin(), metaNodes.end());
    }


    for (size_t i = 0; i < gMetaNodes.size(); i++)
    {
        gMetaNodesIndices[gMetaNodes[i].mName] = i;
    }
}

void LoadMetaNodes()
{
    std::vector<std::string> metaNodeFilenames;
    DiscoverFiles("json", "Nodes/", metaNodeFilenames);
    LoadMetaNodes(metaNodeFilenames);
}

void SaveMetaNodes(const char* filename)
{
    // write lib to json -----------------------
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    rapidjson::Value nodelist(rapidjson::kArrayType);

    for (auto &node : gMetaNodes)
    {
        rapidjson::Value nodeValue;
        nodeValue.SetObject();
        nodeValue.AddMember("name", rapidjson::Value(node.mName.c_str(), allocator), allocator);
        nodeValue.AddMember("category", rapidjson::Value().SetInt(node.mCategory), allocator);
        {
            ImColor clr(node.mHeaderColor);
            rapidjson::Value colorValue(rapidjson::kArrayType);
            colorValue.PushBack(rapidjson::Value().SetFloat(clr.Value.x), allocator);
            colorValue.PushBack(rapidjson::Value().SetFloat(clr.Value.y), allocator);
            colorValue.PushBack(rapidjson::Value().SetFloat(clr.Value.z), allocator);
            colorValue.PushBack(rapidjson::Value().SetFloat(clr.Value.w), allocator);
            nodeValue.AddMember("color", colorValue, allocator);
        }

        std::vector<MetaCon>* cons[] = { &node.mInputs, &node.mOutputs };
        for (int i = 0; i < 2; i++)
        {
            auto inouts = cons[i];
            if (inouts->empty())
                continue;

            rapidjson::Value ioValue(rapidjson::kArrayType);

            for (auto &con : *inouts)
            {
                rapidjson::Value conValue;
                conValue.SetObject();
                conValue.AddMember("name", rapidjson::Value(con.mName.c_str(), allocator), allocator);
                conValue.AddMember("type", rapidjson::Value(GetParameterTypeName(ConTypes(con.mType)), allocator), allocator);
                ioValue.PushBack(conValue, allocator);
            }
            if (i)
                nodeValue.AddMember("outputs", ioValue, allocator);
            else
                nodeValue.AddMember("inputs", ioValue, allocator);
        }

        if (!node.mParams.empty())
        {
            rapidjson::Value paramValues(rapidjson::kArrayType);
            for (auto& con : node.mParams)
            {
                rapidjson::Value conValue;
                conValue.SetObject();
                conValue.AddMember("name", rapidjson::Value(con.mName.c_str(), allocator), allocator);
                conValue.AddMember("type", rapidjson::Value(GetParameterTypeName(con.mType), allocator), allocator);
                if (con.mRangeMinX > FLT_EPSILON || con.mRangeMaxX > FLT_EPSILON || con.mRangeMinY > FLT_EPSILON || con.mRangeMaxY > FLT_EPSILON)
                {
                    conValue.AddMember("rangeMinX", rapidjson::Value().SetFloat(con.mRangeMinX), allocator);
                    conValue.AddMember("rangeMaxX", rapidjson::Value().SetFloat(con.mRangeMaxX), allocator);
                    conValue.AddMember("rangeMinY", rapidjson::Value().SetFloat(con.mRangeMinY), allocator);
                    conValue.AddMember("rangeMaxY", rapidjson::Value().SetFloat(con.mRangeMaxY), allocator);
                }
                if (con.mbRelative)
                    conValue.AddMember("relative", rapidjson::Value().SetBool(con.mbRelative), allocator);
                if (con.mbQuadSelect)
                    conValue.AddMember("quadSelect", rapidjson::Value().SetBool(con.mbQuadSelect), allocator);
                if (con.mEnumList.size())
                {
                    conValue.AddMember("enum", rapidjson::Value(con.mEnumList.c_str(), allocator), allocator);
                }
                paramValues.PushBack(conValue, allocator);
            }
            nodeValue.AddMember("parameters", paramValues, allocator);
        }
        if (node.mbHasUI)
            nodeValue.AddMember("hasUI", rapidjson::Value().SetBool(node.mbHasUI), allocator);
        if (node.mbSaveTexture)
            nodeValue.AddMember("saveTexture", rapidjson::Value().SetBool(node.mbSaveTexture), allocator);

        nodelist.PushBack(nodeValue, allocator);
    }

    d.AddMember("nodes", nodelist, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    std::ofstream myfile;
    myfile.open(filename);
    myfile << buffer.GetString();
    myfile.close();
}

std::vector<MetaNode> ReadMetaNodes(const char *filename)
{
    // read it back
    std::vector<MetaNode> serNodes;
    std::ifstream t(filename);
    if (!t.good())
    {
        Log("%s - Unable to load file.\n", filename);
        return serNodes;
    }
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    rapidjson::Document doc;
    doc.Parse(str.c_str());
    if (doc.HasParseError())
    {
        Log("Parsing error in %s\n", filename);
        return serNodes;
    }
    rapidjson::Value &nodesValue = doc["nodes"];
    for (rapidjson::SizeType i = 0; i < nodesValue.Size(); i++)
    {
        MetaNode curNode;
        rapidjson::Value& node = nodesValue[i];
        if (!node.HasMember("name"))
        {
            //error
            Log("Missing name in node %d definition (%s)\n", i, filename);
            return serNodes;
        }
        curNode.mName = node["name"].GetString();
        if (!node.HasMember("category"))
        {
            //error
            Log("Missing category in node %s definition (%s)\n", curNode.mName.c_str(), filename);
            return serNodes;
        }
        curNode.mCategory = node["category"].GetInt();

        if (node.HasMember("hasUI"))
            curNode.mbHasUI = node["hasUI"].GetBool();
        else
            curNode.mbHasUI = false;
        if (node.HasMember("saveTexture"))
            curNode.mbSaveTexture = node["saveTexture"].GetBool();
        else
            curNode.mbSaveTexture = false;
            
        if (!node.HasMember("color"))
        {
            //error
            Log("Missing color in node %s definition (%s)\n", curNode.mName.c_str(), filename);
            return serNodes;

        }
        rapidjson::Value &color = node["color"];
        float c[4];
        if (color.Size() != 4)
        {
            //error
            Log("wrong color component count in node %s definition (%s)\n", curNode.mName.c_str(), filename);
            return serNodes;
        }
        for (rapidjson::SizeType i = 0; i < color.Size(); i++)
        {
            c[i] = color[i].GetFloat();
        }
        curNode.mHeaderColor = ImColor(c[0], c[1], c[2], c[3]).operator ImU32();
        // inputs/outputs
        if (node.HasMember("inputs"))
        {
            rapidjson::Value &inputs = node["inputs"];
            for (rapidjson::SizeType i = 0; i < inputs.Size(); i++)
            {
                MetaCon metaNode;
                if (!inputs[i].HasMember("name") || !inputs[i].HasMember("type"))
                {
                    //error
                    Log("Missing name or type in inputs for node %s definition (%s)\n", curNode.mName.c_str(), filename);
                    return serNodes;
                }

                metaNode.mName = inputs[i]["name"].GetString();
                metaNode.mType = GetParameterType(inputs[i]["type"].GetString());
                if (metaNode.mType == Con_Any)
                {
                    //error
                    Log("Wrong type for %s in inputs for node %s definition (%s)\n", metaNode.mName.c_str(), curNode.mName.c_str(), filename);
                    return serNodes;
                }
                curNode.mInputs.emplace_back(metaNode);
            }
        }
        if (node.HasMember("outputs"))
        {
            rapidjson::Value &outputs = node["outputs"];
            for (rapidjson::SizeType i = 0; i < outputs.Size(); i++)
            {
                MetaCon metaNode;
                if (!outputs[i].HasMember("name") || !outputs[i].HasMember("type"))
                {
                    //error
                    Log("Missing name or type in outputs for node %s definition (%s)\n", curNode.mName.c_str(), filename);
                    return serNodes;
                }
                metaNode.mName = outputs[i]["name"].GetString();
                metaNode.mType = GetParameterType(outputs[i]["type"].GetString());
                if (metaNode.mType == Con_Any)
                {
                    //error
                    Log("Wrong type for %s in outputs for node %s definition (%s)\n", metaNode.mName.c_str(), curNode.mName.c_str(), filename);
                    return serNodes;
                }
                curNode.mOutputs.emplace_back(metaNode);
            }
        }
        //parameters
        if (node.HasMember("parameters"))
        {
            rapidjson::Value &params = node["parameters"];
            for (rapidjson::SizeType i = 0; i < params.Size(); i++)
            {
                MetaParameter metaParam;
                rapidjson::Value &param = params[i];
                if (!param.HasMember("name") || !param.HasMember("type"))
                {
                    //error
                    Log("Missing name or type in parameters for node %s definition (%s)\n", curNode.mName.c_str(), filename);
                    return serNodes;
                }
                metaParam.mName = param["name"].GetString();
                metaParam.mType = GetParameterType(param["type"].GetString());
                if (metaParam.mType == Con_Any)
                {
                    //error
                    Log("Wrong type for %s in parameters for node %s definition (%s)\n", metaParam.mName.c_str(), curNode.mName.c_str(), filename);
                    return serNodes;
                }

                if (param.HasMember("rangeMinX") && param.HasMember("rangeMaxX") && param.HasMember("rangeMinY") && param.HasMember("rangeMaxY"))
                {
                    metaParam.mRangeMinX = param["rangeMinX"].GetFloat();
                    metaParam.mRangeMaxX = param["rangeMaxX"].GetFloat();
                    metaParam.mRangeMinY = param["rangeMinY"].GetFloat();
                    metaParam.mRangeMaxY = param["rangeMaxY"].GetFloat();
                }
                else
                {
                    metaParam.mRangeMinX = metaParam.mRangeMinY = metaParam.mRangeMaxX = metaParam.mRangeMaxY = 0.f;
                }
                if (param.HasMember("loop"))
                    metaParam.mbLoop = param["loop"].GetBool();
                else
                    metaParam.mbLoop = true;
                if (param.HasMember("relative"))
                    metaParam.mbRelative = param["relative"].GetBool();
                else
                    metaParam.mbRelative = false;
                if (param.HasMember("quadSelect"))
                    metaParam.mbQuadSelect = param["quadSelect"].GetBool();
                else
                    metaParam.mbQuadSelect = false;
                if (param.HasMember("enum"))
                {
                    if (metaParam.mType != Con_Enum)
                    {
                        //error
                        Log("Mismatch type for enumerator in parameter %s for node %s definition (%s)\n", metaParam.mName.c_str(), curNode.mName.c_str(), filename);
                        return serNodes;
                    }
                    std::string enumStr = param["enum"].GetString();
                    if (enumStr.size())
                    {
                            metaParam.mEnumList = enumStr;
                            if (metaParam.mEnumList.back() != '|')
                                metaParam.mEnumList += '|';
                    }
                    else
                    {
                        //error
                        Log("Empty string for enumerator in parameter %s for node %s definition (%s)\n", metaParam.mName.c_str(), curNode.mName.c_str(), filename);
                        return serNodes;
                    }
                }
                curNode.mParams.emplace_back(metaParam);
            }
        }

        serNodes.emplace_back(curNode);
    }
    return serNodes;
}

AnimationBase::AnimationPointer AnimationBase::GetPointer(int32_t frame, bool bSetting) const
{
    if (mFrames.empty())
        return { bSetting ? -1 : 0, 0, 0, 0, 0.f };
    if (frame <= mFrames[0])
        return { bSetting ? -1 : 0, 0, 0, 0, 0.f };
    if (frame > mFrames.back())
    {
        int32_t last = int32_t(mFrames.size() - (bSetting ? 0 : 1));
        return {last, mFrames.back(), last, mFrames.back(), 0.f };
    }
    for (int i = 0; i < int(mFrames.size()) - 1; i++)
    {
        if (mFrames[i] <= frame && mFrames[i + 1] >= frame)
        {
            float ratio = float(frame - mFrames[i]) / float(mFrames[i+1] - mFrames[i]);
            return { i, mFrames[i], i + 1, mFrames[i+1], ratio };
        }
    }
    assert(0);
    return { bSetting? -1:0, 0, 0, 0, 0.f };
}

AnimTrack& AnimTrack::operator = (const AnimTrack& other)
{
    mNodeIndex = other.mNodeIndex;
    mParamIndex = other.mParamIndex;
    mValueType = other.mValueType;
    delete mAnimation;
    mAnimation = AllocateAnimation(mValueType);
    mAnimation->Copy(other.mAnimation);
    return *this;
}