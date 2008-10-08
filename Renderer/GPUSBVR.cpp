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
  \author    Jens Krueger
        SCI Institute
        University of Utah
  \date    August 2008
*/


#include "GPUSBVR.h"

#include <math.h>
#include <Basics/SysTools.h>
#include <Controller/MasterController.h>

using namespace std;

GPUSBVR::GPUSBVR(MasterController* pMasterController) :
  GLRenderer(pMasterController),
  m_iCurrentView(0),
  m_vRot(0,0),
  m_bDelayedCompleteRedraw(false),
  m_bRenderWireframe(false),
  m_pFBO3DImage(NULL),
  m_pProgram1DTrans(NULL)
{
}

GPUSBVR::~GPUSBVR() {
  delete [] m_p1DData;
  delete [] m_p2DData;
}


void GPUSBVR::Initialize() {
  GLRenderer::Initialize();

  m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","");

  glClearColor(m_vBackgroundColors[0].x,m_vBackgroundColors[0].y,m_vBackgroundColors[0].z,0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  
  m_IDTex[0] = m_pMasterController->MemMan()->Load2DTextureFromFile(SysTools::GetFromResourceOnMac("RenderWin1x3.bmp").c_str());
  if (m_IDTex[0] == NULL) {
    m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","First Image load failed");    
  }
  m_IDTex[1] = m_pMasterController->MemMan()->Load2DTextureFromFile(SysTools::GetFromResourceOnMac("RenderWin2x2.bmp").c_str());
  if (m_IDTex[1] == NULL) {
    m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","Second Image load failed");    
  }
  m_IDTex[2] = m_pMasterController->MemMan()->Load2DTextureFromFile(SysTools::GetFromResourceOnMac("RenderWin1.bmp").c_str());
  if (m_IDTex[2] == NULL) {
    m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","Third Image load failed");    
  }

  m_pProgram1DTrans = m_pMasterController->MemMan()->GetGLSLProgram(SysTools::GetFromResourceOnMac("GPUSBVR-1D-VS.glsl"),
                                                                    SysTools::GetFromResourceOnMac("GPUSBVR-1D-FS.glsl"));

 
  m_pProgram1DTrans->Enable();
  m_pProgram1DTrans->SetUniformVector("texVolTexture",0);
  m_pProgram1DTrans->SetUniformVector("texTrans1DTexture",1);
  m_pProgram1DTrans->Disable();
}

void GPUSBVR::SetDataDepShaderVars() {
  m_pProgram1DTrans->Enable();
  size_t       iMaxValue = m_p1DTrans->vColorData.size();
  unsigned int iMaxRange = (unsigned int)(1<<m_pDataset->GetInfo()->GetBitwith());
  float fScale = float(iMaxRange)/float(iMaxValue);
  m_pProgram1DTrans->SetUniformVector("fTransScale",fScale);
  m_pProgram1DTrans->Disable();
}

bool GPUSBVR::LoadDataset(const string& strFilename) {
  if (GLRenderer::LoadDataset(strFilename)) {
    if (m_pProgram1DTrans != NULL) SetDataDepShaderVars();
    return true;
  } else return false;
}


void GPUSBVR::UpdateGeoGen(const std::vector<UINT64>& vLOD, const std::vector<UINT64>& vBrick) {
  UINTVECTOR3  vSize = m_pDataset->GetBrickSize(vLOD, vBrick);
  FLOATVECTOR3 vCenter, vExtension;
  m_pDataset->GetBrickCenterAndExtension(vLOD, vBrick, vCenter, vExtension);

  // TODO: transfer brick offsets to m_SBVRGeogen
  m_SBVRGeogen.SetVolumeData(vExtension, vSize);
}

void GPUSBVR::DrawLogo() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-0.5, +0.5, +0.5, -0.5, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (m_IDTex[m_iCurrentView] != NULL) m_IDTex[m_iCurrentView]->Bind();
    glDisable(GL_TEXTURE_3D);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
      glColor4d(1,1,1,1);
      glTexCoord2d(0,0);
      glVertex3d(0.2,  0.4, -0.5);
      glTexCoord2d(1,0);
      glVertex3d( 0.4,  0.4, -0.5);
      glTexCoord2d(1,1);
      glVertex3d( 0.4, 0.2, -0.5);
      glTexCoord2d(0,1);
      glVertex3d(0.2, 0.2, -0.5);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void GPUSBVR::DrawBackGradient() {
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-0.5, +0.5, +0.5, -0.5, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_TEXTURE_3D);

  glBegin(GL_QUADS);
    glColor4d(m_vBackgroundColors[1].x,m_vBackgroundColors[1].y,m_vBackgroundColors[1].z,1);
    glVertex3d(-1.0,  1.0, -0.5);
    glVertex3d( 1.0,  1.0, -0.5);
    glColor4d(m_vBackgroundColors[0].x,m_vBackgroundColors[0].y,m_vBackgroundColors[0].z,1);
    glVertex3d( 1.0, -1.0, -0.5);
    glVertex3d(-1.0, -1.0, -0.5);
  glEnd();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glEnable(GL_DEPTH_TEST);
}

void GPUSBVR::Paint(bool bClearDepthBuffer) {

  if (bClearDepthBuffer) glClear(GL_DEPTH_BUFFER_BIT);

  if (m_bCompleteRedraw) {
    m_pMasterController->DebugOut()->Message("GPUSBVR::Paint","Complete Redraw");

    // DEBUG: just render the smallest brick for a start
    std::vector<UINT64> vSmallestLOD = m_pDataset->GetInfo()->GetLODLevelCount();
    for (size_t i = 0;i<vSmallestLOD.size();i++) vSmallestLOD[i] -= 1;   
    std::vector<UINT64> vFirstBrick(m_pDataset->GetInfo()->GetBrickCount(vSmallestLOD).size());
    for (size_t i = 0;i<vFirstBrick.size();i++) vFirstBrick[i] = 0;
    UpdateGeoGen(vSmallestLOD, vFirstBrick);

    m_pFBO3DImage->Write();

    SetDataDepShaderVars();

    if (m_vBackgroundColors[0] == m_vBackgroundColors[1]) {
      glClear(GL_COLOR_BUFFER_BIT);
    } else {
      DrawBackGradient();
    }

    DrawLogo();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    FLOATMATRIX4 trans, rotx, roty;
    trans.Translation(0,0,-4);
    rotx.RotationAxis(FLOATVECTOR3(0,1,0),m_vRot.x);
    roty.RotationAxis(FLOATVECTOR3(1,0,0),m_vRot.y);
    m_matModelView = trans*rotx*roty;

    //m_matModelView.setModelview();
    m_SBVRGeogen.SetTransformation(m_matModelView);

  	glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // switch to wireframe
    if (m_bRenderWireframe) {
      // TODO save state
	    glDisable(GL_CULL_FACE);
	    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	    glEdgeFlag(true);
    }

    GLTexture3D* t = m_pMasterController->MemMan()->Get3DTexture(m_pDataset, vSmallestLOD, vFirstBrick);
    if(t!=NULL) t->Bind(0);
    m_p1DTransTex->Bind(1);

    m_pProgram1DTrans->Enable();


    glBegin(GL_TRIANGLES);
      glColor4f(1,1,1,1);

      for (size_t i = 0;i<m_SBVRGeogen.m_vSliceTriangles.size();i++) {
        glTexCoord3fv(m_SBVRGeogen.m_vSliceTriangles[i].m_vTex);
        glVertex3fv(m_SBVRGeogen.m_vSliceTriangles[i].m_vPos);
      }
    glEnd();

    m_pFBO3DImage->FinishWrite();

    m_pProgram1DTrans->Disable();

    glDisable(GL_BLEND);

    if (m_bRenderWireframe) {
      // TODO: restore state
    }

  } else m_pMasterController->DebugOut()->Message("GPUSBVR::Paint","Quick Redraw");

  RerenderPreviousResult();

  m_bCompleteRedraw = false;
  m_bRedraw = false;
}

void GPUSBVR::Resize(int width, int height) {
  m_pMasterController->DebugOut()->Message("GPUSBVR::Resize","");

//  int side = std::min(width, height);
//  glViewport((width - side) / 2, (height - side) / 2, side, side);

  float aspect=(float)width/(float)height;
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);		
	glLoadIdentity();
	gluPerspective(50.0,aspect,0.2,100.0); 	// Set Projection. Arguments are FOV (in degrees), aspect-ratio, near-plane, far-plane.
	glMatrixMode(GL_MODELVIEW);

  if (m_pFBO3DImage != NULL) m_pMasterController->MemMan()->FreeFBO(m_pFBO3DImage);
  m_pFBO3DImage = m_pMasterController->MemMan()->GetFBO(GL_NEAREST, GL_NEAREST, GL_CLAMP, width, height, GL_RGBA8, 8*4);

  m_bDelayedCompleteRedraw = true;
}

void GPUSBVR::Cleanup() {
  m_pMasterController->MemMan()->FreeTexture(m_IDTex[0]);
  m_pMasterController->MemMan()->FreeTexture(m_IDTex[1]);
  m_pMasterController->MemMan()->FreeTexture(m_IDTex[2]);

  m_pMasterController->MemMan()->FreeFBO(m_pFBO3DImage);
  m_pMasterController->MemMan()->FreeGLSLProgram(m_pProgram1DTrans);
}


bool GPUSBVR::CheckForRedraw() {
  if (m_bDelayedCompleteRedraw) {
    m_bDelayedCompleteRedraw = false;
    m_bCompleteRedraw = true;
    m_bRedraw = true;
  }

  return m_bRedraw;
}


void GPUSBVR::RerenderPreviousResult() {
  m_pFBO3DImage->Read(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);

  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1.0, +1.0, +1.0, -1.0, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glBegin(GL_QUADS);
    glColor4d(1,1,1,1);
    glTexCoord2d(0,0);
    glVertex3d(-1.0,  1.0, -0.5);
    glTexCoord2d(1,0);
    glVertex3d( 1.0,  1.0, -0.5);
    glTexCoord2d(1,1);
    glVertex3d( 1.0, -1.0, -0.5);
    glTexCoord2d(0,1);
    glVertex3d(-1.0, -1.0, -0.5);
  glEnd();

  m_pFBO3DImage->FinishRead();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
}
