//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMainEditor.h
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

#include "ZEDCore/ZEDCore.h"
#include <QtGui/QMainWindow>

class ZEDCore;

namespace Ui
{
	class MainEditor;
}

class ZEDMainEditor : public QMainWindow
{
	Q_OBJECT
	private:
		Ui::MainEditor* ui;
		ZEDCore* Core;

		bool InitializeSelf();
		bool DeinitializeSelf();

	private slots:
		//File Menu Actions
		void actNew_onTriggered();
		void actOpen_onTriggered();
		void actClose_onTriggered();
		void actSave_onTriggered();
		void actSaveAs_onTriggered();
		void actExit_onTriggered();
		//Edit Menu Actions
// 		void actUndo_onTriggered();
// 		void actRedo_onTriggered();
// 		void actClone_onTriggered();
// 		void actDelete_onTriggered();
		//Operations Menu Actions
// 		void actSelect_onTriggered();
// 		void actMove_onTriggered();
// 		void actRotate_onTriggered();
// 		void actScale_onTriggered();
// 		void actHide_onTriggered();
// 		void actUnhide_onTriggered();
// 		void actFreeze_onTriggered();
// 		void actUnfreeze_onTriggered();
// 		void actGoToEntity_onTriggered();


		
	public:
		bool Initialize();
		bool Deinitalize();

		ZEDMainEditor(QWidget* Parent = 0, Qt::WindowFlags Flags = 0);
		~ZEDMainEditor();
};
