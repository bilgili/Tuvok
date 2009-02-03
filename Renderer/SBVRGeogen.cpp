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
  \file    SBVRGeogen.cpp
  \author  Jens Krueger
           SCI Institute
           University of Utah
  \date    September 2008
*/

#include <algorithm>
#include <limits>
#include "SBVRGeogen.h"

#include <Basics/MathTools.h>

static bool EpsilonEqual(float a, float b);
static bool CheckOrdering(FLOATVECTOR3& a, FLOATVECTOR3& b, FLOATVECTOR3& c);
static void SortPoints(std::vector<POS3TEX3_VERTEX> &fArray);

SBVRGeogen::SBVRGeogen(void) :
  m_fSamplingModifier(1.0f),
  m_fMinZ(0),
  m_vAspect(1,1,1),
  m_vSize(1,1,1),
  m_vTexCoordMin(0,0,0),
  m_vTexCoordMax(1,1,1),
  m_iMinLayers(100),
  m_vGlobalAspect(1,1,1),
  m_vGlobalSize(1,1,1),
  m_vLODSize(1,1,1)
{
  m_pfBBOXStaticVertex[0] = FLOATVECTOR3(-0.5, 0.5,-0.5);
  m_pfBBOXStaticVertex[1] = FLOATVECTOR3( 0.5, 0.5,-0.5);
  m_pfBBOXStaticVertex[2] = FLOATVECTOR3( 0.5, 0.5, 0.5);
  m_pfBBOXStaticVertex[3] = FLOATVECTOR3(-0.5, 0.5, 0.5);
  m_pfBBOXStaticVertex[4] = FLOATVECTOR3(-0.5,-0.5,-0.5);
  m_pfBBOXStaticVertex[5] = FLOATVECTOR3( 0.5,-0.5,-0.5);
  m_pfBBOXStaticVertex[6] = FLOATVECTOR3( 0.5,-0.5, 0.5);
  m_pfBBOXStaticVertex[7] = FLOATVECTOR3(-0.5,-0.5, 0.5);

  m_pfBBOXVertex[0] = FLOATVECTOR3(0,0,0);
  m_pfBBOXVertex[1] = FLOATVECTOR3(0,0,0);
  m_pfBBOXVertex[2] = FLOATVECTOR3(0,0,0);
  m_pfBBOXVertex[3] = FLOATVECTOR3(0,0,0);
  m_pfBBOXVertex[4] = FLOATVECTOR3(0,0,0);
  m_pfBBOXVertex[5] = FLOATVECTOR3(0,0,0);
  m_pfBBOXVertex[6] = FLOATVECTOR3(0,0,0);
  m_pfBBOXVertex[7] = FLOATVECTOR3(0,0,0);
}

SBVRGeogen::~SBVRGeogen(void)
{
}

void SBVRGeogen::SetTransformation(const FLOATMATRIX4& matTransform, bool bForceUpdate) {
  if (bForceUpdate || m_matTransform != matTransform)  {
    m_matTransform = matTransform;
    InitBBOX();
    ComputeGeometry();
  }
}

// Finds the point with the minimum position in Z.
struct vertex_min_z : public std::binary_function<POS3TEX3_VERTEX,
                                                  POS3TEX3_VERTEX,
                                                  bool> {
  bool operator()(const POS3TEX3_VERTEX &a, const POS3TEX3_VERTEX &b) const {
    return (a.m_vPos.z < b.m_vPos.z);
  }
};

void SBVRGeogen::InitBBOX() {

  FLOATVECTOR3 vVertexScale(m_vAspect);

  m_pfBBOXVertex[0] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[0]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMin.x,m_vTexCoordMax.y,m_vTexCoordMin.z));
  m_pfBBOXVertex[1] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[1]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMax.x,m_vTexCoordMax.y,m_vTexCoordMin.z));
  m_pfBBOXVertex[2] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[2]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMax.x,m_vTexCoordMax.y,m_vTexCoordMax.z));
  m_pfBBOXVertex[3] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[3]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMin.x,m_vTexCoordMax.y,m_vTexCoordMax.z));
  m_pfBBOXVertex[4] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[4]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMin.x,m_vTexCoordMin.y,m_vTexCoordMin.z));
  m_pfBBOXVertex[5] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[5]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMax.x,m_vTexCoordMin.y,m_vTexCoordMin.z));
  m_pfBBOXVertex[6] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[6]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMax.x,m_vTexCoordMin.y,m_vTexCoordMax.z));
  m_pfBBOXVertex[7] = POS3TEX3_VERTEX(FLOATVECTOR4(m_pfBBOXStaticVertex[7]*vVertexScale,1.0f) * m_matTransform, FLOATVECTOR3(m_vTexCoordMin.x,m_vTexCoordMin.y,m_vTexCoordMax.z));

  // find the minimum z value
  m_fMinZ = (*std::min_element(m_pfBBOXVertex, m_pfBBOXVertex+8,
                               vertex_min_z())).m_vPos.z;
}

static bool ComputeIntersection(float z,
                                const POS3TEX3_VERTEX &plA,
                                const POS3TEX3_VERTEX &plB,
                                std::vector<POS3TEX3_VERTEX>& vHits)
{
  /*
     returns NO INTERSECTION if the line of the 2 points a,b is
     1. in front of the intersection plane
     2. behind the intersection plane
     3. parallel to the intersection plane (both points have "pretty much" the same z)
  */
  if ((z > plA.m_vPos.z && z > plB.m_vPos.z) ||
      (z < plA.m_vPos.z && z < plB.m_vPos.z) ||
      (EpsilonEqual(plA.m_vPos.z, plB.m_vPos.z))) {
    return false;
  }

  float fAlpha = (z - plA.m_vPos.z) /
                 (plA.m_vPos.z - plB.m_vPos.z);

  POS3TEX3_VERTEX vHit;

  vHit.m_vPos.x = plA.m_vPos.x + (plA.m_vPos.x - plB.m_vPos.x) * fAlpha;
  vHit.m_vPos.y = plA.m_vPos.y + (plA.m_vPos.y - plB.m_vPos.y) * fAlpha;
  vHit.m_vPos.z = z;

  vHit.m_vTex = plA.m_vTex + (plA.m_vTex - plB.m_vTex) * fAlpha;

  vHits.push_back(vHit);

  return true;
}

// Functor to identify the point with the lowest `y' coordinate.
struct vertex_min : public std::binary_function<POS3TEX3_VERTEX,
                                                POS3TEX3_VERTEX,
                                                bool> {
  bool operator()(const POS3TEX3_VERTEX &a, const POS3TEX3_VERTEX &b) const {
    return (a.m_vPos.y < b.m_vPos.y);
  }
};

void SBVRGeogen::Triangulate(std::vector<POS3TEX3_VERTEX> &fArray) {
  // move bottom element to front of array
  std::swap(fArray[0],
            *std::min_element(fArray.begin(), fArray.end(), vertex_min()));
  // sort points according to gradient
  SortPoints(fArray);

  // convert to triangles
  for (UINT32 i=0; i<(fArray.size()-2) ; i++) {
    m_vSliceTriangles.push_back(fArray[0]);
    m_vSliceTriangles.push_back(fArray[i+1]);
    m_vSliceTriangles.push_back(fArray[i+2]);
  }
}


bool SBVRGeogen::ComputeLayerGeometry(float fDepth) {
  std::vector<POS3TEX3_VERTEX> vLayerPoints;
  vLayerPoints.reserve(12);

  ComputeIntersection(fDepth, m_pfBBOXVertex[0], m_pfBBOXVertex[1],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[1], m_pfBBOXVertex[2],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[2], m_pfBBOXVertex[3],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[3], m_pfBBOXVertex[0],
                      vLayerPoints);

  ComputeIntersection(fDepth, m_pfBBOXVertex[4], m_pfBBOXVertex[5],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[5], m_pfBBOXVertex[6],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[6], m_pfBBOXVertex[7],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[7], m_pfBBOXVertex[4],
                      vLayerPoints);

  ComputeIntersection(fDepth, m_pfBBOXVertex[4], m_pfBBOXVertex[0],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[5], m_pfBBOXVertex[1],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[6], m_pfBBOXVertex[2],
                      vLayerPoints);
  ComputeIntersection(fDepth, m_pfBBOXVertex[7], m_pfBBOXVertex[3],
                      vLayerPoints);

  if (vLayerPoints.size() <= 2) {
    return false;
  }

  Triangulate(vLayerPoints);
  return true;
}

float SBVRGeogen::GetLayerDistance() {
  return (0.5f * 1.0f/m_fSamplingModifier * (m_vAspect/FLOATVECTOR3(m_vSize))).minVal(); //float(m_vAspect.minVal())/float(std::max(m_vSize.maxVal(),m_iMinLayers));
}


float SBVRGeogen::GetOpacityCorrection() {
  return 1.0f/m_fSamplingModifier * (FLOATVECTOR3(m_vGlobalSize)/FLOATVECTOR3(m_vLODSize)).maxVal();//  GetLayerDistance()*m_vSize.maxVal();
}

void SBVRGeogen::ComputeGeometry() {
  m_vSliceTriangles.clear();

  float fDepth = m_fMinZ;
  float fLayerDistance = GetLayerDistance();

  while (ComputeLayerGeometry(fDepth)) fDepth += fLayerDistance;
}

void SBVRGeogen::SetVolumeData(const FLOATVECTOR3& vAspect, const UINTVECTOR3& vSize) {
  m_vGlobalAspect = vAspect;
  m_vGlobalSize = vSize;
}

void SBVRGeogen::SetLODData(const UINTVECTOR3& vSize) {
  m_vLODSize = vSize;
}

void SBVRGeogen::SetBrickData(const FLOATVECTOR3& vAspect,
                              const UINTVECTOR3& vSize,
                              const FLOATVECTOR3& vTexCoordMin,
                              const FLOATVECTOR3& vTexCoordMax) {
  m_vAspect       = vAspect;
  m_vSize         = vSize;
  m_vTexCoordMin  = vTexCoordMin;
  m_vTexCoordMax  = vTexCoordMax;
  InitBBOX();
}

static bool EpsilonEqual(float a, float b) {
  return fabs(a-b) <= std::numeric_limits<float>::epsilon();
}

static bool CheckOrdering(FLOATVECTOR3& a, FLOATVECTOR3& b, FLOATVECTOR3& c) {
  float g1 = (a[1]-c[1])/(a[0]-c[0]),
        g2 = (b[1]-c[1])/(b[0]-c[0]);

  if (EpsilonEqual(a[0],c[0])) return (g2 < 0) || (EpsilonEqual(g2,0) && b[0] < c[0]);
  if (EpsilonEqual(b[0],c[0])) return (g1 > 0) || (EpsilonEqual(g1,0) && a[0] > c[0]);

  if (a[0] < c[0])
    if (b[0] < c[0]) return g1 < g2; else return false;
  else
    if (b[0] < c[0]) return true; else return g1 < g2;
}

/// @todo: should be replaced with std::sort.
static void SortPoints(std::vector<POS3TEX3_VERTEX> &fArray) {
  // for small arrays, this bubble sort actually beats qsort.
  for (UINT32 i= 1;i<fArray.size();++i)
    for (UINT32 j = 1;j<fArray.size()-i;++j)
      if (!CheckOrdering(fArray[j].m_vPos,fArray[j+1].m_vPos,fArray[0].m_vPos))
        std::swap(fArray[j], fArray[j+1]);
}
