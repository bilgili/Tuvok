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
  \author    Jens Krueger
        SCI Institute
        University of Utah
  \version  1.0
  \date    August 2008
*/


#pragma once

#ifndef ABSTRRENDERER_H
#define ABSTRRENDERER_H

#include <string>

#include "../IO/VolumeDataset.h"
#include "../IO/TransferFunction1D.h"
#include "../IO/TransferFunction2D.h"
#include "../Renderer/GLTexture1D.h"
#include "../Renderer/GLTexture2D.h"

class MasterController;

enum ERenderMode {
  RM_1DTRANS = 0,
  RM_2DTRANS,
  RM_ISOSURFACE,
  RM_INVALID
};

class AbstrRenderer {
  public:
    AbstrRenderer(MasterController* pMasterController);
    virtual ~AbstrRenderer();
    virtual bool LoadDataset(const std::string& strFilename);
    virtual bool CheckForRedraw() = 0;

    VolumeDataset*      GetDataSet() {return m_pDataset;}
    TransferFunction1D* Get1DTrans() {return m_p1DTrans;}
    TransferFunction2D* Get2DTrans() {return m_p2DTrans;}
    
    ERenderMode GetRendermode() {return m_eRenderMode;}
    virtual void SetRendermode(ERenderMode eRenderMode);
    virtual void Changed1DTrans();
    virtual void Changed2DTrans();

    virtual void SetBackgroundColors(FLOATVECTOR3 vColors[2]) {m_vBackgroundColors[0]=vColors[0];m_vBackgroundColors[1]=vColors[1];}
    virtual void SetTextColor(FLOATVECTOR4 vColor) {m_vTextColor=vColor;}
    FLOATVECTOR3 GetBackgroundColor(int i) const {return m_vBackgroundColors[i];}
    FLOATVECTOR4 GetTextColor() const {return m_vTextColor;}

  protected:
    MasterController*   m_pMasterController;
    bool                m_bRedraw;
    bool                m_bCompleteRedraw;
    ERenderMode         m_eRenderMode;
    VolumeDataset*      m_pDataset;
    TransferFunction1D* m_p1DTrans;
    TransferFunction2D* m_p2DTrans;

    FLOATVECTOR3        m_vBackgroundColors[2];
    FLOATVECTOR4        m_vTextColor;
};

#endif // ABSTRRENDERER_H
