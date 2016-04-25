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

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEAABBox.h"

#include "ZEGame/ZERayCast.h"
#include "ZERenderer/ZECanvas.h"
#include "ZERenderer/ZERNCommand.h"

#include <QWidget>
#include <QMenu>

class ZERNPreRenderParameters;
class ZERNRenderParameters;

/*ZE_ATTRIBUTE_0(ObjectClass)*/
class ZEDObjectWrapper : public ZEObject
{
/*	ZE_OBJECT*/
	protected:
		ZEObject*							Object;
		ZEDObjectWrapper*					Parent;

		ZEString							Icon;
		bool								Selectable;
		bool								Selected;
		bool								Locked;

		ZEArray<ZEDObjectWrapper*>			ChildWrappers;

											ZEDObjectWrapper();
		virtual								~ZEDObjectWrapper();

 	//public slots:
 	//	virtual void						OnSelected();
 	//	virtual void						OnDeselected();
 	//	virtual void						OnCreated();
 	//	virtual void						OnDestroy();
 	//	virtual void						OnTransformed();
 	//	virtual void						OnChildObjectChanged();
 	//	virtual void						OnParentObjectChanged();
 	//	virtual void						OnOpenContainer();
 	//	virtual void						OnCloseContainer();

 	//	virtual void						Save();
 	//	virtual void						Load();

	public:
		virtual void						SetObject(ZEObject* Object);
		virtual ZEObject*					GetObject();
		
		virtual void						SetParent(ZEDObjectWrapper* Wrapper);
		virtual ZEDObjectWrapper*			GetParent();

		virtual void						SetId(ZEInt Id);
		virtual ZEInt						GetId();

		virtual void						SetName(const ZEString& Name);
		virtual ZEString					GetName();

		void								SetIcon(const ZEString& Icon);
		const ZEString&						GetIcon();

		virtual ZEAABBox					GetLocalBoundingBox();
		virtual ZEMatrix4x4					GetWorldTransform();

		virtual void						SetPosition(const ZEVector3& NewPosition);
		virtual ZEVector3					GetPosition();

		virtual void						SetRotation(const ZEQuaternion& NewRotation);
		virtual ZEQuaternion				GetRotation();

		virtual void						SetScale(const ZEVector3& NewScale);
		virtual ZEVector3					GetScale();

		virtual void						SetLocked(bool Value);
		virtual bool						GetLocked();

		void								SetSelectable(bool Value);
		bool								GetSelectable();

		virtual void						SetSelected(bool Selected);
		virtual bool						GetSelected();

		virtual void						SetVisible(bool Value);
		virtual bool						GetVisible();

		virtual const ZEArray<ZEDObjectWrapper*>& GetChildWrappers();
		virtual void						AddChildWrapper(ZEDObjectWrapper* Wrapper);
		virtual void						RemoveChildWrapper(ZEDObjectWrapper* Wrapper);
		
		virtual QWidget*					GetCustomWidget();
		virtual QMenu*						GetPopupMenu();

		virtual void						PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void						Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);
		virtual void						Tick(float ElapsedTime);
		virtual bool						RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters) = 0;

		virtual void						Destroy();
};
