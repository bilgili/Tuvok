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
  \file    Dataset.cpp
  \author  Tom Fogal
           SCI Institute
           University of Utah
*/
#include "Dataset.h"

namespace tuvok {

Dataset::Dataset():
  m_pHist1D(NULL),
  m_pHist2D(NULL),
  m_UserScale(1.0,1.0,1.0),
  m_DomainScale(1.0, 1.0, 1.0)
{
  m_DomainScale = DOUBLEVECTOR3(1.0, 1.0, 1.0);
}

Dataset::~Dataset()
{
  delete m_pHist1D;            m_pHist1D = NULL;
  delete m_pHist2D;            m_pHist2D = NULL;
}

/// unimplemented!
bool Dataset::Export(UINT64, const std::string&, bool,
                            bool (*)(LargeRAWFile* pSourceFile,
                                     const std::vector<UINT64> vBrickSize,
                                     const std::vector<UINT64> vBrickOffset,
                                     void* pUserContext),
                            void *, UINT64) const {
  return false;
}

void Dataset::SetRescaleFactors(const DOUBLEVECTOR3& rescale) {
  m_UserScale = rescale;
}

DOUBLEVECTOR3 Dataset::GetRescaleFactors() const {
  return m_UserScale;
}


}; // tuvok namespace.
