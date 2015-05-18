//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelection.cpp
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

#include "ZEDSelection.h"

void ZEDSelection::CalculateBoundingBox()
{

}

bool ZEDSelection::IsElementExists(const ZEObject* Object)
{
	if (Elements.GetCount() == 0)
		return false;

	for (ZESize I = 0; I < Elements.GetCount(); I++)
		if (Elements[I].Object == Object)
			return true;

	return false;
}

ZEArray<ZEDSelectionElement>& ZEDSelection::GetElements()
{
	return Elements;
}

ZEObject* ZEDSelection::GetElement(ZESize Index)
{
	return Elements[Index].Object;
}

void ZEDSelection::AddElement(const ZEObject* Object)
{
	if (Object == NULL)
		return;

	if (IsElementExists(Object))
		return;

	//How to set Element.OffsetToPivot? Should it be sent to TransformationManager.
}

void ZEDSelection::RemoveElement(const ZEObject* Object)
{
	if (Object == NULL)
		return;

	if (!IsElementExists(Object))
		return;

	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		if (Elements[I].Object == Object)
		{
			Elements.Remove(I);
			return;
		}
	}
}

const ZEMatrix4x4& ZEDSelection::GetTransform()
{
	if (!IsDirty)
		return Transformation;

	ZEMatrix4x4::CreateOrientation(Transformation, PivotPosition, PivotRotation, PivotScale);
	IsDirty = false;

	return Transformation;
}

void ZEDSelection::SetPosition(const ZEVector3& Position)
{
	PivotPosition = Position;
	IsDirty = true;
}

const ZEVector3& ZEDSelection::GetPosition()
{
	return PivotPosition;
}

void ZEDSelection::SetRotation(const ZEQuaternion& Rotation)
{
	PivotRotation = Rotation;
	IsDirty = true;
}

const ZEQuaternion& ZEDSelection::GetRotation()
{
	return PivotRotation;
}

void ZEDSelection::SetScale(const ZEVector3& Scale)
{
	PivotScale = Scale;
	IsDirty = true;
}

const ZEVector3& ZEDSelection::GetScale()
{
	return PivotScale;
}

bool ZEDSelection::IsLocked()
{
	return Lock;
}

void ZEDSelection::Tick(float Time)
{

}

void ZEDSelection::Draw(ZEDrawParameters* Parameters)
{

}

ZEDSelection::ZEDSelection()
{
	IsDirty = true;
	Lock = false;
}

ZEDSelection::~ZEDSelection()
{

}
