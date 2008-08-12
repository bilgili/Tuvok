#include "GPUSBVR.h"

#include <math.h>
#include <Basics/SysTools.h>
#include "../Controller/MasterController.h"

GPUSBVR::GPUSBVR(MasterController* pMasterController) :
	m_iCurrentView(0),
	m_xRot(0),
	m_bDelayedCompleteRedraw(false)
{
	m_pMasterController = pMasterController;
}

GPUSBVR::~GPUSBVR() {
}


void GPUSBVR::Initialize() {

	m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","");

	glClearColor(1,0,0,0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	
	m_IDTex[0] = m_pMasterController->MemMan()->Load2DTextureFromFile(SysTools::GetFromResourceOnMac("RenderWin1x3.bmp").c_str());
	if (m_IDTex[0] == NULL) {
		m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","First Image load failed");		
	}
	m_IDTex[1] = m_pMasterController->MemMan()->Load2DTextureFromFile(SysTools::GetFromResourceOnMac("RenderWin2x2.bmp").c_str());
	if (m_IDTex[1] == NULL) {
		m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","Second Image load failed");		
	}
	m_IDTex[2] = m_pMasterController->MemMan()->Load2DTextureFromFile(SysTools::GetFromResourceOnMac("RenderWin1.bmp").c_str());
	if (m_IDTex[2] == NULL) {
		m_pMasterController->DebugOut()->Message("GPUSBVR::Initialize","Third Image load failed");		
	}
}

void GPUSBVR::Paint() {
	if (m_bCompleteRedraw) {
		m_pMasterController->DebugOut()->Message("GPUSBVR::Paint","Complete Redraw");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslated(0.0, 0.0, -10.0);
		glRotated(m_xRot / 16.0, 1.0, 0.0, 0.0);

		m_IDTex[m_iCurrentView]->Bind();

		// just do some stuff to simulate a slow draw call
		for (unsigned int i = 0;i<10000;i++) {
			glBegin(GL_QUADS);
				glColor4d(1,1,1,1);
				glTexCoord2d(0,0);
				glVertex3d(-0.5,  0.5, 0.0);
				glTexCoord2d(1,0);
				glVertex3d( 0.5,  0.5, 0.0);
				glTexCoord2d(1,1);
				glVertex3d( 0.5, -0.5, 0.0);
				glTexCoord2d(0,1);
				glVertex3d(-0.5, -0.5, 0.0);
			glEnd();
		}
	} else {
		m_pMasterController->DebugOut()->Message("GPUSBVR::Paint","Quick Redraw");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslated(0.0, 0.0, -10.0);
		glRotated(m_xRot / 16.0, 1.0, 0.0, 0.0);

		m_IDTex[m_iCurrentView]->Bind();

		glBegin(GL_QUADS);
			glColor4d(1,1,1,1);
			glTexCoord2d(0,0);
			glVertex3d(-0.5,  0.5, 0.0);
			glTexCoord2d(1,0);
			glVertex3d( 0.5,  0.5, 0.0);
			glTexCoord2d(1,1);
			glVertex3d( 0.5, -0.5, 0.0);
			glTexCoord2d(0,1);
			glVertex3d(-0.5, -0.5, 0.0);
		glEnd();
	}

	m_bCompleteRedraw = false;
	m_bRedraw = false;
}

void GPUSBVR::Resize(int width, int height) {
	m_pMasterController->DebugOut()->Message("GPUSBVR::Resize","");

	int side = std::min(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
	glMatrixMode(GL_MODELVIEW);

	m_bDelayedCompleteRedraw = true;
}

void GPUSBVR::Cleanup() {
	m_pMasterController->MemMan()->FreeTexture(m_IDTex[0]);
	m_pMasterController->MemMan()->FreeTexture(m_IDTex[1]);
	m_pMasterController->MemMan()->FreeTexture(m_IDTex[2]);
}


void GPUSBVR::Set1DTrans(TransferFunction1D* p1DTrans) {
	AbstrRenderer::Set1DTrans(p1DTrans);
	m_bRedraw = true;
	m_bCompleteRedraw = true;
}

void GPUSBVR::Set2DTrans(TransferFunction2D* p2DTrans) {
	AbstrRenderer::Set2DTrans(p2DTrans);
	m_bRedraw = true;
	m_bCompleteRedraw = true;
}

void GPUSBVR::Changed1DTrans() {
	m_bRedraw = true;
	m_bCompleteRedraw = true;
}

void GPUSBVR::Changed2DTrans() {
	m_bRedraw = true;
	m_bCompleteRedraw = true;
}


void GPUSBVR::CheckForRedraw() {
	if (m_bDelayedCompleteRedraw) {
		m_bDelayedCompleteRedraw = false;
		m_bCompleteRedraw = true;
		m_bRedraw = true;
	}

	if (m_bRedraw) Paint();
}