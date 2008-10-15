#pragma once

#ifndef HISTOGRAM2DDATABLOCK_H
#define HISTOGRAM2DDATABLOCK_H

#include "RasterDataBlock.h"

class Histogram2DDataBlock : public DataBlock
{
public:
	Histogram2DDataBlock();
  ~Histogram2DDataBlock();
	Histogram2DDataBlock(const Histogram2DDataBlock &other);
	Histogram2DDataBlock(LargeRAWFile* pStreamFile, UINT64 iOffset, bool bIsBigEndian);
	virtual Histogram2DDataBlock& operator=(const Histogram2DDataBlock& other);
	virtual UINT64 ComputeDataSize() const;

  bool Compute(RasterDataBlock* source);
  const std::vector< std::vector<UINT64> >& GetHistogram() {return m_vHistData;}
  float GetMaxGradMagnitude() const {return m_fMaxGradMagnitude;}

protected:
  float                              m_fMaxGradMagnitude;
  std::vector< std::vector<UINT64> > m_vHistData;

	virtual UINT64 GetHeaderFromFile(LargeRAWFile* pStreamFile, UINT64 iOffset, bool bIsBigEndian);
	virtual UINT64 CopyToFile(LargeRAWFile* pStreamFile, UINT64 iOffset, bool bIsBigEndian, bool bIsLastBlock);
	virtual UINT64 GetOffsetToNextBlock() const;

	virtual DataBlock* Clone();
};

#endif // HISTOGRAM2DDATABLOCK_H
