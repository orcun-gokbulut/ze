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

bool ZEDSelection::IsElementExists(const ZEDTag* Element)
{
	if (Elements.GetCount() == 0)
		return false;

	for (ZESize I = 0; I < Elements.GetCount(); I++)
		if (Elements[I].Tag == Element)
			return true;

	return false;
}

ZEArray<ZEDSelectionElement>& ZEDSelection::GetElements()
{
	return Elements;
}

ZEDTag* ZEDSelection::GetElement(ZESize Index)
{
	return Elements[Index].Tag;
}

void ZEDSelection::AddElement(const ZEDTag* Element)
{
	if (Element == NULL)
		return;

	if (IsElementExists(Element))
		return;

	DirtyFlags.RaiseFlags(ZED_SELECTION_DIRTY_FLAG_BBOX);
	//How to set Element.OffsetToPivot? Should it be sent to TransformationManager.
}

void ZEDSelection::RemoveElement(const ZEDTag* Element)
{
	if (Element == NULL)
		return;

	if (!IsElementExists(Element))
		return;

	for (ZESize I = 0; I < Elements.GetCount(); I++)
	{
		if (Elements[I].Tag == Element)
		{
			Elements.Remove(I);
			DirtyFlags.RaiseFlags(ZED_SELECTION_DIRTY_FLAG_BBOX);
			return;
		}
	}
}

void ZEDSelection::SetDirtyFlags(ZEUInt Flags, bool Value)
{
	DirtyFlags.SetFlags(Flags, Value);
}

ZEUInt ZEDSelection::GetDirtyFlags()
{
	return DirtyFlags.Value;
}

const ZEMatrix4x4& ZEDSelection::GetTransform()
{
	if (!DirtyFlags.GetFlags(ZED_SELECTION_DIRTY_FLAG_TRANSFORM))
		return Transformation;

	ZEMatrix4x4::CreateOrientation(Transformation, PivotPosition, PivotRotation, PivotScale);
	DirtyFlags.UnraiseFlags(ZED_SELECTION_DIRTY_FLAG_TRANSFORM);

	return Transformation;
}

const ZEAABBox& ZEDSelection::GetBoundingBox()
{
	if (!DirtyFlags.GetFlags(ZED_SELECTION_DIRTY_FLAG_BBOX))
		return BoundingBox;

	CalculateBoundingBox();
	DirtyFlags.UnraiseFlags(ZED_SELECTION_DIRTY_FLAG_BBOX);

	return BoundingBox;
}

void ZEDSelection::SetPosition(const ZEVector3& Position)
{
	PivotPosition = Position;
	DirtyFlags.RaiseFlags(ZED_SELECTION_DIRTY_FLAG_TRANSFORM);
}

const ZEVector3& ZEDSelection::GetPosition()
{
	return PivotPosition;
}

void ZEDSelection::SetRotation(const ZEQuaternion& Rotation)
{
	PivotRotation = Rotation;
	DirtyFlags.RaiseFlags(ZED_SELECTION_DIRTY_FLAG_TRANSFORM);
}

const ZEQuaternion& ZEDSelection::GetRotation()
{
	return PivotRotation;
}

void ZEDSelection::SetScale(const ZEVector3& Scale)
{
	PivotScale = Scale;
	DirtyFlags.RaiseFlags(ZED_SELECTION_DIRTY_FLAG_TRANSFORM);
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
	DirtyFlags.RaiseFlags(ZED_SELECTION_DIRTY_FLAG_ALL);
	Lock = false;
}

ZEDSelection::~ZEDSelection()
{

}
