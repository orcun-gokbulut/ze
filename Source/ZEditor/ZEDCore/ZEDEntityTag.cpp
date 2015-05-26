//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntityTag.cpp
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

#include "ZEDEntityTag.h"
#include "ZEDSelection.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZERenderer.h"

void ZEDEntityTag::SetObject(ZEObject* Object)
{
	if (Object->GetClass()->GetParentClass() != ZEEntity::Class())
		return;

	ZEDTag::SetObject(Object);
}

ZEObject* ZEDEntityTag::GetObject()
{
	return ZEDTag::GetObject();
}

ZEAABBox ZEDEntityTag::GetBoundingBox()
{
	if (GetObject() == NULL)
		return ZEAABBox();

	return ((ZEEntity*)GetObject())->GetBoundingBox();
}

ZEAABBox ZEDEntityTag::GetWorldBoundingBox()
{
	if (GetObject() == NULL)
		return ZEAABBox();

	return ((ZEEntity*)GetObject())->GetWorldBoundingBox();
}

void ZEDEntityTag::SetPosition(const ZEVector3& NewPosition)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetPosition(NewPosition);

	if (GetSelection() != NULL)
		GetSelection()->SetDirtyFlags(ZED_SELECTION_DIRTY_FLAG_BBOX, true);
}

ZEVector3 ZEDEntityTag::GetPosition()
{
	if (GetObject() == NULL)
		return ZEVector3::Zero;

	return ((ZEEntity*)GetObject())->GetPosition();
}

void ZEDEntityTag::SetWorldPosition(const ZEVector3& NewPosition)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetWorldPosition(NewPosition);

	if (GetSelection() != NULL)
		GetSelection()->SetDirtyFlags(ZED_SELECTION_DIRTY_FLAG_BBOX, true);
}

ZEVector3 ZEDEntityTag::GetWorldPosition()
{
	if (GetObject() == NULL)
		return ZEVector3::Zero;

	return ((ZEEntity*)GetObject())->GetWorldPosition();
}

void ZEDEntityTag::SetRotation(const ZEQuaternion& NewRotation)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetRotation(NewRotation);

	if (GetSelection() != NULL)
		GetSelection()->SetDirtyFlags(ZED_SELECTION_DIRTY_FLAG_BBOX, true);
}

ZEQuaternion ZEDEntityTag::GetRotation()
{
	if (GetObject() == NULL)
		return ZEQuaternion::Identity;

	return ((ZEEntity*)GetObject())->GetRotation();
}

void ZEDEntityTag::SetWorldRotation(const ZEQuaternion& NewRotation)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetWorldRotation(NewRotation);

	if (GetSelection() != NULL)
		GetSelection()->SetDirtyFlags(ZED_SELECTION_DIRTY_FLAG_BBOX, true);
}

ZEQuaternion ZEDEntityTag::GetWorldRotation()
{
	if (GetObject() == NULL)
		return ZEQuaternion::Identity;

	return ((ZEEntity*)GetObject())->GetWorldRotation();
}

void ZEDEntityTag::SetScale(const ZEVector3& NewScale)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetScale(NewScale);

	if (GetSelection() != NULL)
		GetSelection()->SetDirtyFlags(ZED_SELECTION_DIRTY_FLAG_BBOX, true);
}

ZEVector3 ZEDEntityTag::GetScale()
{
	if (GetObject() == NULL)
		return ZEVector3::One;

	return ((ZEEntity*)GetObject())->GetScale();
}

void ZEDEntityTag::SetWorldScale(const ZEVector3& NewScale)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetWorldScale(NewScale);

	if (GetSelection() != NULL)
		GetSelection()->SetDirtyFlags(ZED_SELECTION_DIRTY_FLAG_BBOX, true);
}

ZEVector3 ZEDEntityTag::GetWorldScale()
{
	if (GetObject() == NULL)
		return ZEVector3::One;

	return ((ZEEntity*)GetObject())->GetWorldScale();
}

void ZEDEntityTag::Draw(ZEDrawParameters* DrawParameters)
{
	if (GetObject() == NULL)
		return;

	ZEEntity* Entity = (ZEEntity*)GetObject();

	TagCanvas.Clean();

	DrawAxisAlignedBoundingBox(Entity->GetWorldBoundingBox(), ZEVector4::UnitW, TagCanvas); //How to affect draw colors etc globally.

	if (TagCanvas.Vertices.GetCount() == 0)
		return;

	TagRenderCommand.PrimitiveCount = TagCanvas.Vertices.GetCount() / 2;
	DrawParameters->Renderer->AddToRenderList(&TagRenderCommand);
}

ZEDEntityTag* ZEDEntityTag::CreateInstance()
{
	return new ZEDEntityTag();
}
