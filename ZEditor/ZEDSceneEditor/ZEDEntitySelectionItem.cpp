//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntitySelectionItem.cpp
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

#include "ZEDEntitySelectionItem.h"
#include "ZEGraphics\ZECamera.h"

void ZEDEntitySelectionItem::Update()
{
	Gizmo->SetPosition(Entity->GetPosition());

	if (GizmoSpace == ZEDGizmoSpace::ZED_GS_LOCAL)
		Gizmo->SetRotation(Entity->GetRotation());
	else if (GizmoSpace == ZEDGizmoSpace::ZED_GS_VIEW)
		Gizmo->SetRotation(Scene->GetActiveCamera()->GetWorldRotation());
	else 
		Gizmo->SetRotation(ZEQuaternion::Identity);
}

ZEArray<QWidget*> ZEDEntitySelectionItem::GetCustomPropertyWidgets(QWidget* Parent)
{
	ZEArray<QWidget*> PropertyWidgets;
	PropertyWidgets.SetCount(0);
	return PropertyWidgets;
}

void ZEDEntitySelectionItem::MoveUsingGizmo(ZEVector3 MoveAmount)
{
	Gizmo->SetPosition(Gizmo->GetPosition() + MoveAmount);
	Entity->SetPosition(Gizmo->GetPosition());
}

void ZEDEntitySelectionItem::ScaleUsingGizmo(ZEVector3 ScaleAmount)
{
	Entity->SetScale(Entity->GetScale() + ScaleAmount);
}

void ZEDEntitySelectionItem::RotateUsingGizmo(ZEQuaternion RotateAmount)
{

}

ZEVariant ZEDEntitySelectionItem::GetPosition()
{
	ZEVariant TempVariant;
	TempVariant.SetVector3(Entity->GetPosition());
	return TempVariant;
}

ZEVariant ZEDEntitySelectionItem::GetScale()
{
	return Entity->GetScale();
}

ZEVariant ZEDEntitySelectionItem::GetRotation()
{
	ZEVariant TempVariant;
	TempVariant.SetQuaternion(Entity->GetRotation());
	return TempVariant;
}

void ZEDEntitySelectionItem::SetGizmoMode(ZEDGizmoMode Mode)
{
	Gizmo->SetMode(Mode);
}

void ZEDEntitySelectionItem::SetBoundingBoxVisibility(bool Visible)
{

}

void ZEDEntitySelectionItem::SetVisiblity(bool Visibility)
{
	Entity->SetVisible(Visibility);
}

bool ZEDEntitySelectionItem::GetVisiblity()
{
	return Entity->GetVisible();
}

ZEClass* ZEDEntitySelectionItem::GetClass() const
{
	ZEClass* Class = ((ZEClass*)(Entity));
	return Class;
}

ZEDGizmo* ZEDEntitySelectionItem::GetGizmo() const
{
	return Gizmo;
}

ZEDEntitySelectionItem::ZEDEntitySelectionItem(ZEEntity* Entity, ZEDGizmoMode Mode, ZEScene* Scene)
{
	this->Entity = Entity;
	this->Scene = Scene;
	this->Gizmo = new ZEDGizmo();

	Gizmo->SetMode(Mode);
	Gizmo->SetPosition(Entity->GetPosition());

	Scene->AddEntity(Gizmo);
	Gizmo->SetName(".:SELECTION__ITEM:.");
}

ZEDEntitySelectionItem::~ZEDEntitySelectionItem()
{
	Scene->RemoveEntity(Gizmo);
	delete Gizmo;
}
