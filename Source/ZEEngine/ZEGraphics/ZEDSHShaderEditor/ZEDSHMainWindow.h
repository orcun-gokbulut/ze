//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSHMainWindow.h
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#pragma once

#include <QtGui/QMainWindow>

#include "ZEPointer/ZEPointer.h"

class Ui_ZEDSHMainWindow;
class ZEDSHOutputWindow;
class ZEDSHErrorsWindow;
class ZEDSHEditorWidget;

class ZEDSHMainWindow : public QMainWindow
{
	Q_OBJECT
	private:
		bool Loaded;
		bool Engine;
		bool Compiled;
		bool HasChanges;
		QString	FileName;

		ZEPointer<Ui_ZEDSHMainWindow> Form;
		ZEDSHEditorWidget* Editor;
		ZEDSHOutputWindow* OutputWindow;
		ZEDSHErrorsWindow* ErrorsWindow;


		void NewDocument();
		void OpenDocument(const QString& FileName);
		void SaveDocument(const QString& FileName);
		bool CloseDocument();

		void UpdateUI();

	private slots:
		void Editor_OnTextChanged();
		void Editor_OnSelectionChanged();

		void actNew_OnTrigger();
		void actOpen_OnTrigger();
		void actRecent_OnTrigger();
		void actSave_OnTrigger();
		void actSaveAs_OnTrigger();
		void actClose_OnTrigger();
		void actQuit_OnTrigger();
		void actUndo_OnTrigger();
		void actRedo_OnTrigger();
		void actSelectAll_OnTrigger();
		void actDeselect_OnTrigger();
		void actCut_OnTrigger();
		void actCopy_OnTrigger();
		void actPaste_OnTrigger();
		void actDelete_OnTrigger();
		void actFind_OnTrigger();
		void actReplace_OnTrigger();
		void actCompile_OnTrigger();
		void actPreference_OnTrigger();
		void actOutput_OnTrigger();
		void actError_OnTrigger();
		void actDefinitions_OnTrigger();
		void actCompileParameters_OnTrigger();
		void actReflection_OnTrigger();
		void actAbout_OnTrigger();


	public:
		ZEDSHMainWindow(QWidget* Parent = NULL);
};
