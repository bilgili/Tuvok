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


//!    File   : SettingsDlg.h
//!    Author : Jens Krueger
//!             SCI Institute
//!             University of Utah
//!    Date   : July 2008
//
//!    Copyright (C) 2008 SCI Institute

#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <Controller/MasterController.h>
#include <UI/AutoGen/ui_SettingsDlg.h>

class SettingsDlg : public QDialog, protected Ui_SettingsDlg
{
  Q_OBJECT
  public:
    SettingsDlg(MasterController& masterController, QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~SettingsDlg();

    UINT64        GetGPUMem();
    UINT64        GetCPUMem();
    bool          GetQuickopen();
    unsigned int  GetMinFramerate();
    unsigned int  GetLODDelay();
    unsigned int  GetActiveTS();
    unsigned int  GetInactiveTS();

    FLOATVECTOR3  GetBackgroundColor1();
    FLOATVECTOR3  GetBackgroundColor2();
    FLOATVECTOR4  GetTextColor();
    unsigned int  GetBlendPrecisionMode();

    void Data2Form(UINT64 iMaxCPU, UINT64 iMaxGPU, 
                   bool bQuickopen, unsigned int iMinFramerate, unsigned int iLODDelay, unsigned int iActiveTS, unsigned int iInactiveTS, 
                   unsigned int iBlendPrecision, const FLOATVECTOR3& vBackColor1, const FLOATVECTOR3& vBackColor2, const FLOATVECTOR4& vTextColor);

  protected slots:
    void SelectTextColor();
    void SetTextOpacity(int iOpacity);
    void SelectBackColor1();
    void SelectBackColor2();
    void SetMaxMemCheck();    
    void LODDelayChanged();
    void MinFramerateChanged();
    void ActTSChanged();
    void InactTSChanged();

  private:
    MasterController& m_MasterController;
    QColor            m_cBackColor1;
    QColor            m_cBackColor2;
    QColor            m_cTextColor;

    int               m_InitialGPUMemMax;
    void setupUi(QDialog *SettingsDlg);

};

#endif // SETTINGSDLG_H
