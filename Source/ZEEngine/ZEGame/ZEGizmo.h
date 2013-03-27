//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGizmo.h
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
#ifndef __ZE_GIZMO_H__
#define __ZE_GIZMO_H__

#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZECanvas.h"
#include "ZEGraphics/ZERenderCommand.h"

enum ZEGizmoMode
{
	ZE_GM_NONE,
	ZE_GM_MOVE,
	ZE_GM_ROTATE,
	ZE_GM_SCALE,
	ZE_GM_HELPER
};

enum ZEGizmoAxis
{
	ZE_GA_NONE,
	ZE_GA_X_AXIS,
	ZE_GA_Y_AXIS,
	ZE_GA_Z_AXIS,
	ZE_GA_XY_AXIS,
	ZE_GA_XZ_AXIS,
	ZE_GA_YZ_AXIS,
	ZE_GA_XYZ_AXIS
};

ZE_META_FORWARD_DECLARE(ZESimpleMaterial, "ZEGraphics/ZESimpleMaterial.h")
ZE_META_FORWARD_DECLARE(ZERenderer, "ZEGraphics/ZERenderer.h")

ZE_CLASS(ZEGizmo)

class ZEGizmo : public ZEEntity
{
	ZE_OBJECT

	private:
		ZECanvas						GizmoLines;
		ZECanvas						GizmoTriangles;
		bool							DirtyGizmoFlag;

		float							AxisLenght;

		static ZESimpleMaterial*		GizmoMaterial;

		ZERenderCommand					RenderCommand;

		ZEGizmoMode						Mode;
		ZEGizmoAxis						HoveredAxis;

		void							UpdateMoveGizmo();
		void							UpdateRotateGizmo();
		void							UpdateScaleGizmo();
		void							UpdateHelperGizmo();

		// Projection
		ZEGizmoAxis						ProjectionAxis;
		ZEVector3						PickPosition;
		ZEVector3						MoveDifference;
		ZEQuaternion					InitialRotation;
		ZEVector3						InitialScale;

		ZEGizmoAxis						PickMoveAxis(const ZERay& Ray, float& TRay);
		ZEGizmoAxis						PickRotateAxis(const ZERay& Ray, float& TRay);
		ZEGizmoAxis						PickScaleAxis(const ZERay& Ray, float& TRay);

		ZEVector3						OldPosition;

		void							UpdateGizmo();
		
		ZEVector3						MoveProjection_(ZEGizmoAxis Axis, const ZERay& Ray);
		ZEQuaternion					RotationProjection_(ZEGizmoAxis Axis, const ZERay& Ray);
		ZEVector3						ScaleProjection_(ZEGizmoAxis Axis, const ZERay& Ray);

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

										ZEGizmo();

	public:
		virtual ZEDrawFlags				GetDrawFlags() const;

		virtual void					SetPosition(const ZEVector3& NewPosition);
		virtual void					SetRotation(const ZEQuaternion& NewRotation);

		void							SetMode(ZEGizmoMode Mode);
		ZEGizmoMode						GetMode();
		
		void							SetHoveredAxis(ZEGizmoAxis HoveredAxis);
		ZEGizmoAxis						GetHoveredAxis();

		void							SetAxisLenght(float Size);
		float							GetAxisLenght();

		ZEVector3						GetPositionChange();

		ZEGizmoAxis						PickAxis(const ZERay& Ray, float& TRay);
		
		void							StartMoveProjection(ZEGizmoAxis Axis, const ZERay& InitialRay);
		ZEVector3						MoveProjection(const ZERay& Ray);
		
		void							StartRotationProjection(ZEGizmoAxis& Axis, const ZERay& InitialRay);
		ZEQuaternion					RotationProjection(const ZERay& Ray);

		void							StartScaleProjection(ZEGizmoAxis Axis, const ZERay& InitialRay);
		ZEVector3						ScaleProjection(const ZERay& Ray);

		virtual void					Draw(ZEDrawParameters* DrawParameters);

		static ZEGizmo*					CreateInstance();
};

#endif
