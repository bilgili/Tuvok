#pragma once
#ifndef TUVOK_SHADER_DESCRIPTOR_H
#define TUVOK_SHADER_DESCRIPTOR_H

#include "StdTuvokDefines.h"
#if defined(_MSC_VER)
# include <memory>
#else
# include <tr1/memory>
#endif
#include <string>
#include <vector>

namespace tuvok {

/// A common way to identify a shader.
/// We need to know whether a shader is equivalent.  When we're asked to load a
/// new shader, we search through a list of cached ones and see if it already
/// exists.  This allows us to avoid loading it multiple times, and more
/// importantly compiling it multiple times.
/// This is made a bit difficult by how we build shaders: they can come from
/// files or strings.
class ShaderDescriptor {
  public:
    ShaderDescriptor();
    /// Constructs the descriptor from a list of FILES for each shader type.
    ShaderDescriptor(const std::vector<std::string>& vertex,
                     const std::vector<std::string>& fragment);
    ShaderDescriptor(const ShaderDescriptor&);
    /// Takes a list of directories and *two* lists of shaders.  Both lists
    /// must be terminated with a null.  The first is a list of filenames for
    /// the vertex shaders, the second is a list of filenames for fragment
    /// shaders.
    static ShaderDescriptor Create(
      std::vector<std::string> directories, ...
    );

    /// Adds a vertex shader in a string (i.e. not from a filename)
    void AddVertexShaderString(const std::string shader);
    /// Adds a fragment shader in a string (i.e. not from a filename)
    void AddFragmentShaderString(const std::string shader);

    /// Two shaders are equal if they utilize the same set of filenames/strings
    /// to compose the shader.
    bool operator ==(const ShaderDescriptor& sd) const;

    // temporary hack -- make an iterator instead
    std::vector<std::string> GetVertexShaders() const;
    // temporary hack -- make an iterator instead
    std::vector<std::string> GetFragmentShaders() const;

  private:
    struct sinfo;
    std::tr1::shared_ptr<sinfo> si;
};

}

#endif // TUVOK_SHADER_DESCRIPTOR_H

/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2012 Interactive Visualization and Data Analysis Group.

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