//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectManager.h
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

#include "ZEDComponent.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDUserInterface/ZEDCommand.h"

class ZEClass;
class ZEMTProperty;
class ZEDObjectWrapper;
class ZEDSelectionManager;
enum ZEDEditorState;

class ZEDWrapperRegistration
{
	public:
		ZEClass*								WrapperClass;
		ZEClass*								TargetClass;
};

class ZEDObjectManager : public ZEDComponent
{
	ZE_OBJECT
	friend class ZEDObjectWrapper;
	private:
		ZEArray<ZEDWrapperRegistration>			WrapperClasses;
		ZEDObjectWrapper*						RootWrapper;

		void									LoadWrapperClasses();

		bool									InitializeInternal();
		bool									DeinitializeInternal();

		virtual void							Editor_OnStateChanged(ZEDEditor* Editor, ZEDEditorState State);
		virtual void							SelectionManager_OnSelectionChanged(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& Selection);

												ZEDObjectManager();
		virtual									~ZEDObjectManager();

	private: /* COMMANDS */
		ZEDCommand								CloneCommand;
		ZEDCommand								DeleteCommand;

		void									RegisterCommands();
		void									UpdateCommands();

		void									CloneCommand_OnAction(const ZEDCommand* Command);
		void									DeleteCommand_OnAction(const ZEDCommand* Command);

	public:
		const ZEArray<ZEDWrapperRegistration>&	GetWrapperClasses();
		void									RegisterWrapperClass(ZEClass* Class);
		void									UnregisterWrapperClass(ZEClass* Class);

		void									SetRootWrapper(ZEDObjectWrapper* Wrapper);
		ZEDObjectWrapper*						GetRootWrapper();

		ZEClass*								FindWrapperClass(ZEClass* ObjectClass);
		ZEDObjectWrapper*						FindWrapper(ZEObject* Object, ZEDObjectWrapper* ParentWrapper = NULL);

		ZEDObjectWrapper*						WrapObject(ZEObject* Object);

		void									CreateObject(ZEDObjectWrapper* Destination, ZEClass* Class);
		void									DeleteObject(ZEDObjectWrapper* Wrapper);
		void									DeleteObjects(const ZEArray<ZEDObjectWrapper*> Wrappers);
		void									RelocateObject(ZEDObjectWrapper* Destination, ZEDObjectWrapper* Wrapper);
		void									RelocateObjects(ZEDObjectWrapper* Destination, const ZEArray<ZEDObjectWrapper*> Wrappers);
		void									CloneObject(ZEDObjectWrapper* Wrapper);
		void									CloneObjects(const ZEArray<ZEDObjectWrapper*> Wrappers);

		void									Process(float ElapsedTime);

		ZE_EVENT(								OnObjectCreated,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object));
		ZE_EVENT(								OnObjectDestroying,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object));
		ZE_EVENT(								OnObjectDestroyed,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object));
		ZE_EVENT(								OnObjectPropertyChanged,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object, ZEMTProperty* Property));
		ZE_EVENT(								OnObjectChildObjectAdding,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));
		ZE_EVENT(								OnObjectChildObjectAdded,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));
		ZE_EVENT(								OnObjectChildObjectRemoving,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));
		ZE_EVENT(								OnObjectChildObjectRemoved,(ZEDObjectManager* Manager, ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));

		static ZEDObjectManager*				CreateInstance();
};
