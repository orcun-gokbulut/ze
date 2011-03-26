//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionItem.h
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
#ifndef __ZED_SELECTION_ITEM_H__
#define __ZED_SELECTION_ITEM_H__

#include "ZEMeta\ZEClass.h"
#include <QWidget>
#include "ZEDCommonEntities/ZEDGizmo.h"

enum ZEDGizmoMode;

enum ZEDGizmoSpace
{
	ZED_GS_WORLD,
	ZED_GS_LOCAL,
	ZED_GS_VIEW
};

class ZEDSelectionItem
{
	protected:

		ZEDGizmoSpace				GizmoSpace;

	public:

		virtual void				Update() = 0;

		virtual ZEArray<QWidget*>	GetCustomPropertyWidgets(QWidget* Parent) = 0;

		virtual void				MoveUsingGizmo(ZEVector3 MoveAmount) = 0;
		virtual void				ScaleUsingGizmo(ZEVector3 ScaleAmount) = 0;
		virtual void				RotateUsingGizmo(ZEQuaternion RotateAmount) = 0;

		virtual ZEVariant			GetPosition() = 0;
		virtual ZEVariant			GetScale() = 0;
		virtual ZEVariant			GetRotation() = 0;

		virtual void				SetVisiblity(bool Visibility) = 0;
		virtual bool				GetVisiblity() = 0;

		void						SetGizmoSpace(ZEDGizmoSpace GizmoSpace);
		ZEDGizmoSpace				GetGizmoSpace();

		virtual void				SetGizmoMode(ZEDGizmoMode Mode) = 0;

		virtual	void				SetBoundingBoxVisibility(bool Visible) = 0;

		virtual ZEClass*			GetClass() const = 0;
		virtual ZEDGizmo*			GetGizmo() const = 0;

		virtual						~ZEDSelectionItem();
};

#endif
