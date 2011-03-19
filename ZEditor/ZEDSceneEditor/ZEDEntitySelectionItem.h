//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntitySelectionItem.h
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
#ifndef __ZED_LIGHT_BRUSH_SELECTION_ITEM_H__
#define __ZED_LIGHT_BRUSH_SELECTION_ITEM_H__

#include "ZEDSelectionItem.h"
#include "ZEGame\ZEScene.h"
#include "ZEGame\ZEEntity.h"
#include "ZEGame\ZECanvasBrush.h"
#include "ZEDCommonEntities\ZEDGizmo.h"

class ZEDEntitySelectionItem : public ZEDSelectionItem
{
private:

	ZEEntity*					Entity;
	ZEScene*					Scene;
	ZEDGizmo*					Gizmo;

public:

	virtual void				Update();
	virtual ZEArray<QWidget*>	GetCustomPropertyWidgets(QWidget* Parent);

	virtual void				MoveUsingGizmo(ZEVector3 MoveAmount);
	virtual void				ScaleUsingGizmo(ZEVector3 ScaleAmount);
	virtual void				RotateUsingGizmo(ZEQuaternion RotateAmount);

	virtual ZEVariant			GetPosition();
	virtual ZEVariant			GetScale();
	virtual ZEVariant			GetRotation();

	virtual void				SetGizmoMode(ZEDGizmoMode Mode);
	virtual	void				SetBoundingBoxVisibility(bool Visible);

	virtual void				SetVisiblity(bool Visibility);
	virtual bool				GetVisiblity();

	virtual ZEClass*			GetClass() const;
	virtual ZEDGizmo*			GetGizmo() const;

								ZEDEntitySelectionItem(ZEEntity* Entity, ZEDGizmoMode Mode, ZEScene* Scene);
	virtual						~ZEDEntitySelectionItem();
};

#endif
