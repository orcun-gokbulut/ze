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
#include "ZEGame\ZEDrawParameters.h"

bool ZEDEntityTag::InitializeSelf()
{
	if (!ZEDTag::InitializeSelf())
		return false;

	return true;
}

bool ZEDEntityTag::DeinitializeSelf()
{
	return ZEDTag::DeinitializeSelf();
}

void ZEDEntityTag::SetObject(ZEObject* Object)
{
	if (Object->GetClass()->GetParentClass() != ZEEntity::GetClass())
		return;

	ZEDTag::SetObject(Object);
}

ZEObject* ZEDEntityTag::GetObject()
{
	ZEDTag::GetObject();
}

void ZEDEntityTag::SetPosition(const ZEVector3& NewPosition)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetPosition(NewPosition);

	ZEDTag::SetPosition(NewPosition);
}

void ZEDEntityTag::SetRotation(const ZEQuaternion& NewRotation)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetRotation(NewRotation);

	ZEDTag::SetRotation(NewRotation);
}

void ZEDEntityTag::SetScale(const ZEVector3& NewScale)
{
	if (GetObject() == NULL)
		return;

	((ZEEntity*)GetObject())->SetScale(NewScale);

	ZEDTag::SetScale(NewScale);
}

bool ZEDEntityTag::Save(ZEMLWriterNode* Serializer)
{
	return true;
}

bool ZEDEntityTag::Restore(ZEMLReaderNode* Unserializer)
{
	return true;
}

void ZEDEntityTag::Tick(float Time)
{
	ZEDTag::Tick(Time);
}

void ZEDEntityTag::Draw(ZEDrawParameters* DrawParameters)
{
	ZEEntity* Entity = (ZEEntity*)GetObject();

	if (Entity == NULL)
		return;

	TagCanvas.Clean();

	DrawAxisAlignedBoundingBox(Entity->GetWorldBoundingBox(), GetDrawColor(), TagCanvas);

	if (TagCanvas.Vertices.GetCount() == 0)
		return;

	TagRenderCommand.PrimitiveCount = TagCanvas.Vertices.GetCount() / 2;
	DrawParameters->Renderer->AddToRenderList(&TagRenderCommand);
}

ZEDEntityTag* ZEDEntityTag::CreateInstance()
{
	return new ZEDEntityTag();
}
