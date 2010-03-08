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
  \file    DSFactory.h
  \author  Tom Fogal
           SCI Institute
           University of Utah
  \brief   Exceptions for use in the IO subsystem
*/
#pragma once
#ifndef TUVOK_IO_ERROR_H
#define TUVOK_IO_ERROR_H

#include "StdTuvokDefines.h"
#include <stdexcept>

namespace tuvok {
namespace io {

/// Generic base for a failed open
class DSOpenFailed : virtual public std::runtime_error {
  public:
    explicit DSOpenFailed(const char* s, const char* where=NULL,
                          size_t ln=0) : std::runtime_error(s), msg(s),
                                         location(where), line(ln) {}
    virtual ~DSOpenFailed() throw() {}

    virtual const char* what() const throw() { return this->msg; }
    const char* where() const { return this->location; }
    size_t lineno() const { return this->line; }

  private:
    const char* msg;
    const char* location;
    const size_t line;
};

/// any kind of dataset verification failed; e.g. a checksum for the
/// file was invalid.
class DSVerificationFailed : virtual public DSOpenFailed {
  public:
    explicit DSVerificationFailed(const char* s, const char* where=NULL,
                                  size_t ln=0)
                                  : std::runtime_error(s),
                                    DSOpenFailed(s, where, ln) {}
};

/// Oversized bricks; needs re-bricking
class DSBricksOversized : virtual public DSOpenFailed {
  public:
    explicit DSBricksOversized(const char* s,
                               size_t bsz,
                               const char* where=NULL,
                               size_t ln=0)
                               : std::runtime_error(s),
                                 DSOpenFailed(s, where, ln),
                                 brick_size(bsz) {}
    size_t BrickSize() const { return this->brick_size; }

  private:
    size_t brick_size;
};

} // io
} // tuvok

#endif // TUVOK_IO_ERROR_H
