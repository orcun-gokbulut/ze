//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECamera.h
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
#ifndef	__ZE_CAMERA_H__
#define __ZE_CAMERA_H__

#include "ZEGame\ZEComponent.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZECanvas.h"
#include "ZEGraphics\ZEViewVolume.h"
#include "ZEMath\ZEPlane.h"
#include "ZEMath\ZERectangle.h"
#include "ZEGame\ZEPortal.h"
#include "ZEGame\ZEDrawParameters.h"

enum ZECameraProjectionType
{
	ZE_CPT_NONE,
	ZE_CPT_ORTHOGRAPHICAL,
	ZE_CPT_PERSPECTIVE
};

class ZEViewPort;

ZE_META_CLASS_DESCRIPTION(ZECamera);

class ZECamera : public ZEComponent
{
	ZE_META_CLASS()
	private:
		bool							UpdateViewFrustum;
		bool							UpdateView;
		bool							UpdateViewTransform;
		bool							UpdateProjectionTransform;
		bool							UpdateViewProjectionTransform;


		float							NearZ, FarZ;
		float							FOV, AspectRatio;
		float							Width, Height;

		ZEMatrix4x4						ViewTransform;
		ZEMatrix4x4						ProjectionTransform;
		ZEMatrix4x4						ViewProjectionTransform;

		ZEViewFrustum					ViewFrustum;
		ZEView							View;
										ZECamera();
	public:
		void							SetNearZ(float NearZ);
		float							GetNearZ() const;

		void							SetFarZ(float FarZ);
		float							GetFarZ() const;

		void							SetFOV(float FOV);
		float							GetFOV() const;

		void							SetAspectRatio(float AspectRatio);
		float							GetAspectRatio() const;

		virtual const ZEMatrix4x4&		GetViewTransform();
		virtual const ZEMatrix4x4&		GetProjectionTransform();
		virtual const ZEMatrix4x4&		GetViewProjectionTransform();

		virtual void					SetPosition(const ZEVector3& NewPosition);	
		virtual void					SetLocalRotation(const ZEQuaternion& NewRotation);

		virtual void					OwnerWorldTransformChanged();	

		const ZEView&					GetView();
		const ZEViewVolume&				GetViewVolume();

		void							GetScreenRay(ZERay& Ray, int ScreenX, int ScreenY);

		static ZECamera*				CreateInstance();
};

#endif

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZECamera" parent="ZEComponent" noinstance="true">
			<description>Camera</description>
			<property name="NearZ" type="float" autogetset="yes" description="The nearest distance that camera can show"/>
			<property name="FarZ" type="float" autogetset="yes" description="The farest distance that camera can show"/>
			<property name="FOV" type="float" autogetset="yes" description="Field of view"/>
			<property name="AspectRatio" type="float" autogetset="yes" description="Width vs Height aspect ratio"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
