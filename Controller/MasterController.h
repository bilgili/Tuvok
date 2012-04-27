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

//!    File   : ImageVis3D.cpp
//!    Author : Jens Krueger
//!             SCI Institute
//!             University of Utah
//!    Date   : September 2008
//
//!    Copyright (C) 2008 SCI Institute
#pragma once

#ifndef MASTERCONTROLLER_H
#define MASTERCONTROLLER_H

#include "../StdTuvokDefines.h"
#ifdef _MSC_VER
# include <functional>
#else
# include <tr1/functional>
#endif
#include <tr1/memory>
#include <list>
#include <string>
#include <vector>

#include "Basics/CommandReturn.h"
#include "Basics/TuvokException.h"
#include "../DebugOut/MultiplexOut.h"
#include "../DebugOut/ConsoleOut.h"

#include "../Scripting/Scriptable.h"

class SystemInfo;
class IOManager;

namespace tuvok {

class AbstrRenderer;
class GPUMemMan;
class Scripting;
class LuaScripting;

typedef std::deque<AbstrRenderer*> AbstrRendererList;

/** \class MasterController
 * Centralized controller for Tuvok.
 *
 * MasterController is a router for all of the components of
 * Tuvok.  We try to keep modules communicating through this controller,
 * as opposed to directly with other modules.
 * You probably don't want to create an instance directly.  Use the singleton
 * provided by Controller::Instance(). */
class MasterController : public Scriptable {
public:
  enum EVolumeRendererType {
    OPENGL_SBVR = 0,
    OPENGL_RAYCASTER,
    DIRECTX_SBVR,
    DIRECTX_RAYCASTER,
    OPENGL_2DSBVR,
    DIRECTX_2DSBVR,
    OPENGL_TRAYCASTER,
    DIRECTX_TRAYCASTER,
    RENDERER_LAST,
  };

  /// Defaults to using a Console-based debug output stream.
  MasterController();
  virtual ~MasterController();

  /// Deallocate any memory we're holding; rendering or doing any real work is
  /// undefined after this operation.
  void Cleanup();

  /// Create a new renderer.
  AbstrRenderer* RequestNewVolumeRenderer(EVolumeRendererType eRendererType,
                                          bool bUseOnlyPowerOfTwo,
                                          bool bDownSampleTo8Bits,
                                          bool bDisableBorder,
                                          bool bNoRCClipplanes,
                                          bool bBiasAndScaleTF=false);

  /// Indicate that a renderer is no longer needed.
  void ReleaseVolumeRenderer(AbstrRenderer* pVolumeRenderer);

  /// Add another debug output
  /// \param debugOut      the new stream
  void AddDebugOut(AbstrDebugOut* debugOut);

  /// Removes the given debug output stream.
  /// The stream must be the currently connected/used one.
  void RemoveDebugOut(AbstrDebugOut* debugOut);

  /// Access the currently-active debug stream.
  ///@{
  AbstrDebugOut* DebugOut();
  const AbstrDebugOut *DebugOut() const;
  ///@}

  /// Returns the most recently registered renderer.
  const AbstrRenderer *Renderer() const {
    return m_vVolumeRenderer[m_vVolumeRenderer.size()-1];
  }

  /// The GPU memory manager moves data from CPU to GPU memory, and
  /// removes data from GPU memory.
  ///@{
  GPUMemMan*       MemMan()       { return m_pGPUMemMan; }
  const GPUMemMan* MemMan() const { return m_pGPUMemMan; }
  ///@}

  /// The IO manager is responsible for loading data into host memory.
  ///@{
  IOManager*       IOMan()       { return m_pIOManager;}
  const IOManager* IOMan() const { return m_pIOManager;}
  ///@}

  /// System information is for looking up host parameters, such as the
  /// amount of memory available.
  ///@{
  SystemInfo*       SysInfo()       { return m_pSystemInfo; }
  const SystemInfo* SysInfo() const { return m_pSystemInfo; }
  ///@}

  /// Lua scripting engine.
  ///@{
  std::tr1::shared_ptr<LuaScripting>  LuaScriptEngine()
      {return m_pLuaScript;}
  std::tr1::shared_ptr<LuaScripting>  LuaScriptEngine() const
      {return m_pLuaScript;}
  ///@}

  Scripting*       ScriptEngine()       { return m_pScriptEngine; }
  const Scripting* ScriptEngine() const { return m_pScriptEngine; }

  /// \todo this should return a pointer to memory.
  void Filter(std::string datasetName,
              uint32_t filter,
              void *var0 = 0, void *var1 = 0,
              void *var2 = 0, void *var3 = 0 );

  // Scriptable implementation
  virtual void RegisterCalls(Scripting* pScriptEngine);
  virtual bool Execute(const std::string& strCommand,
                       const std::vector< std::string >& strParams,
                       std::string& strMessage);

  /// Provenance recording.
  ///@{
  /// Callback function prototype.
  /// @param classification  type of event that occured
  /// @param command         command to use for IV3D's scripting.
  /// @param arguments       args for aforementioned command.  Might be empty.
  typedef void (provenance_func)(const std::string classification,
                                 const std::string command,
                                 const std::string arguments);
  /// Register new callback.  Overwrites the previous callback.
  void RegisterProvenanceCB(provenance_func *);
  /// Calls most recently registered provenance callback.
  void Provenance(const std::string, const std::string,
                  const std::string args = std::string());
  ///@}

  /// Whether or not to expose certain features which aren't actually ready for
  /// users.
  bool ExperimentalFeatures() const;
  void ExperimentalFeatures(bool b);

  /// Command execution.
  ///@{
  class FailedCommand: public Exception {};
  class CommandNotFound: public FailedCommand {};
  class ParseError: public FailedCommand {};
  class SemanticError: public FailedCommand {};
  /// Registers a new command.  The function is expected to take a string with
  /// the arguments and both parse and execute it.
  /// 'f' should throw 'ParseError' if the arguments do not parse.
  /// 'f' should throw 'SemanticError' if the command makes no sense for the
  /// current state of the system.
  typedef std::tr1::function<CommandReturn (AbstrRenderer*,
                                            std::vector<std::string> args)>
    command;
  void RegisterCommand(const std::string name, command& f);

  /// Takes a string for a command, parses it, and executes it.
  tuvok::CommandReturn Command(const std::string cmd) throw(FailedCommand);

  /// For convenience.  Lets you invoke an unregistered command.
  tuvok::CommandReturn Command(const command& f, const std::string args)
    throw(FailedCommand);
  ///@}

private:
  /// Initializer; add all our builtin commands.
  void RegisterInternalCommands();

private:
  SystemInfo*      m_pSystemInfo;
  GPUMemMan*       m_pGPUMemMan;
  IOManager*       m_pIOManager;
  MultiplexOut     m_DebugOut;
  ConsoleOut       m_DefaultOut;
  Scripting*       m_pScriptEngine;
  bool             m_bDeleteDebugOutOnExit;
  provenance_func* m_pProvenance;
  bool             m_bExperimentalFeatures;

  std::tr1::shared_ptr<LuaScripting>  m_pLuaScript;

  AbstrRendererList m_vVolumeRenderer;
  // The active renderer should point into a member of the renderer list.
  AbstrRenderer*   m_pActiveRenderer;
  struct TuvokCommand {
    std::string name;
    command cmd;
    bool operator<(const TuvokCommand& c) const { return name < c.name; }
    bool operator==(const TuvokCommand& c) const { return name == c.name; }
    bool operator==(const std::string & c) const { return name == c; }
  };
  std::list<TuvokCommand> m_Commands;
};

}
#endif // MASTERCONTROLLER_H
