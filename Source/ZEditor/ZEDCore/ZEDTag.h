//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTag.h
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
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include <QtGui/QWidget>
#include <QtGui/QMenu>

class ZEDTag
{
	private:
		ZESize Id;
		ZEString Name;
		ZEString Icon;
		QWidget* CustomWidget;
		QMenu* PopupMenu;
		
		ZEObject* Object;

		ZEDTag* ParentTag;
		ZEArray<ZEDTag*> ChildTags;
		ZEArray<ZEDTag*> ComponentTags;
		
	protected:
		ZEDTag();
		virtual ~ZEDTag();

	public slots:
		virtual void OnSelected();
		virtual void OnDeselected();
		virtual void OnCreated();
		virtual void OnDestroy();
		virtual void OnTransformed();
		virtual void OnChildObjectChanged();
		virtual void OnParentObjectChanged();
		virtual void OnOpenContainer();
		virtual void OnCloseContainer();
		virtual void Save();
		virtual void Load();

	public:
		void SetId(ZESize Id);
		ZESize GetId();
		void SetName(const ZEString& Name);
		const ZEString& GetName();
		void SetIcon(const ZEString& Icon);
		const ZEString& GetIcon();
		void SetCustomWidget(QWidget* Widget);
		QWidget* GetCustomWidget();
		void SetPopupMenu(QMenu* Menu);
		QMenu* GetPopupMenu();

		virtual void SetObject(ZEObject* Object);
		virtual ZEObject* GetObject();

		virtual void SetVisibility(bool Value) = 0;
		virtual bool GetVisibility() = 0;

		virtual ZEOBBox GetBoundingBox() = 0;

		virtual void SetPosition(const ZEVector3& NewPosition) = 0;
		virtual ZEVector3 GetPosition() = 0;

		virtual void SetRotation(const ZEQuaternion& NewRotation) = 0;
		virtual ZEQuaternion GetRotation() = 0;

		virtual void SetScale(const ZEVector3& NewScale) = 0;
		virtual ZEVector3 GetScale() = 0;

		void SetParentTag(ZEDTag* Tag);
		ZEDTag* GetParentTag();

		ZEArray<ZEDTag*>& GetChildTags();
		virtual void AddChildTag(ZEDTag* Tag);
		virtual void RemoveChildTag(ZEDTag* Tag);

		ZEArray<ZEDTag*>& GetComponentTags();
		void AddComponentTag(ZEDTag* Tag);
		void RemoveComponentTag(ZEDTag* Tag);

};
