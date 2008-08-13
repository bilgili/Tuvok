#pragma once

#ifndef Q2DTRANSFERFUNCTION
#define Q2DTRANSFERFUNCTION

#include <QtGui/QWidget>
#include <IO/TransferFunction1D.h>
#include <IO/TransferFunction2D.h>

#define Q2DT_PAINT_NONE  0
#define Q2DT_PAINT_RED   1
#define Q2DT_PAINT_GREEN 2
#define Q2DT_PAINT_BLUE  4
#define Q2DT_PAINT_ALPHA 8
#define Q2DT_PAINT_UNDEF 16

class MasterController;

class Q2DTransferFunction : public QWidget
{
	Q_OBJECT

	public:
		Q2DTransferFunction(MasterController& masterController, QWidget *parent=0);
		virtual ~Q2DTransferFunction(void);
		void SetData(const Histogram2D* vHistrogram, TransferFunction2D* pTrans);
		void SetPaintmode(unsigned int iPaintmode) {if (iPaintmode < Q2DT_PAINT_UNDEF) m_iPaintmode = iPaintmode;};
		void Set1DTrans(const TransferFunction1D* p1DTrans);

		QSize minimumSizeHint() const;
		QSize sizeHint() const;

		int GetActiveSwatchIndex() {return m_iActiveSwatchIndex;}
		size_t GetSwatchCount() {return m_pTrans->m_Swatches.size();}
		size_t GetSwatchSize(unsigned int i) {return m_pTrans->m_Swatches[i].pPoints.size();}

		size_t GetGradientCount() {return m_pTrans->m_Swatches[m_iActiveSwatchIndex].pGradientStops.size();}
		GradientStop GetGradient(unsigned int i) {return m_pTrans->m_Swatches[m_iActiveSwatchIndex].pGradientStops[i];}
		void AddGradient(GradientStop stop);
		void DeleteGradient(unsigned int i);
		void SetGradient(unsigned int i, GradientStop stop);

	public slots:
		void SetActiveSwatch(const int iActiveSwatch);
		void AddSwatch();
		void DeleteSwatch();
		void UpSwatch();
		void DownSwatch();

		bool LoadFromFile(const QString& strFilename);
		bool SaveToFile(const QString& strFilename);

	signals:
		void SwatchChange();
	
	protected:
		virtual void paintEvent(QPaintEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void changeEvent(QEvent * event);

	private:
		// states
		NormalizedHistogram2D	m_vHistrogram;
		TransferFunction2D*		m_pTrans;
		unsigned int			m_iPaintmode;
		int						m_iActiveSwatchIndex;
		MasterController&		m_MasterController;

		// cached image of the backdrop
		bool		 m_bBackdropCacheUptodate;
		unsigned int m_iCachedHeight;
		unsigned int m_iCachedWidth;
		QPixmap*	 m_pBackdropCache;

		// border size, may be changed in the constructor 
		unsigned int m_iBorderSize;
		unsigned int m_iSwatchBorderSize;

		// colors, may be changed in the setcolor routine
		QColor m_colorHistogram;
		QColor m_colorBack;
		QColor m_colorBorder;
		QColor m_colorSwatchBorder;
		QColor m_colorSwatchBorderCircle;
		QColor m_colorSwatchGradCircle;
		QColor m_colorSwatchBorderCircleSel;
		QColor m_colorSwatchGradCircleSel;

		void SetColor(bool bIsEnabled);

		// mouse motion handling
		int m_iPointSelIndex;
		int m_iGradSelIndex;
		INTVECTOR2 m_vMousePressPos;
		bool m_bDragging;

		// drawing routines
		void DrawBorder(QPainter& painter);
		void DrawHistogram(QPainter& painter, float fScale=1.0f);
		void DrawSwatches(QPainter& painter);
		void Draw1DTrans(QPainter& painter);

		// helper
		INTVECTOR2   Rel2Abs(FLOATVECTOR2 vfCoord);
		FLOATVECTOR2 Abs2Rel(INTVECTOR2   vCoord);
};


#endif // Q2DTRANSFERFUNCTION
