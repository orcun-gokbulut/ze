//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEViewCuboid.cpp
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

#include "ZEViewCuboid.h"
#include "ZEQuaternion.h"

ZEViewVolumeType ZEViewCuboid::GetViewVolumeType() const
{
	return ZE_VVT_CUBOID;
}

bool ZEViewCuboid::CullTest(const ZEBSphere& BoundingSphere) const
{
	return !ZEOBBox::IntersectionTest(Box, BoundingSphere);
}

bool ZEViewCuboid::CullTest(const ZEAABBox& BoundingBox) const
{
	return !ZEOBBox::IntersectionTest(Box, BoundingBox);
}

bool ZEViewCuboid::CullTest(const ZEOBBox& BoundingBox) const
{
	return !ZEOBBox::IntersectionTest(Box, BoundingBox);
}

bool ZEViewCuboid::CullTest(const ZERectangle3D& Rectangle) const
{
	zeDebugCheck(true, "Not implemented.");
	return false;
}

void ZEViewCuboid::Create(const ZEOBBox& OBBox)
{
	Box.Center = OBBox.Center;
	Box.Right = OBBox.Right;
	Box.Up = OBBox.Up;
	Box.Front = OBBox.Front;
	Box.HalfSize = OBBox.HalfSize;
}

void ZEViewCuboid::Create(const ZEVector3& Position, const ZEQuaternion& Rotation, float Width, float Height, float NearZ, float FarZ)
{
	ZEQuaternion::VectorProduct(Box.Right, Rotation, ZEVector3::UnitX);
	ZEQuaternion::VectorProduct(Box.Up, Rotation, ZEVector3::UnitY);
	ZEQuaternion::VectorProduct(Box.Front, Rotation, ZEVector3::UnitZ);

	Box.HalfSize = ZEVector3(Width * 0.5f, Height * 0.5f, (FarZ - NearZ) * 0.5f);

	ZEVector3 Center;
	ZEQuaternion::VectorProduct(Center, Rotation, ZEVector3(0.0f, 0.0f, FarZ - NearZ));

	ZEVector3::Multiply(Box.Center, Position, Center);
}
