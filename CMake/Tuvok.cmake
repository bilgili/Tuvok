

set(TUVOK_REPO_TAG bbp)
set(TUVOK_NOTEST ON)
set(TUVOK_OPTIONAL ON)
set(TUVOK_DEPENDS OpenMP REQUIRED ZLIB Qt5Core Qt5OpenGL BISON FLEX OpenGL)
set(TUVOK_REPO_URL  https://github.com/BlueBrain/Tuvok.git)
set(TUVOK_DEB_DEPENDS libqt5opengl5-dev)


if(CI_BUILD_COMMIT)
  set(TUVOK_REPO_TAG ${CI_BUILD_COMMIT})
else()
  set(TUVOK_REPO_TAG master)
endif()
set(TUVOK_FORCE_BUILD ON)
set(TUVOK_SOURCE ${CMAKE_SOURCE_DIR})