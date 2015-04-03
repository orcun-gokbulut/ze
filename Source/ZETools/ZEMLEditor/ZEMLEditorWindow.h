//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLEditorWindow.h
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
#ifndef	__ZEML_EDITOR_WINDOW_H__
#define __ZEML_EDITOR_WINDOW_H__

#include <QtGui/QMainWindow>
#include "QtGui/qtreewidget.h"

#include "ZEDS/ZEString.h"
#include "ZEML/ZEMLRoot.h"

class Ui_ZEMLEditorWindow;

class ZEMLEditorWindow : public QMainWindow
{
	Q_OBJECT
	private:
		static ZEMLEditorWindow* Instance;
		Ui_ZEMLEditorWindow*	Form;
		ZEString				FileName;
		ZEMLRoot				Root;
		ZEMLNode*				RootNode;

		void					LoadNode(QTreeWidgetItem* Item, ZEMLNode* Node);
		void					LoadTree();
		
		void					RegisterRecentFile(const ZEString& FileName);
		void					LoadRecentFiles();

		void					OpenFile(const ZEString& FileName);
		void					SaveFile(const ZEString& FileName);

		void					ConfigureUI();

	private slots:
		void					NameChanged(ZEMLElement* Element, const ZEString& NewName, const ZEString& OldName);
		void					ValueChanged(ZEMLProperty* Property, const ZEValue& NewValue, const ZEValue& OldValue);
		//void					DataChange(ZEMLData* Data, void* NewData, ZESize NewDataSize, void* OldData, ZESize OldDataSize);
		
		void					Select();
		void					Deselect();

		void					New();
		void					Open();
		void					OpenRecentFile();
		void					Save();
		void					SaveAs();
		void					Close();
		void					Quit();

		void					Undo();
		void					Redo();
		/*void					Cut();
		void					Copy();
		void					Paste();*/

		void					AddNewNode();
		void					AddNewProperty();
		void					AddNewData();
		void					Delete();

		void					UserGuide();
		void					BugReport();
		void					Website();
		void					About();

	public:
		Ui_ZEMLEditorWindow*	 GetForm();
		void					 Update();

								ZEMLEditorWindow();
								~ZEMLEditorWindow();

		static ZEMLEditorWindow* GetInstance();
};

#endif
