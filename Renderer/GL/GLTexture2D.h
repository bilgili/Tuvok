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
  \file    GLTexture2D.h
  \author  Jens Krueger
           SCI Institute
           University of Utah
  \date    August 2008
*/
#pragma once

#ifndef GLTEXTURE2D_H
#define GLTEXTURE2D_H

#include "../../StdTuvokDefines.h"
#include <memory>
#include "GLTexture.h"
#include "../../Basics/Vectors.h"

namespace tuvok {

class GLTexture2D : public GLTexture {
  public:
    GLTexture2D(uint32_t iSizeX, uint32_t iSizeY, GLint internalformat,
                GLenum format, GLenum type, const GLvoid *pixels = 0,
                GLint iMagFilter = GL_NEAREST, GLint iMinFilter = GL_NEAREST,
                GLint wrapX = GL_CLAMP_TO_EDGE, GLint wrapY = GL_CLAMP_TO_EDGE
    );
    virtual ~GLTexture2D() {}

    virtual void Bind(uint32_t iUnit=0) const {
      GLint iPrevUint;
      GL(glGetIntegerv(GL_ACTIVE_TEXTURE, &iPrevUint));

      GL(glActiveTexture(GLenum(GL_TEXTURE0 + iUnit)));
      GL(glBindTexture(GL_TEXTURE_2D, m_iGLID));

      GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_iMagFilter));
      GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_iMinFilter));

      GL(glActiveTexture(iPrevUint));
    }

    virtual void SetData(const void *pixels, bool bRestoreBinding=true);
    void SetData(const UINTVECTOR2& offset, const UINTVECTOR2& size,
                 const void *pixels, bool bRestoreBinding=true);

    virtual std::shared_ptr<void> GetData();

    virtual uint64_t GetCPUSize() const {
      return uint64_t(m_iSizeX*m_iSizeY*SizePerElement());
    }
    virtual uint64_t GetGPUSize() const {
      return uint64_t(m_iSizeX*m_iSizeY*SizePerElement());
    }

    UINTVECTOR2 GetSize() const {
      return UINTVECTOR2(uint32_t(m_iSizeX), uint32_t(m_iSizeY));
    }

  protected:
    GLuint m_iSizeX;
    GLuint m_iSizeY;
};

}
#endif // GLTEXTURE2D_H
