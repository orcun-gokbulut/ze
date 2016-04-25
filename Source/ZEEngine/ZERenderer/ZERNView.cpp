//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNView.cpp
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

#include "ZERNView.h"

ZERNView::ZERNView()
{
	Type = ZERN_VT_NONE;
	Entity = NULL;

	ViewTransform = ZEMatrix4x4::Identity;
	ProjectionTransform = ZEMatrix4x4::Identity;		
	ViewProjectionTransform = ZEMatrix4x4::Identity;
	InvViewTransform = ZEMatrix4x4::Identity;
	InvProjectionTransform = ZEMatrix4x4::Identity;			
	InvViewProjectionTransform = ZEMatrix4x4::Identity;
	
	ProjectionType = ZERN_PT_NONE;
	VerticalFOV = 0.0f;
	VerticalFOVTop = 0.0f;
	VerticalFOVBottom = 0.0f;
	HorizontalFOV = 0.0f;
	HorizontalFOVRight = 0.0f;
	HorizontalFOVLeft = 0.0f;
	AspectRatio = 1.0f;
	NearZ = 0.0f;
	FarZ = 1000.0f;
	ShadowDistance = 1000.0f;
	ShadowFadeDistance = 100.0f;

	
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Direction = ZEVector3::UnitZ;
	U = ZEVector3::UnitX;
	V = ZEVector3::UnitY;
	N = ZEVector3::UnitZ;
	
	ViewVolume = NULL;
}
