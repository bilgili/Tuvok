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
  \file    GLRaycaster.cpp
  \author    Jens Krueger
        SCI Institute
        University of Utah
  \date    November 2008
*/


#include "GLRaycaster.h"

#include <cmath>
#include <Basics/SysTools.h>
#include <Controller/MasterController.h>
#include <ctime>

using namespace std;

GLRaycaster::GLRaycaster(MasterController* pMasterController) :
  GLRenderer(pMasterController)
{
  m_pProgram1DTrans[0]   = NULL;
  m_pProgram1DTrans[1]   = NULL;
  m_pProgram2DTrans[0]   = NULL;
  m_pProgram2DTrans[1]   = NULL;
  m_pProgramIso          = NULL;
}

GLRaycaster::~GLRaycaster() {
}


bool GLRaycaster::Initialize() {
  if (!GLRenderer::Initialize()) {
    m_pMasterController->DebugOut()->Error("GLRaycaster::Initialize","Error in parent call -> aborting");
    return false;
  }

  glShadeModel(GL_SMOOTH);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  
  if (!LoadAndVerifyShader("Shaders/GLRaycaster-VS.glsl", "Shaders/GLRaycaster-1D-FS.glsl",       &(m_pProgram1DTrans[0])) ||
      !LoadAndVerifyShader("Shaders/GLRaycaster-VS.glsl", "Shaders/GLRaycaster-1D-light-FS.glsl", &(m_pProgram1DTrans[1])) ||
      !LoadAndVerifyShader("Shaders/GLRaycaster-VS.glsl", "Shaders/GLRaycaster-2D-FS.glsl",       &(m_pProgram2DTrans[0])) ||
      !LoadAndVerifyShader("Shaders/GLRaycaster-VS.glsl", "Shaders/GLRaycaster-2D-light-FS.glsl", &(m_pProgram2DTrans[1])) ||
      !LoadAndVerifyShader("Shaders/GLRaycaster-VS.glsl", "Shaders/GLRaycaster-ISO-FS.glsl",      &m_pProgramIso)) {

      m_pMasterController->DebugOut()->Error("GLRaycaster::Initialize","Error loading a shader.");
      return false;
  } else {

    m_pProgram1DTrans[0]->Enable();
    m_pProgram1DTrans[0]->SetUniformVector("texVolume",0);
    m_pProgram1DTrans[0]->SetUniformVector("texTrans1D",1);
    m_pProgram1DTrans[0]->Disable();

    m_pProgram1DTrans[1]->Enable();
    m_pProgram1DTrans[1]->SetUniformVector("texVolume",0);
    m_pProgram1DTrans[1]->SetUniformVector("texTrans1D",1);
    m_pProgram1DTrans[1]->SetUniformVector("vLightAmbient",0.2f,0.2f,0.2f);
    m_pProgram1DTrans[1]->SetUniformVector("vLightDiffuse",1.0f,1.0f,1.0f);
    m_pProgram1DTrans[1]->SetUniformVector("vLightSpecular",1.0f,1.0f,1.0f);
    m_pProgram1DTrans[1]->SetUniformVector("vLightDir",0.0f,0.0f,-1.0f);
    m_pProgram1DTrans[1]->Disable();

    m_pProgram2DTrans[0]->Enable();
    m_pProgram2DTrans[0]->SetUniformVector("texVolume",0);
    m_pProgram2DTrans[0]->SetUniformVector("texTrans2D",1);
    m_pProgram2DTrans[0]->Disable();

    m_pProgram2DTrans[1]->Enable();
    m_pProgram2DTrans[1]->SetUniformVector("texVolume",0);
    m_pProgram2DTrans[1]->SetUniformVector("texTrans2D",1);
    m_pProgram2DTrans[1]->SetUniformVector("vLightAmbient",0.2f,0.2f,0.2f);
    m_pProgram2DTrans[1]->SetUniformVector("vLightDiffuse",1.0f,1.0f,1.0f);
    m_pProgram2DTrans[1]->SetUniformVector("vLightSpecular",1.0f,1.0f,1.0f);
    m_pProgram2DTrans[1]->SetUniformVector("vLightDir",0.0f,0.0f,-1.0f);
    m_pProgram2DTrans[1]->Disable();

    m_pProgramIso->Enable();
    m_pProgramIso->SetUniformVector("texVolume",0);
    m_pProgramIso->SetUniformVector("vLightAmbient",0.2f,0.2f,0.2f);
    m_pProgramIso->SetUniformVector("vLightDiffuse",0.8f,0.8f,0.8f);
    m_pProgramIso->SetUniformVector("vLightSpecular",1.0f,1.0f,1.0f);
    m_pProgramIso->SetUniformVector("vLightDir",0.0f,0.0f,-1.0f);
    m_pProgramIso->Disable();
  }

  return true;
}

void GLRaycaster::SetBrickDepShaderVars(const Brick& currentBrick) {

  FLOATVECTOR3 vStep(1.0f/currentBrick.vVoxelCount.x, 1.0f/currentBrick.vVoxelCount.y, 1.0f/currentBrick.vVoxelCount.z);

  float fStepScale = currentBrick.vVoxelCount.maxVal() * 0.5f * 1.0f/m_fSampleRateModifier * (currentBrick.vExtension/FLOATVECTOR3(currentBrick.vVoxelCount)).minVal();

  switch (m_eRenderMode) {
    case RM_1DTRANS    :  {                    
                            m_pProgram1DTrans[m_bUseLigthing ? 1 : 0]->SetUniformVector("fStepScale", fStepScale);
                            if (m_bUseLigthing)
                                m_pProgram1DTrans[1]->SetUniformVector("vVoxelStepsize", vStep.x, vStep.y, vStep.z);
                            break;
                          }
    case RM_2DTRANS    :  {
                            m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->SetUniformVector("fStepScale", fStepScale);
                            m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->SetUniformVector("vVoxelStepsize", vStep.x, vStep.y, vStep.z);
                            break;
                          }
    case RM_ISOSURFACE : {
                            m_pProgramIso->SetUniformVector("vVoxelStepsize", vStep.x, vStep.y, vStep.z);
                            break;
                          }
    case RM_INVALID    :  m_pMasterController->DebugOut()->Error("GLRaycaster::SetBrickDepShaderVars","Invalid rendermode set"); break;
  }

}

const FLOATVECTOR2 GLRaycaster::SetDataDepShaderVars() {
  const FLOATVECTOR2 vSizes = GLRenderer::SetDataDepShaderVars();

  switch (m_eRenderMode) {
    case RM_1DTRANS    :  {
                            m_pProgram1DTrans[m_bUseLigthing ? 1 : 0]->Enable();
                            m_pProgram1DTrans[m_bUseLigthing ? 1 : 0]->SetUniformVector("fTransScale",vSizes.x);
                            m_pProgram1DTrans[m_bUseLigthing ? 1 : 0]->Disable();
                            break;
                          }
    case RM_2DTRANS    :  {
                            float fGradientScale = 1.0f/m_pDataset->GetMaxGradMagnitude();
                            m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->Enable();
                            m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->SetUniformVector("fTransScale",vSizes.x);
                            m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->SetUniformVector("fGradientScale",vSizes.y);
                            m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->Disable();
                            break;
                          }
    case RM_ISOSURFACE : {
                            m_pProgramIso->Enable();
                            m_pProgramIso->SetUniformVector("fIsoval",m_fIsovalue/vSizes.x);
                            m_pProgramIso->Disable();
                            break;
                          }
    case RM_INVALID    :  m_pMasterController->DebugOut()->Error("GLRaycaster::SetDataDepShaderVars","Invalid rendermode set"); break;
  }


  return vSizes;
}

bool GLRaycaster::LoadDataset(const string& strFilename) {
  if (GLRenderer::LoadDataset(strFilename)) {
    if (m_pProgram1DTrans[0] != NULL) SetDataDepShaderVars();
    return true;
  } else return false;
}

void GLRaycaster::Render3DView() {
  /*// ************** GL States ***********
  // Modelview
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  m_matModelView.setModelview();

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_3D);
  glDisable(GL_TEXTURE_2D);

  if (m_iBricksRenderedInThisSubFrame == 0) {
    // for rendering modes other than isosurface render the bbox in the first pass once to init the depth buffer
    // for isosurface rendering we can go ahead and render the bbox directly as isosurfacing 
    // writes out correct depth values
    glDisable(GL_BLEND);
    if (m_eRenderMode != RM_ISOSURFACE) glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    if (m_bRenderGlobalBBox) RenderBBox();
    if (m_bRenderLocalBBox) {
      for (UINT64 iCurrentBrick = 0;iCurrentBrick<m_vCurrentBrickList.size();iCurrentBrick++) {
        RenderBBox(FLOATVECTOR4(0,1,0,1), m_vCurrentBrickList[iCurrentBrick].vCenter, m_vCurrentBrickList[iCurrentBrick].vExtension);
      }
    }
    if (m_eRenderMode != RM_ISOSURFACE) glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  }

  switch (m_eRenderMode) {
    case RM_1DTRANS    :  m_p1DTransTex->Bind(1); 
                          m_pProgram1DTrans[m_bUseLigthing ? 1 : 0]->Enable();
                          glEnable(GL_BLEND);
                          glBlendEquation(GL_FUNC_ADD);
                          glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
                          break;
    case RM_2DTRANS    :  m_p2DTransTex->Bind(1);
                          m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->Enable(); 
                          glEnable(GL_BLEND);
                          glBlendEquation(GL_FUNC_ADD);
                          glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
                          break;
    case RM_ISOSURFACE :  m_pProgramIso->Enable(); 
                          break;
    default    :  m_pMasterController->DebugOut()->Error("GLRaycaster::Render3DView","Invalid rendermode set"); 
                          break;
  }

  if (m_eRenderMode != RM_ISOSURFACE) glDepthMask(GL_FALSE);

  // loop over all bricks in the current LOD level
  clock_t timeStart, timeProbe;
  timeStart = timeProbe = clock();

  while (m_vCurrentBrickList.size() > m_iBricksRenderedInThisSubFrame && float(timeProbe-timeStart)*1000.0f/float(CLOCKS_PER_SEC) < m_iTimeSliceMSecs) {
  
    // setup the slice generator
    m_SBVRGeogen.SetVolumeData(m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vExtension, m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vVoxelCount, 
                               m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vTexcoordsMin, m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vTexcoordsMax);
    FLOATMATRIX4 maBricktTrans; 
    maBricktTrans.Translation(m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vCenter.x, m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vCenter.y, m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vCenter.z);
    FLOATMATRIX4 maBricktModelView = maBricktTrans * m_matModelView;
    maBricktModelView.setModelview();
    m_SBVRGeogen.SetTransformation(maBricktModelView, true);

    // convert 3D variables to the more general ND scheme used in the memory manager, e.i. convert 3-vectors to stl vectors
    vector<UINT64> vLOD; vLOD.push_back(m_iCurrentLOD);
    vector<UINT64> vBrick; 
    vBrick.push_back(m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vCoords.x);
    vBrick.push_back(m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vCoords.y);
    vBrick.push_back(m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame].vCoords.z);

    // get the 3D texture from the memory manager
    GLTexture3D* t = m_pMasterController->MemMan()->Get3DTexture(m_pDataset, vLOD, vBrick, m_iIntraFrameCounter++, m_iFrameCounter);
    if(t!=NULL) t->Bind(0);

    // update the shader parameter
    SetBrickDepShaderVars(m_vCurrentBrickList[m_iBricksRenderedInThisSubFrame]);

    // render the slices
    glBegin(GL_TRIANGLES);
      for (int i = int(m_SBVRGeogen.m_vSliceTriangles.size())-1;i>=0;i--) {
        glTexCoord3fv(m_SBVRGeogen.m_vSliceTriangles[i].m_vTex);
        glVertex3fv(m_SBVRGeogen.m_vSliceTriangles[i].m_vPos);
      }
    glEnd();

    // release the 3D texture
    m_pMasterController->MemMan()->Release3DTexture(t);

    // count the bricks rendered
    m_iBricksRenderedInThisSubFrame++;
	  
    // time this loop
    if (!m_bLODDisabled) timeProbe = clock();
  }

  // disable the shader
  switch (m_eRenderMode) {
    case RM_1DTRANS    :  m_pProgram1DTrans[m_bUseLigthing ? 1 : 0]->Disable(); break;
    case RM_2DTRANS    :  m_pProgram2DTrans[m_bUseLigthing ? 1 : 0]->Disable(); break;
    case RM_ISOSURFACE :  m_pProgramIso->Disable(); break;
    case RM_INVALID    :  m_pMasterController->DebugOut()->Error("GLRaycaster::Render3DView","Invalid rendermode set"); break;
  }

  // at the very end render the bboxes
  if (m_vCurrentBrickList.size() == m_iBricksRenderedInThisSubFrame) {
    if (m_eRenderMode != RM_ISOSURFACE) {    
      m_matModelView.setModelview();
      if (m_bRenderGlobalBBox) {
        glDisable(GL_DEPTH_TEST);
        RenderBBox();
      }

      if (m_bRenderLocalBBox) {
        glDisable(GL_DEPTH_TEST);
        for (UINT64 iCurrentBrick = 0;iCurrentBrick<m_vCurrentBrickList.size();iCurrentBrick++) {
          RenderBBox(FLOATVECTOR4(0,1,0,1), m_vCurrentBrickList[iCurrentBrick].vCenter, m_vCurrentBrickList[iCurrentBrick].vExtension);
        }
      }
      glDepthMask(GL_TRUE);
    }
  }

  glDisable(GL_BLEND);
*/
}


void GLRaycaster::Cleanup() {
  GLRenderer::Cleanup();

  m_pMasterController->MemMan()->FreeGLSLProgram(m_pProgram1DTrans[0]);
  m_pMasterController->MemMan()->FreeGLSLProgram(m_pProgram1DTrans[1]);
  m_pMasterController->MemMan()->FreeGLSLProgram(m_pProgram2DTrans[0]);
  m_pMasterController->MemMan()->FreeGLSLProgram(m_pProgram2DTrans[1]);
  m_pMasterController->MemMan()->FreeGLSLProgram(m_pProgramIso);
}
