//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectWrapper.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"
#include "ZEDestroyable.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMeta/ZEEvent.h"

class QWidget;
class QMenu;

class ZEDObjectWrapper : public ZEObject, public ZEInitializable, public ZEDestroyable
{
	ZE_OBJECT
	friend class ZEDObjectManager;
	private:
		ZEObject*									Object;
		ZEDObjectWrapper*							Parent;
		ZEDObjectManager*							Manager;

		ZEString									IconFileName;
		bool										Selectable;
		bool										Selected;
		bool										Focused;
		bool										Frozen;

		ZEArray<ZEDObjectWrapper*>					ChildWrappers;

		void										SetManager(ZEDObjectManager* Module);
		
	protected:
		virtual void								SetParent(ZEDObjectWrapper* Wrapper);

		void										SyncronizeChildWrappers(ZEObject*const* TargetList, ZESize TargetListSize);
		void										ClearChildWrappers();

		virtual bool								InitializeInternal();
		virtual bool								DeinitializeInternal();

													ZEDObjectWrapper();
		virtual										~ZEDObjectWrapper();

	public:
		ZEDObjectManager*							GetManager() const;

		virtual void								SetObject(ZEObject* Object);
		ZEObject*									GetObject() const;
		ZEClass*									GetObjectClass();
		ZEDObjectWrapper*							GetParent() const;

		virtual void								SetId(ZEInt Id);
		virtual ZEInt								GetId() const;

		virtual void								SetName(const ZEString& Name);
		virtual ZEString							GetName() const;

		virtual void								SetFrozen(bool Locked);
		bool										GetFrozen() const;

		virtual void								SetSelectable(bool Value);
		bool										GetSelectable() const;

		virtual void								SetSelected(bool Selected);
		bool										GetSelected() const;
		
		virtual void								SetFocused(bool Focused);
		bool										GetFocused() const;

		virtual const ZEArray<ZEDObjectWrapper*>&	GetChildWrappers();
		virtual bool								AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update = false);
		virtual bool								RemoveChildWrapper(ZEDObjectWrapper* Wrapper, bool Update = false);

		virtual bool								CheckChildrenClass(ZEClass* Class);

		virtual QWidget*							GetCustomWidget() const;
		virtual QMenu*								GetPopupMenu() const;

		virtual void								SetIconFileName(const ZEString& FileName);
		const ZEString&								GetIconFileName() const;

		virtual void								Tick(float ElapsedTime);

		virtual void								SendChangedEvent();

		virtual void								LockWrapper();
		virtual void								UnlockWrapper();

		virtual bool								Load(const ZEString& FileName);
		virtual bool								Save(const ZEString& FileName);
		virtual void								Clean();

		virtual void								UpdateLocal();
		virtual void								Update();

		virtual ZEDObjectWrapper*					Clone();

		ZE_EVENT(									OnParentChanged,(ZEDObjectWrapper* Object, ZEDObjectWrapper* Parent));
		ZE_EVENT(									OnPropertyChanged,(ZEDObjectWrapper* Object, ZEMTProperty* Property));
		ZE_EVENT(									OnSelected,(ZEDObjectWrapper* Object));
		ZE_EVENT(									OnDeselected,(ZEDObjectWrapper* Object));
		ZE_EVENT(									OnFrozen,(ZEDObjectWrapper* Object));
		ZE_EVENT(									OnUnfrozen,(ZEDObjectWrapper* Object));
		ZE_EVENT(									OnFocused,(ZEDObjectWrapper* Object));
		ZE_EVENT(									OnUnfocused,(ZEDObjectWrapper* Object));
		ZE_EVENT(									OnChildObjectAdding,(ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));
		ZE_EVENT(									OnChildObjectAdded,(ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));
		ZE_EVENT(									OnChildObjectRemoving,(ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));
		ZE_EVENT(									OnChildObjectRemoved,(ZEDObjectWrapper* Object, ZEDObjectWrapper* ChildObject));
}
ZEMT_ATTRIBUTE(ZEDObjectWrapper.TargetClass, ZEObject);
