//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEViewFrustum.h
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
#ifndef __ZE_VIEW_FRUSTUM_H__
#define __ZE_VIEW_FRUSTUM_H__

#include "ZEViewVolume.h"

enum ZEViewFrustumVertex
{
	ZE_VFV_FARRIGHTUP	= 0,
	ZE_VFV_FARRIGHTDOWN,
	ZE_VFV_FARLEFTDOWN,
	ZE_VFV_FARLEFTUP,
	ZE_VFV_NEARRIGHTUP,
	ZE_VFV_NEARRIGHTDOWN,
	ZE_VFV_NEARLEFTDOWN,
	ZE_VFV_NEARLEFTUP
};

enum ZEViewFrustumEdge
{
	ZE_VFE_FARRIGHT = 0,
	ZE_VFE_FARBOTTOM,
	ZE_VFE_FARLEFT,
	ZE_VFE_FARTOP,
	ZE_VFE_RIGHTUP,
	ZE_VFE_RIGHTDOWN,
	ZE_VFE_LEFTDOWN,
	ZE_VFE_LEFTUP,
	ZE_VFE_NEARRIGHT,
	ZE_VFE_NEARBOTTOM,
	ZE_VFE_NEARLEFT,
	ZE_VFE_NEARTOP
};

enum ZEViewFrustumPlane
{
	ZE_VFP_RIGHT = 0,
	ZE_VFP_BOTTOM,
	ZE_VFP_LEFT,
	ZE_VFP_TOP,
	ZE_VFP_FAR,
	ZE_VFP_NEAR
};

class ZEViewFrustum : public ZEViewVolume
{
	private:
		ZEVector3							Position;
		ZEVector3							CenterVector;
		float								SinRatio;
		float								TanRatio;

		float								AspectRatio;
		float								FOV;
		float								NearZ;	
		float								FarZ;

		ZEPlane								RightClippingPlane;
		ZEPlane								BottomClippingPlane;
		ZEPlane								LeftClippingPlane; 
		ZEPlane								TopClippingPlane;
		ZEPlane								FarClippingPlane;
		ZEPlane								NearClippingPlane;

	public:

		ZEVector3							Look;
		ZEVector3							Right;
		ZEVector3							Up;

		virtual ZEViewVolumeType			GetViewVolumeType() const;

		void								SetPosition(ZEVector3 Value);
		virtual ZEVector3					GetPosition() const;

		const ZEPlane&						GetClippingPlane(ZEViewFrustumPlane Plane) const;
		const ZELineSegment					GetEdge(ZEViewFrustumEdge Edge) const;
		const ZEVector3						GetVertex(ZEViewFrustumVertex Vertex) const;

		float								GetAspectRatio() const;
		float								GetFOV() const;
		float								GetNearZ() const;
		float								GetFarZ() const;

		const ZEVector3&					GetDirection() const;
		const ZEVector3&					GetRight() const;
		const ZEVector3&					GetUp() const;

		virtual bool						CullTest(const ZEAABBox& BoundingBox) const;
		virtual bool						CullTest(const ZEOBBox& BoundingBox) const;
		virtual bool						CullTest(const ZEBSphere& BoundingSphere) const;
		virtual bool						CullTest(const ZERectangle3D& Rectangle) const;

		ZEArray<ZELineSegment>				GetFrustumLineSegments();
		ZEArray<ZELineSegment>				GetCustomFrustumLineSegments();

		void								Create(const ZEVector3& Position, const ZEQuaternion& Rotation, 
												   float FOV, float AspectRatio, float NearZ, float FarZ);

		void								Create(const ZEPlane& RightClippingPlane, const ZEPlane& BottomClippingPlane, const ZEPlane& LeftClippingPlane,
													const ZEPlane& TopClippingPlane, const ZEPlane& FarClippingPlane, const ZEPlane& NearClippingPlane);
};


#endif
