
set(TUVOK_VERSION   3.1.0)
set(TUVOK_REPO_TAG  bbp)
set(TUVOK_NOTEST    ON)
set(TUVOK_OPTIONAL  ON)
set(TUVOK_DEPENDS   OpenMP REQUIRED ZLIB Qt4 BISON FLEX OpenGL )
set(TUVOK_QT4_COMPONENTS "QtCore QtGui QtXml QtOpenGL")

if(CXX_SHAREDPTR_SUPPORTED AND CXX_TUPLE_SUPPORTED AND CXX_AUTO_SUPPORTED AND
    CXX_NULLPTR_SUPPORTED AND CXX_ARRAY_SUPPORTED)
   set(TUVOK_REPO_URL  https://github.com/BlueBrain/Tuvok.git)
else()
  message(STATUS "Skip Tuvok: missing C++11 features")
endif()

if(CI_BUILD_COMMIT)
  set(TUVOK_REPO_TAG ${CI_BUILD_COMMIT})
else()
  set(TUVOK_REPO_TAG master)
endif()
set(TUVOK_FORCE_BUILD ON)
set(TUVOK_SOURCE ${CMAKE_SOURCE_DIR})