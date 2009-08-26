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
#pragma once

#ifndef TUVOK_UNBRICKED_DS_METADATA_H
#define TUVOK_UNBRICKED_DS_METADATA_H

#include "ExternalMetadata.h"
#include "Controller/Controller.h"

namespace tuvok {

/** UnbrickedDSMetadata presents a simplified view of external data.  The
 * volumes do not have any LODs, and consist entirely of one brick. */
class UnbrickedDSMetadata : public ExternalMetadata {
public:
  UnbrickedDSMetadata();
  virtual ~UnbrickedDSMetadata() {}

  UINTVECTOR3 GetMaxBrickSize() const;
  UINTVECTOR3 GetBrickOverlapSize() const;
};

}; //namespace tuvok

#endif // TUVOK_UNBRICKED_DS_METADATA_H
