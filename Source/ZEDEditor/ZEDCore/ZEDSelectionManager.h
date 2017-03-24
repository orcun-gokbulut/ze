//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionManager.h
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

#include "ZEDComponent.h"

#include "ZEDS/ZEArray.h"
#include "ZEDUserInterface/ZEDCommand.h"

class ZEObject;
class ZEClass;
class ZEDEditor;
class ZEDObjectWrapper;

class ZEDSelectionManager : public ZEDComponent
{
	ZE_OBJECT
	friend class ZEDEditorCore;
	private:
		ZEArray<ZEDObjectWrapper*>			Selection;
		ZEArray<ZEDObjectWrapper*>			FrozenObjects;
		ZEDObjectWrapper*					FocusedObject;
		ZEClass*							Filter;
		bool								LockSelection;

		bool								FilterSelection(ZEObject* Object, void* Class);
		void								UnfrezeeAllInternal(ZEDObjectWrapper* Object);

		virtual bool						InitializeInternal();
		virtual bool						DeinitializeInternal();

		virtual void						Editor_OnClosed(ZEDEditor* Editor);

											ZEDSelectionManager();
		virtual								~ZEDSelectionManager();

	private: /* COMMANDS */
		ZEDCommand							SelectAllCommand;
		ZEDCommand							ClearSelectionCommand;
		ZEDCommand							LockSelectionCommand;
		ZEDCommand							FreezeObjectsCommand;
		ZEDCommand							UnfreezeObjectsCommand;

		void								RegisterCommands();
		void								UpdateCommands();

		void								SelectAllCommand_OnAction(const ZEDCommand* Command);
		void								ClearSelectionCommand_OnAction(const ZEDCommand* Command);
		void								LockSelectionCommand_OnAction(const ZEDCommand* Command);
		void								FreezeObjectsCommand_OnAction(const ZEDCommand* Command);
		void								UnfreezeObjectsCommand_OnAction(const ZEDCommand* Command);

	public:
		void								SetSelectionFilter(ZEClass* Class);
		ZEClass*							GetSelectionFilter();
		
		void								SetLockSelection(bool Lock);
		bool								GetLockSelection();

		void								SetSelection(const ZEArray<ZEDObjectWrapper*>& Selection);
		const ZEArray<ZEDObjectWrapper*>&	GetSelection();

		ZEDObjectWrapper*					GetFocusedObject();

		void								SelectObject(ZEDObjectWrapper* Object);
		void								SelectObjects(const ZEArray<ZEDObjectWrapper*>& Objects);
		void								DeselectObject(ZEDObjectWrapper* Object);
		void								DeselectObjects(const ZEArray<ZEDObjectWrapper*>& Objects);
		void								ClearSelection();

		void								FocusObject(ZEDObjectWrapper* Object);
		void								ClearFocus();

		const ZEArray<ZEDObjectWrapper*>	GetFrozenObjects();
		void								FreezeObject(ZEDObjectWrapper* Object);
		void								FreezeObjects(const ZEArray<ZEDObjectWrapper*>& Objects);
		void								UnfreezeObject(ZEDObjectWrapper* Object);
		void								UnfreezeObjects(const ZEArray<ZEDObjectWrapper*>& Objects);

		ZE_EVENT(							OnStateChanged,(ZEDSelectionManager* Manager));
		ZE_EVENT(							OnSelectionChanged,(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& Selection));
		ZE_EVENT(							OnSelectionLocked,(ZEDSelectionManager* Manager));
		ZE_EVENT(							OnSelectionUnlocked,(ZEDSelectionManager* Manager));
		ZE_EVENT(							OnObjectsSelected,(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& SelectedObjects));
		ZE_EVENT(							OnObjectsDeselected,(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& DeselectedObjects));
		ZE_EVENT(							OnObjectsFrozen,(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& FrozenObjects));
		ZE_EVENT(							OnObjectsUnfrozen,(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& UnfrozenObjects));
		ZE_EVENT(							OnFocusChanged,(ZEDSelectionManager* Manager, ZEDObjectWrapper* FocusedObject));

		static ZEDSelectionManager*			CreateInstance();
};
