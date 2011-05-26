TEMPLATE          = app
CONFIG           += staticlib static create_prl warn_on stl exceptions
TARGET            = tuvok
p                 = . ../ ../../
p                += ../../Basics/3rdParty
p                += ../../IO/3rdParty
p                += ../../IO/3rdParty/boost
p                += ../../3rdParty/GLEW
DEPENDPATH        = $$p
INCLUDEPATH       = $$p
LIBPATH          += ../../Build ../../IO/expressions
QT               += opengl
LIBS             += -lTuvok -lz
unix:QMAKE_CXXFLAGS += -fno-strict-aliasing
unix:QMAKE_CFLAGS += -fno-strict-aliasing
unix:QMAKE_CFLAGS += -D_GLIBCXX_DEBUG -g
unix:QMAKE_CXXFLAGS += -D_GLIBCXX_DEBUG -g

# If this is a 10.5 machine, build for both x86 and x86_64.  Not
# the best idea (there's no guarantee the machine will have a
# 64bit compiler), but the best we can do via qmake.
macx {
    exists(/Developer/SDKs/MacOSX10.5.sdk/) {
        CONFIG += x86 x86_64
    }
}

### Should we link Qt statically or as a shared lib?
# Find the location of QtCore's prl file, and include it here so we can look at
# the QMAKE_PRL_CONFIG variable.
TEMP = $$[QT_INSTALL_LIBS] libQtCore.prl
PRL  = $$[QT_INSTALL_LIBS] QtCore.framework/QtCore.prl
exists($$TEMP) {
  include($$join(TEMP, "/"))
}
exists($$PRL) {
  include($$join(PRL, "/"))
}

# If that contains the `shared' configuration, the installed Qt is shared.
# In that case, disable the image plugins.
contains(QMAKE_PRL_CONFIG, shared) {
  message("Shared build, ensuring there will be image plugins linked in.")
  QTPLUGIN -= qgif qjpeg
} else {
  message("Static build, forcing image plugins to get loaded.")
  QTPLUGIN += qgif qjpeg
}

SOURCES += \
  ../context.cpp \
  render.cpp
