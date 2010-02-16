/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2008 Scientific Computing and Imaging Institute,
   University of Utah.


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

/**
  \file    AbstrRenderer.h
  \author  Jens Krueger
           SCI Institute
           University of Utah
  \version 1.0
  \date    August 2008
*/
#pragma once

#ifndef ABSTRRENDERER_H
#define ABSTRRENDERER_H

#include "StdTuvokDefines.h"
#include <string>
#ifdef DETECTED_OS_WINDOWS
# include <memory>
#else
# include <tr1/memory>
#endif

#include "../StdTuvokDefines.h"
#include "../Renderer/CullingLOD.h"
#include "../Renderer/RenderRegion.h"
#include "../IO/Dataset.h"
#include "../Basics/Plane.h"
#include "../Basics/GeometryGenerator.h"

class TransferFunction1D;
class TransferFunction2D;

namespace tuvok {

class MasterController;

class Brick {
public:
  Brick() :
    vCenter(0,0,0),
    vExtension(0,0,0),
    vCoords(0,0,0)
  {
  }

  Brick(UINT32 x, UINT32 y, UINT32 z,
        UINT32 iSizeX, UINT32 iSizeY, UINT32 iSizeZ,
        const BrickKey& k) :
    vCenter(0,0,0),
    vExtension(0,0,0),
    vVoxelCount(iSizeX, iSizeY, iSizeZ),
    vCoords(x,y,z),
    kBrick(k)
  {
  }

  FLOATVECTOR3 vCenter;
  FLOATVECTOR3 vTexcoordsMin;
  FLOATVECTOR3 vTexcoordsMax;
  FLOATVECTOR3 vExtension;
  UINTVECTOR3 vVoxelCount;
  UINTVECTOR3 vCoords;
  BrickKey kBrick;
  float fDistance;
};

inline bool operator < (const Brick& left, const Brick& right) {
  if  (left.fDistance < right.fDistance) return true;
  return false;
}

/** \class AbstrRenderer
 * Base for all renderers. */
class AbstrRenderer {
  public:

    enum ERendererType {
      RT_SBVR = 0,
      RT_RC,
      RT_INVALID
    };

    virtual ERendererType GetRendererType() {return RT_INVALID;}

    enum ERenderMode {
      RM_1DTRANS = 0,  /**< one dimensional transfer function */
      RM_2DTRANS,      /**< two dimensional transfer function */
      RM_ISOSURFACE,   /**< render isosurfaces                */
      RM_INVALID
    };
    ERenderMode GetRendermode() {return m_eRenderMode;}
    virtual void SetRendermode(ERenderMode eRenderMode);

    enum EBlendPrecision {
      BP_8BIT = 0,
      BP_16BIT,
      BP_32BIT,
      BP_INVALID
    };
    EBlendPrecision GetBlendPrecision() {return m_eBlendPrecision;}
    virtual void SetBlendPrecision(EBlendPrecision eBlendPrecision);

    bool GetUseLighting() const { return m_bUseLighting; }
    virtual void SetUseLighting(bool bUseLighting);

    enum ScalingMethod {
      SMETH_BIT_WIDTH=0,    ///< scaled based on DS and TF bit width
      SMETH_BIAS_AND_SCALE  ///< bias + scale factors of DS calculated
    };
    /** Default settings: 1D transfer function, single-image view, white text,
     * black BG.
     * @param pMasterController message router
     * @param bUseOnlyPowerOfTwo force power of two textures, for systems that
     *                           do not support npot textures.
     * @param bDownSampleTo8Bits force 8bit textures, for systems that do not
     *                           support 16bit textures (or 16bit linear
     *                           interpolation).
     * @param bDisableBorder     don't use OpenGL borders for textures.
     * @param smeth              method to scale values into TF range */
    AbstrRenderer(MasterController* pMasterController, bool bUseOnlyPowerOfTwo,
                  bool bDownSampleTo8Bits, bool bDisableBorder,
                  enum ScalingMethod smeth = SMETH_BIT_WIDTH);
    /** Deallocates dataset and transfer functions. */
    virtual ~AbstrRenderer();
    /** Sends a message to the master to ask for a dataset to be loaded.
     * The dataset is converted to UVF if it is not one already.
     * @param strFilename path to a file */
    virtual bool LoadDataset(const std::string& strFilename, bool& bRebrickingRequired);
    /** Query whether or not we should redraw the next frame, else we should
     * reuse what is already rendered or continue with the current frame if it
     * is not complete yet. */
    virtual bool CheckForRedraw();

    virtual void Paint() {
      if (renderRegions.empty()) {
        renderRegions.push_back(&simpleRenderRegion3D);
      }
      if (renderRegions.size() == 1 && renderRegions[0] == &simpleRenderRegion3D) {
        simpleRenderRegion3D.maxCoord = m_vWinSize; //minCoord is always (0,0)
      }

      // check if we are rendering a stereo frame
      m_bDoStereoRendering = m_bRequestStereoRendering &&
                             renderRegions.size() == 1 &&
                             renderRegions[0]->is3D();
    }


    virtual bool Initialize();

    /** Deallocates GPU memory allocated during the rendering process. */
    virtual void Cleanup() = 0;

    /// Sets the dataset from external source; only meant to be used by clients
    /// which don't want to use the LOD subsystem.
    void SetDataset(Dataset *vds);
/*    /// Modifies previously uploaded data.
    void UpdateData(const BrickKey&,
                    std::tr1::shared_ptr<float> fp, size_t len);
*/
    Dataset&       GetDataset()       { return *m_pDataset; }
    const Dataset& GetDataset() const { return *m_pDataset; }
    void ClearBricks() { m_pDataset->Clear(); }

    TransferFunction1D* Get1DTrans() {return m_p1DTrans;}
    TransferFunction2D* Get2DTrans() {return m_p2DTrans;}
    virtual void Set1DTrans(const std::vector<unsigned char>& rgba) = 0;

    /** Notify renderer that 1D TF has changed.  In most cases, this will cause
     * the renderer to start anew. */
    virtual void Changed1DTrans();
    /** Notify renderer that 2D TF has changed.  In most cases, this will cause
     * the renderer to start anew. */
    virtual void Changed2DTrans();

    /** Sets up a gradient background which fades vertically.
     * @param vColors[0] is the color at the bottom;
     * @param vColors[1] is the color at the top. */
    virtual bool SetBackgroundColors(FLOATVECTOR3 vColors[2]) {
      if (vColors[0] != m_vBackgroundColors[0] ||
          vColors[1] != m_vBackgroundColors[1]) {
        m_vBackgroundColors[0]=vColors[0];
        m_vBackgroundColors[1]=vColors[1];
        ScheduleCompleteRedraw();
        return true;
      } return false;
    }

    virtual bool SetTextColor(FLOATVECTOR4 vColor) {
      if (vColor != m_vTextColor) {
        m_vTextColor=vColor;ScheduleCompleteRedraw();
        return true;
      } return false;
    }

    FLOATVECTOR3 GetBackgroundColor(int i) const {
      return m_vBackgroundColors[i];
    }
    FLOATVECTOR4 GetTextColor() const {return m_vTextColor;}

    virtual void SetSampleRateModifier(float fSampleRateModifier);
    float GetSampleRateModifier() const { return m_fSampleRateModifier; }

    virtual void SetIsoValue(float fIsovalue);
    float GetIsoValue() const { return m_fIsovalue; }

    virtual void SetIsosufaceColor(const FLOATVECTOR3& vColor);
    virtual FLOATVECTOR3 GetIsosufaceColor() const { return m_vIsoColor; }

    virtual void SetOrthoView(bool bOrthoView);
    virtual bool GetOrthoView() const {return m_bOrthoView;}

    virtual void SetViewPort(UINTVECTOR2 lower_left, UINTVECTOR2 upper_right,
                             bool decrease_screen_res) = 0;

    virtual void SetRenderCoordArrows(bool bRenderCoordArrows);
    virtual bool GetRenderCoordArrows() const {return m_bRenderCoordArrows;}

    virtual void Set2DPlanesIn3DView(bool bRenderPlanesIn3D,
                                     RenderRegion* renderRegion=NULL);
    virtual bool Get2DPlanesIn3DView(RenderRegion* = NULL) const {
      /// @todo: Make this bool a per 3d render region toggle.
      return m_bRenderPlanesIn3D;}

    /** Change the size of the render window.  Any previous image is
     * destroyed, causing a full redraw on the next render.
     * \param vWinSize  new width and height of the view window */
    virtual void Resize(const UINTVECTOR2& vWinSize);

    virtual void SetRotation(RenderRegion *renderRegion,
                             const FLOATMATRIX4& mRotation);
    virtual const FLOATMATRIX4& GetRotation(const RenderRegion *renderRegion) const;

    virtual void SetTranslation(RenderRegion *renderRegion,
                                const FLOATMATRIX4& mTranslation);
    virtual const FLOATMATRIX4& GetTranslation(
                                        const RenderRegion *renderRegion) const;

    void SetClipPlane(RenderRegion *renderRegion,
                      const ExtendedPlane& plane);
    virtual void EnableClipPlane(RenderRegion *renderRegion=NULL);
    virtual void DisableClipPlane(RenderRegion *renderRegion=NULL);
    virtual void ShowClipPlane(bool, RenderRegion *renderRegion=NULL);

    virtual void ClipPlaneRelativeLock(bool);
    virtual bool CanDoClipPlane() {return true;}
    bool ClipPlaneEnabled() const { return m_bClipPlaneOn; }
    bool ClipPlaneShown() const   { return m_bClipPlaneDisplayed; }
    bool ClipPlaneLocked() const  { return m_bClipPlaneLocked; }

    /// slice parameter for slice views.
    virtual void SetSliceDepth(RenderRegion *renderRegion, UINT64 fSliceDepth);
    virtual UINT64 GetSliceDepth(const RenderRegion *renderRegion) const;

    void SetClearFramebuffer(bool bClearFramebuffer) {
      m_bClearFramebuffer = bClearFramebuffer;
    }
    bool GetClearFramebuffer() {return m_bClearFramebuffer;}
    void SetGlobalBBox(bool bRenderBBox);
    bool GetGlobalBBox() {return m_bRenderGlobalBBox;}
    void SetLocalBBox(bool bRenderBBox);
    bool GetLocalBBox() {return m_bRenderLocalBBox;}

    virtual void SetLogoParams(std::string strLogoFilename, int iLogoPos);
    void Set2DFlipMode(RenderRegion *renderRegion, bool bFlipX, bool bFlipY);
    void Get2DFlipMode(const RenderRegion *renderRegion, bool& bFlipX,
                       bool& bFlipY) const;
    bool GetUseMIP(const RenderRegion *renderRegion) const;
    void SetUseMIP(RenderRegion *renderRegion, bool bUseMIP);

    UINT64 GetMaxLODIndex() const      { return m_iMaxLODIndex; }
    UINT64 GetMinLODIndex() const      { return m_iMinLODForCurrentView; }

    UINTVECTOR2 GetLODLimits() const      { return m_iLODLimits; }
    void SetLODLimits(const UINTVECTOR2 iLODLimits);

    // scheduling routines
    UINT64 GetCurrentSubFrameCount() const
        { return 1+m_iMaxLODIndex-m_iMinLODForCurrentView; }
    UINT32 GetWorkingSubFrame() const
        { return 1+m_iMaxLODIndex-m_iCurrentLOD; }

    UINT32 GetCurrentBrickCount() const
        { return UINT32(m_vCurrentBrickList.size()); }
    UINT32 GetWorkingBrick() const
        { return m_iBricksRenderedInThisSubFrame; }

    UINT32 GetFrameProgress() const {
        return UINT32(100.0f * float(GetWorkingSubFrame()) /
                      float(GetCurrentSubFrameCount()));
    }
    UINT32 GetSubFrameProgress() const {
        return (m_vCurrentBrickList.empty()) ? 100 :
                UINT32(100.0f * m_iBricksRenderedInThisSubFrame /
                float(m_vCurrentBrickList.size()));
    }

    void SetTimeSlice(UINT32 iMSecs) {m_iTimeSliceMSecs = iMSecs;}
    void SetPerfMeasures(UINT32 iMinFramerate, bool bUseAllMeans,
                         float fScreenResDecFactor,
                         float fSampleDecFactor, UINT32 iStartDelay);
    void SetRescaleFactors(const DOUBLEVECTOR3& vfRescale) {
      m_pDataset->SetRescaleFactors(vfRescale); ScheduleCompleteRedraw();
    }
    DOUBLEVECTOR3 GetRescaleFactors() {
      return m_pDataset->GetRescaleFactors();
    }

    void SetCaptureMode(bool bCaptureMode) {m_bCaptureMode = bCaptureMode;}
    void SetMIPLOD(bool bMIPLOD) {m_bMIPLOD = bMIPLOD;}

    virtual void  SetStereo(bool bStereoRendering);
    virtual void  SetStereoEyeDist(float fStereoEyeDist);
    virtual void  SetStereoFocalLength(float fStereoFocalLength);
    virtual bool  GetStereo() {return m_bRequestStereoRendering;}
    virtual float GetStereoEyeDist() {return m_fStereoEyeDist;}
    virtual float GetStereoFocalLength() {return m_fStereoFocalLength;}

    virtual void  SetConsiderPreviousDepthbuffer(bool bConsiderPreviousDepthbuffer);
    virtual bool  GetConsiderPreviousDepthbuffer() {return m_bConsiderPreviousDepthbuffer;}

    void SetColors(const FLOATVECTOR4& ambient,
                   const FLOATVECTOR4& diffuse,
                   const FLOATVECTOR4& specular);


    FLOATVECTOR4 GetAmbient() const;
    FLOATVECTOR4 GetDiffuse() const;
    FLOATVECTOR4 GetSpecular()const;

    // ClearView
    virtual bool SupportsClearView() {return false;}
    virtual void SetCV(bool bEnable);
    virtual bool GetCV() const {return m_bDoClearView;}
    virtual void SetCVIsoValue(float fIsovalue);
    virtual float GetCVIsoValue() const { return m_fCVIsovalue; }
    virtual void SetCVColor(const FLOATVECTOR3& vColor);
    virtual FLOATVECTOR3 GetCVColor() const {return m_vCVColor;}
    virtual void SetCVSize(float fSize);
    virtual float GetCVSize() const {return m_fCVSize;}
    virtual void SetCVContextScale(float fScale);
    virtual float GetCVContextScale() const {return m_fCVContextScale;}
    virtual void SetCVBorderScale(float fScale);
    virtual float GetCVBorderScale() const {return m_fCVBorderScale;}
    virtual void SetCVFocusPos(RenderRegion& renderRegion, INTVECTOR2 vPos);
    virtual INTVECTOR2 GetCVFocusPos() const {return m_vCVMousePos;}

    virtual void ScheduleCompleteRedraw();
    virtual void Schedule3DWindowRedraws(); // Redraw all 3D windows.
    virtual void ScheduleWindowRedraw(RenderRegion *renderRegion);

    void SetAvoidSeperateCompositing(bool bAvoidSeperateCompositing) {
      m_bAvoidSeperateCompositing = bAvoidSeperateCompositing;
    }
    bool GetAvoidSeperateCompositing() const {
      return m_bAvoidSeperateCompositing;
    }

    void SetMIPRotationAngle(float fAngle) {
      m_fMIPRotationAngle = fAngle;
      ScheduleCompleteRedraw();
    }

    /// Prepends the given directory to the list of paths Tuvok will
    /// try to find shaders in.
    void AddShaderPath(const std::string &path) {
      m_vShaderSearchDirs.insert(m_vShaderSearchDirs.begin(), path);
    }

    void SetViewParameters(float angle, float znear, float zfar,
                           const FLOATVECTOR3& eye,
                           const FLOATVECTOR3& ref,
                           const FLOATVECTOR3& vup) {
      m_vEye = eye;
      m_vAt = ref;
      m_vUp = vup;
      m_fFOV = angle;
      m_fZNear = znear;
      m_fZFar = zfar;
    }

    void SetScalingMethod(enum ScalingMethod sm) {
      this->m_TFScalingMethod = sm;
    }

    virtual void NewFrameClear(const RenderRegion &) { assert(1==0); }

    const std::vector<RenderRegion*>& GetRenderRegions() const {
      return renderRegions; }
    void SetRenderRegions(const std::vector<RenderRegion*> &regions) {
      renderRegions = regions; }

  protected:
    /// Unsets the current transfer function, including deleting it from GPU
    /// memory.  It's expected you'll set another one directly afterwards.
    void Free1DTrans();

    /// @return the current iso value, normalized to be in [0,1]
    double GetNormalizedIsovalue() const;
    /// @return the current clearview iso value, normalized to be in [0,1]
    double GetNormalizedCVIsovalue() const;

  protected:
    MasterController*   m_pMasterController;
    ERenderMode         m_eRenderMode;
    EBlendPrecision     m_eBlendPrecision;
    bool                m_bUseLighting;
    Dataset*            m_pDataset;
    TransferFunction1D* m_p1DTrans;
    TransferFunction2D* m_p2DTrans;
    float               m_fSampleRateModifier;
    FLOATVECTOR3        m_vIsoColor;
    FLOATVECTOR3        m_vBackgroundColors[2];
    FLOATVECTOR4        m_vTextColor;
    bool                m_bRenderGlobalBBox;
    bool                m_bRenderLocalBBox;
    UINTVECTOR2         m_vWinSize;
    int                 m_iLogoPos;
    std::string         m_strLogoFilename;

    UINT32              m_iLODNotOKCounter;
    float               m_fMaxMSPerFrame;
    float               m_fScreenResDecFactor;
    float               m_fSampleDecFactor;
    bool                m_bUseAllMeans;
    bool                m_bOffscreenIsLowRes;
    UINT32              m_iStartDelay;
    UINT64              m_iMinLODForCurrentView;
    UINT32              m_iTimeSliceMSecs;

    UINT64              m_iIntraFrameCounter;
    UINT64              m_iFrameCounter;
    UINT32              m_iCheckCounter;
    UINT64              m_iMaxLODIndex;
    UINTVECTOR2         m_iLODLimits;
    UINT64              m_iPerformanceBasedLODSkip;
    UINT64              m_iCurrentLODOffset;
    UINT64              m_iStartLODOffset;
    CullingLOD          m_FrustumCullingLOD;
    bool                m_bClearFramebuffer;
    bool                m_bConsiderPreviousDepthbuffer;
    UINT64              m_iCurrentLOD;
    UINT64              m_iBricksRenderedInThisSubFrame;
    std::vector<Brick>  m_vCurrentBrickList;
    std::vector<Brick>  m_vLeftEyeBrickList;
    bool                m_bCaptureMode;
    bool                m_bMIPLOD;
    float               m_fMIPRotationAngle;
    FLOATMATRIX4        m_maMIPRotation;
    bool                m_bOrthoView;
    bool                m_bRenderCoordArrows;
    bool                m_bRenderPlanesIn3D;

    bool                m_bDoClearView;
    FLOATVECTOR3        m_vCVColor;
    float               m_fCVSize;
    float               m_fCVContextScale;
    float               m_fCVBorderScale;
    INTVECTOR2          m_vCVMousePos;
    FLOATVECTOR4        m_vCVPos;
    bool                m_bPerformReCompose;
    bool                m_bRequestStereoRendering;
    bool                m_bDoStereoRendering;
    float               m_fStereoEyeDist;
    float               m_fStereoFocalLength;
    std::vector<Triangle> m_vArrowGeometry;

    // compatibility settings
    bool                m_bUseOnlyPowerOfTwo;
    bool                m_bDownSampleTo8Bits;
    bool                m_bDisableBorder;
    bool                m_bAvoidSeperateCompositing;
    enum ScalingMethod  m_TFScalingMethod;

    FLOATMATRIX4        m_mProjection[2];
    FLOATMATRIX4        m_mView[2];
    std::vector<std::string> m_vShaderSearchDirs;

    ExtendedPlane       m_ClipPlane;
    bool                m_bClipPlaneOn;
    bool                m_bClipPlaneDisplayed;
    bool                m_bClipPlaneLocked;

    /// view parameters.
    ///@{
    FLOATVECTOR3        m_vEye, m_vAt, m_vUp;
    float               m_fFOV;
    float               m_fZNear, m_fZFar;
    ///@}

    std::vector<RenderRegion*> renderRegions;

    // For displaying a full single 3D view without the client needing to know
    // about RenderRegions.
    RenderRegion3D simpleRenderRegion3D;

    FLOATVECTOR4        m_cAmbient;
    FLOATVECTOR4        m_cDiffuse;
    FLOATVECTOR4        m_cSpecular;

    virtual void        ScheduleRecompose(RenderRegion *renderRegion=NULL);
    void                ComputeMinLODForCurrentView();
    void                ComputeMaxLODForCurrentView(RenderRegion& region);
    void                Plan3DFrame(RenderRegion3D& region);
    void                PlanHQMIPFrame(RenderRegion& renderRegion);
    std::vector<Brick>  BuildSubFrameBrickList(const RenderRegion& renderRegion,
                                               bool bUseResidencyAsDistanceCriterion=false);
    std::vector<Brick>  BuildLeftEyeSubFrameBrickList(RenderRegion& renderRegion,
                                                      const std::vector<Brick>& vRightEyeBrickList);
    virtual void        ClearDepthBuffer() = 0;
    virtual void        ClearColorBuffer() = 0;
    virtual void        CVFocusHasChanged(RenderRegion &);
    void                CompletedASubframe(RenderRegion* region);
    void                RestartTimer(RenderRegion& region, const size_t iTimerIndex);
    void                RestartTimers(RenderRegion& region);
    virtual void        UpdateColorsInShaders() = 0;
    double              MaxValue() const;
    bool                OnlyRecomposite(RenderRegion* region) const;

    RenderRegion3D* GetFirst3DRegion();

  private:
    float               m_fIsovalue;
    float               m_fCVIsovalue;
};

}; //namespace tuvok

#endif // ABSTRRENDERER_H
