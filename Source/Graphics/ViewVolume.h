//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ViewVolume.h
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
#ifndef __ZE_VIEWVOLUME_H__
#define __ZE_VIEWVOLUME_H__

#include "ZEMath/Vector.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEMath/OBoundingBox.h"
#include "ZEMath/BoundingSphere.h"
#include "ZEMath/Rectangle3D.h"

enum ZEViewVolumeType
{
	ZE_VVT_NONE	= 0,
	ZE_VVT_FRUSTUM,
	ZE_VVT_SPHERE,
	ZE_VVT_HEMISPHERE,
	ZE_VVT_CUBOID,
	ZE_VVT_CONE,
	ZE_VVT_PLANE
};

enum ZEDoorViewTest
{
	ZE_DVT_INSIDE,
	ZE_DVT_OUTSIDE,
	ZE_DVT_INTERSECTS,
	ZE_DVT_DOORCOVERS
};

class ZEEntity;
class ZEEntityComponent;
class ZELight;
class ZEViewVolume
{
	public:
		virtual ZEViewVolumeType			GetViewVolumeType() const = 0;

		virtual bool						LightCullTest(ZELight* Light) const = 0;
		virtual bool						CullTest(const ZEAABoundingBox& BoundingBox) const = 0;
		virtual bool						CullTest(ZEEntity* Entity) const = 0;
		virtual bool						CullTest(ZEEntityComponent* Component) const = 0;
		virtual ZEDoorViewTest				CullTest(const ZERectangle3D& PortalDoor) const = 0;
};

class ZEViewFrustum : public ZEViewVolume
{
	private:
		ZEVector3							Position;
		ZEVector3							CenterVector;
		float								SinRatio;
		float								TanRatio;

		ZEPlane								LeftClippingPlane;
		ZEPlane								RightClippingPlane;
		ZEPlane								TopClippingPlane;
		ZEPlane								BottomClippingPlane;
		ZEPlane								FarClippingPlane;
		ZEPlane								NearClippingPlane;


		bool								ConeCullTest(const ZEBoundingSphere& BoundingSphere) const;
		virtual bool						CullTest(const ZEOBoundingBox& BoundingBox) const;
		virtual bool						CullTest(const ZEBoundingSphere& BoundingSphere) const;

	public:
		virtual ZEViewVolumeType			GetViewVolumeType() const;

		virtual bool						LightCullTest(ZELight* Light) const;
		virtual bool						CullTest(const ZEAABoundingBox& BoundingBox) const;
		virtual bool						CullTest(ZEEntity* Entity) const;
		virtual bool						CullTest(ZEEntityComponent* Component) const;
		virtual ZEDoorViewTest				CullTest(const ZERectangle3D& PortalDoor) const;

		void								Create(const ZEVector3& Position, const ZEQuaternion& Rotation, 
												   float FOV, float AspectRatio, float NearZ, float FarZ);
};

class ZEViewSphere : public ZEViewVolume
{
	private:
		ZEBoundingSphere					BoundingSphere;
		float								NearZ;

	public:
		virtual ZEViewVolumeType			GetViewVolumeType() const;

		virtual bool						LightCullTest(ZELight* Light) const;
		virtual bool						CullTest(const ZEAABoundingBox& BoundingBox) const;
		virtual bool						CullTest(ZEEntity* Entity) const;
		virtual bool						CullTest(ZEEntityComponent* Component) const;
		virtual ZEDoorViewTest				CullTest(const ZERectangle3D& PortalDoor) const;

		void								Create(const ZEVector3& Position, float Radius, float NearZ);
};

class ZEViewHemiSphere : public ZEViewVolume
{
	private:
		ZEPlane								HalfPlane;
		ZEBoundingSphere					BoundingSphere;

	public:
		virtual ZEViewVolumeType			GetViewVolumeType() const;

		virtual bool						LightCullTest(ZELight* Light) const;
		virtual bool						CullTest(const ZEAABoundingBox& BoundingBox) const;
		virtual bool						CullTest(ZEEntity* Entity) const;
		virtual bool						CullTest(ZEEntityComponent* Component) const;
		virtual ZEDoorViewTest				CullTest(const ZERectangle3D& PortalDoor) const;

		void								Create(const ZEVector3& Position, const ZEVector3& Direction, float Radius, float NearZ);
};

class ZEViewCuboid: public ZEViewVolume
{
	private:
		ZEOBoundingBox						BoundingBox;

	public:
		virtual ZEViewVolumeType			GetViewVolumeType() const;

		virtual bool						LightCullTest(ZELight* Light) const;
		virtual bool						CullTest(const ZEAABoundingBox& BoundingBox) const;
		virtual bool						CullTest(ZEEntity* Entity) const;
		virtual bool						CullTest(ZEEntityComponent* Component) const;
		virtual ZEDoorViewTest				CullTest(const ZERectangle3D& PortalDoor) const;

		void								Create(const ZEVector3& Position, const ZEQuaternion& Rotation,
												   float Width, float Height, float NearZ, float FarZ);

};

class ZEViewPlane : public ZEViewVolume
{
	public:
		ZEPlane								Plane;
		float								MaxDistance;

	public:
		virtual ZEViewVolumeType			GetViewVolumeType() const;
		
		virtual bool						LightCullTest(ZELight* Light) const;
		virtual bool						CullTest(const ZEAABoundingBox& BoundingBox) const;
		virtual bool						CullTest(ZEEntity* Entity) const;
		virtual bool						CullTest(ZEEntityComponent* Component) const;
		virtual ZEDoorViewTest				CullTest(const ZERectangle3D& PortalDoor) const;				

		void								Create(const ZEVector3& Position, const ZEVector3& Direction, float MaxDistance);

};
#endif
