#include "ImageVis3D.h"
#include <QTGui/QMdiSubWindow>

MainWindow::MainWindow(QWidget* parent /* = 0 */, Qt::WindowFlags flags /* = 0 */) : QMainWindow(parent, flags)
{
	setupUi(this);
	HideAllTools();
	HideAllFilters();
	HideVisualizationWindows();
}

MainWindow::~MainWindow()
{

}

void MainWindow::HideAllTools()
{
	polyline_tool_widget->hide();
	paintbrush_tool_widget->hide();
	resample_tool_widget->hide();
	crop_tool_widget->hide();
}

void MainWindow::HideAllFilters()
{
	Gaussian_filter_widget->hide();
	Median_filter_widget->hide();
	Histogram_filter_widget->hide();
}

void MainWindow::HideVisualizationWindows()
{
/*	widget_2By2->hide();
	widget_3By1->hide();
	widget_Single->hide();*/
}


void MainWindow::Show1DTransferFunction() {
	m_TransDialog1D.show();
}

void MainWindow::Show2DTransferFunction() {
	m_TransDialog2D.show();
}


void MainWindow::AddNewRenderWindow() {
     RenderWindow *renderWin = CreateNewRenderWindow();
     renderWin->show();
}

 RenderWindow* MainWindow::CreateNewRenderWindow()
 {
     RenderWindow *renderWin = new RenderWindow;
     mdiArea->addSubWindow(renderWin);

     return renderWin;
 }


void MainWindow::ToggleRenderWindowView1x3() {
	RenderWindow* win = GetActiveRenderWindow();
	if (win) win->ToggleRenderWindowView1x3();
}

void MainWindow::ToggleRenderWindowView2x2() {
	RenderWindow* win = GetActiveRenderWindow();
	if (win) win->ToggleRenderWindowView2x2();
}

void MainWindow::ToggleRenderWindowViewSingle() {
	RenderWindow* win = GetActiveRenderWindow();
	if (win) win->ToggleRenderWindowViewSingle();
}

 RenderWindow* MainWindow::GetActiveRenderWindow()
 {
     if (QMdiSubWindow* activeSubWindow = mdiArea->activeSubWindow())
		 return qobject_cast<RenderWindow*>(activeSubWindow->widget());
	 else
		return NULL;
 }