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
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include "EvaluationStages.h"

struct EvaluationContext
{
    EvaluationContext(EvaluationStages& evaluation, bool synchronousEvaluation, int defaultWidth, int defaultHeight);
    ~EvaluationContext();

    void RunAll();
    // return true if any node is in processing state
    bool RunBackward(size_t nodeIndex);
    void RunSingle(size_t nodeIndex, EvaluationInfo& evaluationInfo);
    void RunDirty();

    unsigned int GetEvaluationTexture(size_t target);
    std::shared_ptr<RenderTarget> GetRenderTarget(size_t target)
    { 
        if (target >= mStageTarget.size())
            return NULL;
        return mStageTarget[target]; 
    }

    FFMPEGCodec::Encoder *GetEncoder(const std::string &filename, int width, int height);
    bool IsSynchronous() const { return mbSynchronousEvaluation; }
    void SetTargetDirty(size_t target, bool onlyChild = false);
    int StageIsProcessing(size_t target) const { if (target >= mbProcessing.size()) return 0; return mbProcessing[target]; }
    float StageGetProgress(size_t target) const { if (target >= mProgress.size()) return 0.f; return mProgress[target]; }
    void StageSetProcessing(size_t target, int processing);
    void StageSetProgress(size_t target, float progress);

    void AllocRenderTargetsForEditingPreview();

    void AllocateComputeBuffer(int target, int elementCount, int elementSize);
    // edit context only
    void UserAddStage();
    void UserDeleteStage(size_t index);

    struct ComputeBuffer
    {
        unsigned int mBuffer{ 0 };
        unsigned int mElementCount;
        unsigned int mElementSize;
    };

    const ComputeBuffer* GetComputeBuffer(size_t index) const;
    void Clear();

    unsigned int GetMaterialUniqueId() const { return mRuntimeUniqueId; }
    void SetMaterialUniqueId(unsigned int uniqueId) { mRuntimeUniqueId = uniqueId; }


    EvaluationStages& mEvaluationStages;
    FullScreenTriangle mFSQuad;

protected:
    void PreRun();
    void EvaluateGLSL(const EvaluationStage& evaluationStage, size_t index, EvaluationInfo& evaluationInfo);
    void EvaluateC(const EvaluationStage& evaluationStage, size_t index, EvaluationInfo& evaluationInfo);
    void EvaluatePython(const EvaluationStage& evaluationStage, size_t index, EvaluationInfo& evaluationInfo);
    void EvaluateGLSLCompute(const EvaluationStage& evaluationStage, size_t index, EvaluationInfo& evaluationInfo);
    // return true if any node is still in processing state
    bool RunNodeList(const std::vector<size_t>& nodesToEvaluate);
    void RunNode(size_t nodeIndex);

    void RecurseBackward(size_t target, std::vector<size_t>& usedNodes);

    void BindTextures(const EvaluationStage& evaluationStage, unsigned int program, std::shared_ptr<RenderTarget> reusableTarget);
    void AllocRenderTargetsForBaking(const std::vector<size_t>& nodesToEvaluate);

    

    int GetBindedComputeBuffer(const EvaluationStage& evaluationStage) const;

    std::vector<std::shared_ptr<RenderTarget> > mStageTarget; // 1 per stage
    std::vector<ComputeBuffer> mComputeBuffers;
    std::map<std::string, FFMPEGCodec::Encoder*> mWriteStreams;
    std::vector<bool> mbDirty;
    std::vector<int> mbProcessing;
    std::vector<float> mProgress;
    EvaluationInfo mEvaluationInfo;

    std::vector<int> mStillDirty;
    int mDefaultWidth;
    int mDefaultHeight;
    bool mbSynchronousEvaluation;
    unsigned int mRuntimeUniqueId; // material unique Id for thumbnail update
};

struct Builder
{
    Builder();
    ~Builder();

    void Add(const char* graphName, EvaluationStages& stages);

    struct BuildInfo
    {
        std::string mName;
        float mProgress;
    };

    // return true if buildInfo has been updated
    bool UpdateBuildInfo(std::vector<BuildInfo>& buildInfo);
private:
    std::mutex mMutex;
    std::thread mThread;

    std::atomic_bool mbRunning;

    struct Entry
    {
        std::string mName;
        float mProgress;
        EvaluationStages mEvaluationStages;
    };
    std::vector<Entry> mEntries;
    void BuildEntries();
    void DoBuild(Entry& entry);
};

namespace DrawUICallbacks
{
    void DrawUICubemap(EvaluationContext *context, size_t nodeIndex);
    void DrawUISingle(EvaluationContext *context, size_t nodeIndex);
    void DrawUIProgress(EvaluationContext *context, size_t nodeIndex);
}