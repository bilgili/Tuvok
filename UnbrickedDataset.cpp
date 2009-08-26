/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
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
  \author  Tom Fogal
           SCI Institute
           University of Utah
*/
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <boost/algorithm/minmax_element.hpp>
#include "UnbrickedDataset.h"
#include "UnbrickedDSMetadata.h"
#include "ExternalMetadata.h"

namespace tuvok {

typedef std::vector<std::vector<UINT32> > hist2d;

UnbrickedDataset::UnbrickedDataset() { }
UnbrickedDataset::~UnbrickedDataset() {}

// There's only one brick!  Ignore the key they gave us, just return the domain
// size.
UINT64VECTOR3 UnbrickedDataset::GetBrickSize(const BrickKey&) const
{
#if 0
  // arguments to GetBrickSize are garbage, only to satisfy interface
  /// \todo FIXME Datasets and Metadata use different BrickKeys (uint,uint
  /// versus uint,uint3vec)!
  UINT64VECTOR3 sz = GetInfo().GetBrickSize(
                       UnbrickedDSMetadata::BrickKey(0, UINT64VECTOR3(0,0,0))
                     );
  return sz;
#else
  T_ERROR("completely broken... is this called?");
  return UINT64VECTOR3();
#endif
}

bool UnbrickedDataset::GetBrick(const BrickKey& bk,
                                std::vector<unsigned char>& brick) const
{
  return ExternalDataset::GetBrick(bk, brick);
}

}; //namespace tuvok
