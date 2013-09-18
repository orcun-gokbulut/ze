//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEView.cpp
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

#include "ZEView.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGraphics/ZEGraphicsWindow.h"
#include "ZEGraphics/ZEGraphicsModule.h"

void ZEView::OnTransformChanged()
{
	DirtyViewVolume = true;
	DirtyViewTransform = true;
	DirtyViewProjectionTransform = true;
	ZEEntity::OnTransformChanged();
}

void ZEView::SetNearZ(float NearZ)
{
	this->NearZ = NearZ;

	DirtyViewVolume = true;
	DirtyProjectionTransform = true;
	DirtyViewProjectionTransform = true;
}

float ZEView::GetNearZ() const
{
	return NearZ;
}

void ZEView::SetFarZ(float FarZ)
{
	this->FarZ = FarZ;

	DirtyViewVolume = true;
	DirtyProjectionTransform = true;
	DirtyViewProjectionTransform = true;
}

float ZEView::GetFarZ() const
{
	return FarZ;
}

void ZEView::SetFov(float Fov)
{
	this->Fov = Fov;

	DirtyViewVolume = true;
	DirtyProjectionTransform = true;
	DirtyViewProjectionTransform = true;
}

float ZEView::GetFov() const
{
	return Fov;
}

void ZEView::SetWidth(float Value)
{
	ViewDimension.x = Value;

	DirtyViewVolume = true;
	DirtyProjectionTransform = true;
	DirtyViewProjectionTransform = true;
}

float ZEView::GetWidth() const
{
	return ViewDimension.x;
}

void ZEView::SetHeight(float Value)	
{
	ViewDimension.y = Value;

	DirtyViewVolume = true;
	DirtyProjectionTransform = true;
	DirtyViewProjectionTransform = true;
}

float ZEView::GetHeight() const
{
	return ViewDimension.y;
}

void ZEView::SetViewDimension(const ZEVector2& Dimension)
{
	ViewDimension = Dimension;

	DirtyViewVolume = true;
	DirtyProjectionTransform = true;
	DirtyViewProjectionTransform = true;
}

const ZEVector2& ZEView::GetViewDimension() const
{
	return ViewDimension;
}

float ZEView::GetAspectRatio() const
{
	return ViewDimension.x / ViewDimension.y;
}

void ZEView::SetPosition(const ZEVector3& NewPosition)
{
	DirtyViewVolume = true;
	DirtyViewTransform = true;
	DirtyViewProjectionTransform = true;
	ZEEntity::SetPosition(NewPosition);
}

void ZEView::SetRotation(const ZEQuaternion& NewRotation)
{
	DirtyViewVolume = true;
	DirtyViewTransform = true;
	DirtyViewProjectionTransform = true;
	ZEEntity::SetRotation(NewRotation);
}

ZEView::ZEView()
{
	FarZ = 0.0f;
	NearZ = 0.0f;
	Fov = ZE_PI_2;
	
	DirtyViewVolume = true;
	DirtyViewTransform = true;
	DirtyProjectionTransform = true;
	DirtyViewProjectionTransform = true;
	
	zeGraphics->GetWindow()->GetSize(ViewDimension);
}

ZEView::~ZEView()
{

}
