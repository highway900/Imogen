// https://github.com/CedricGuillemet/Imogen
//
// The MIT License(MIT)
// 
// Copyright(c) 2019 Cedric Guillemet
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
#include <vector>
#include <stdint.h>
#include <string>
#include "imgui.h"
#include "imgui_internal.h"

struct NodeGraphControlerBase
{
    NodeGraphControlerBase() : mSelectedNodeIndex(-1), mCategoriesCount(0), mCategories(0)
    {}

    int mSelectedNodeIndex;
    int mCategoriesCount;
    const char ** mCategories;

    virtual void UpdateEvaluationList(const std::vector<size_t> nodeOrderList) = 0;
    virtual void AddLink(int InputIdx, int InputSlot, int OutputIdx, int OutputSlot) = 0;
    virtual void DelLink(int index, int slot) = 0;
    virtual unsigned int GetNodeTexture(size_t index) = 0;
    // A new node has been added in the graph. Do a push_back on your node array
    // add node for batch(loading graph)
    virtual void AddSingleNode(size_t type) = 0;
    // add  by user interface
    virtual void UserAddNode(size_t type) = 0;
    // node deleted
    virtual void UserDeleteNode(size_t index) = 0;
    virtual ImVec2 GetEvaluationSize(size_t index) = 0;

    virtual void SetParamBlock(size_t index, const std::vector<unsigned char>& paramBlock) = 0;
    virtual void SetTimeSlot(size_t index, int frameStart, int frameEnd) = 0;
    virtual bool NodeHasUI(size_t nodeIndex) = 0;
    virtual int NodeIsProcesing(size_t nodeIndex) = 0;
    virtual float NodeProgress(size_t nodeIndex) = 0;
    virtual bool NodeIsCubemap(size_t nodeIndex) = 0;
    virtual bool NodeIs2D(size_t nodeIndex) = 0;
    virtual bool NodeIsCompute(size_t nodeIndex) = 0;
    virtual void DrawNodeImage(ImDrawList *drawList, const ImRect &rc, const ImVec2 marge, const size_t nodeIndex) = 0;

    // clipboard
    virtual void CopyNodes(const std::vector<size_t> nodes) = 0;
    virtual void CutNodes(const std::vector<size_t> nodes) = 0;
    virtual void PasteNodes() = 0;
};

struct Node
{
    int     mType;
    ImVec2  Pos, Size;
    size_t InputsCount, OutputsCount;
    bool mbSelected;
    Node() : mbSelected(false) {}
    Node(int type, const ImVec2& pos);

    ImVec2 GetInputSlotPos(int slot_no, float factor) const { return ImVec2(Pos.x*factor, Pos.y*factor + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
    ImVec2 GetOutputSlotPos(int slot_no, float factor) const { return ImVec2(Pos.x*factor + Size.x, Pos.y*factor + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
    ImRect GetNodeRect(float factor) { return ImRect(Pos * factor, Pos * factor + Size); }

    bool operator != (const Node& other) const
    {
        if (mType != other.mType)
            return true;
        if (InputsCount != other.InputsCount)
            return true;
        if (OutputsCount != other.OutputsCount)
            return true;
        if (Pos.x != other.Pos.x)
            return true;
        if (Pos.y != other.Pos.y)
            return true;

        if (Size.x != other.Size.x)
            return true;
        if (Size.y != other.Size.y)
            return true;

        return false;
    }
};

struct NodeLink
{
    int     InputIdx, InputSlot, OutputIdx, OutputSlot;
    NodeLink() {}
    NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
    bool operator == (const NodeLink& other) const
    {
        return InputIdx == other.InputIdx && InputSlot == other.InputSlot && OutputIdx == other.OutputIdx && OutputSlot == other.OutputSlot;
    }
};

inline bool operator != (const ImVec2 r1, const ImVec2 r2)
{
    if (r1.x != r2.x)
        return true;
    if (r1.y != r2.y)
        return true;
    return false;
}

struct NodeRug
{
    ImVec2 mPos, mSize;
    uint32_t mColor;
    std::string mText;

    bool operator != (const NodeRug& other) const
    {
        if (mPos != other.mPos)
            return true;
        if (mSize != other.mSize)
            return false;
        if (mColor != other.mColor)
            return true;
        if (mText != other.mText)
            return true;
        return false;
    }
};

void NodeGraph(NodeGraphControlerBase *delegate, bool enabled);
void NodeGraphClear(); // delegate is not called
const std::vector<NodeLink>& NodeGraphGetLinks();
const std::vector<NodeRug>& NodeGraphRugs();
ImVec2 NodeGraphGetNodePos(size_t index);

void NodeGraphAddNode(NodeGraphControlerBase *delegate, int type, const std::vector<unsigned char>& parameters, int posx, int posy, int frameStart, int frameEnd);
void NodeGraphAddRug(int32_t posX, int32_t posY, int32_t sizeX, int32_t sizeY, uint32_t color, const std::string comment);
void NodeGraphAddLink(NodeGraphControlerBase *delegate, int InputIdx, int InputSlot, int OutputIdx, int OutputSlot);
void NodeGraphUpdateEvaluationOrder(NodeGraphControlerBase *delegate);
void NodeGraphUpdateScrolling();
void NodeGraphSelectNode(int selectedNodeIndex);
void NodeGraphLayout();