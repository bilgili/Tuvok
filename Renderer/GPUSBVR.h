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
  \file    GPUSBVR.h
  \author  Jens Krueger
           SCI Institute
           University of Utah
  \version 1.0
  \date    August 2008
*/


#pragma once

#ifndef GPUSBVR_H
#define GPUSBVR_H

#include "GLInclude.h"
#include <Renderer/GPUMemMan/GPUMemMan.h>
#include <Renderer/SBVRGeogen.h>
#include <Renderer/GLRenderer.h>
#include <deque>

/** \class GPUSBVR
 * Slice-based GPU volume renderer.
 *
 * GPUSBVR is a slice based volume renderer which uses GLSL. */
class GPUSBVR : public GLRenderer {
  public:
    /** Constructs a VRer with immediate redraw, and
     * wireframe mode off.
     * \param pMasterController message routing object */
    GPUSBVR(MasterController* pMasterController);
    virtual ~GPUSBVR();

    virtual bool LoadDataset(const std::string& strFilename);

    void Paint();
    /** Change the size of the FBO we render to.  Any previous image is
     * destroyed, causing a full redraw on the next render.
     * \param vWinSize  new width and height of the view window */
    virtual void Resize(const UINTVECTOR2& vWinSize);

    /** Deallocates GPU memory allocated during the rendering process. */
    void Cleanup();

    /** Loads GLSL vertex and fragment shaders. */
    virtual bool Initialize();

    /** Query whether or not we should redraw the next frame, else we should
     * reuse what is already rendered. */
    virtual bool CheckForRedraw();

    /** Set the oversampling ratio (e.g. 2 means twice the slices as needed).  Causes a full redraw. */
    virtual void SetSampleRateModifier(float fSampleRateModifier);

    /** Set the bit depth mode of the offscreen buffer used for blending.  Causes a full redraw. */
    void SetBlendPrecision(EBlendPrecision eBlendPrecision);

  protected:
    FLOATMATRIX4  m_matModelView;
    SBVRGeogen    m_SBVRGeogen;
    GLTexture2D*  m_IDTex[3];

    GLFBOTex*     m_pFBO3DImageLast;
    GLFBOTex*     m_pFBO3DImageCurrent;
    GLSLProgram*  m_pProgram1DTrans[2];
    GLSLProgram*  m_pProgram2DTrans[2];
    GLSLProgram*  m_pProgramIso;
    GLSLProgram*  m_pProgramTrans;
    GLSLProgram*  m_pProgram1DTransSlice;
    GLSLProgram*  m_pProgram2DTransSlice;
    int           m_iFilledBuffers;

    void DrawLogo();
    void DrawBackGradient();
    void RerenderPreviousResult();
    void SetDataDepShaderVars();
    void SetBrickDepShaderVars(UINT64 iCurrentLOD, const Brick& currentBrick);
    void SetBrickDepShaderVarsSlice(UINT64 iCurrentLOD, const UINTVECTOR3& vVoxelCount);

    void Plan3DFrame();
    bool Execute3DFrame(ERenderArea eREnderArea);

    void Render3DView();
    bool Render2DView(EWindowMode eDirection, float fSliceIndex);
    void RenderBBox(const FLOATVECTOR4 vColor = FLOATVECTOR4(1,0,0,1));
    void RenderBBox(const FLOATVECTOR4 vColor, const FLOATVECTOR3& vCenter, const FLOATVECTOR3& vExtend);
    void CreateOffscreenBuffer();

    std::vector<Brick> BuildFrameBrickList();
    void SetRenderTargetArea(ERenderArea eREnderArea);
    void SetRenderTargetAreaScissor(ERenderArea eREnderArea);
    void SetViewPort(UINTVECTOR2 viLowerLeft, UINTVECTOR2 viUpperRight);
};

#endif // GPUSBVR_H
