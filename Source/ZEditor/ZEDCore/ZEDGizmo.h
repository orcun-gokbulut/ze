//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDGizmo.h
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

#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZECanvas.h"
#include "ZEGraphics/ZERenderCommand.h"

enum ZEDGizmoMode
{
	ZED_GM_NONE,
	ZED_GM_MOVE,
	ZED_GM_ROTATE,
	ZED_GM_SCALE,
	ZED_GM_HELPER
};

enum ZEDGizmoAxis
{
	ZED_GA_NONE,
	ZED_GA_X_AXIS,
	ZED_GA_Y_AXIS,
	ZED_GA_Z_AXIS,
	ZED_GA_XY_AXIS,
	ZED_GA_XZ_AXIS,
	ZED_GA_YZ_AXIS,
	ZED_GA_XYZ_AXIS,
	ZED_GA_SCREEN_AXIS
};

class ZESimpleMaterial;

class ZEDGizmo : public ZEEntity
{
	private:
		ZECanvas GizmoLines;
		ZECanvas GizmoTriangles;
		bool DirtyGizmoFlag;

		float AxisLength;

		static ZESimpleMaterial* GizmoMaterial;
		ZERenderCommand	RenderCommand;

		ZEDGizmoMode Mode;
		ZEDGizmoAxis HoveredAxis;
		ZEDGizmoAxis SelectedAxis;

		void UpdateMoveGizmo();
		void UpdateRotateGizmo();
		void UpdateScaleGizmo();
		void UpdateHelperGizmo();

		// Projection
		ZEVector3 MoveDifference;
		ZEQuaternion InitialRotation;
		ZEVector3 InitialScale;

		ZEDGizmoAxis PickMoveAxis(const ZERay& Ray, float& TRay);
		ZEDGizmoAxis PickRotateAxis(const ZERay& Ray, float& TRay);
		ZEDGizmoAxis PickScaleAxis(const ZERay& Ray, float& TRay);

		ZEVector3 OldPosition;
		ZEVector2 StartScreenPosition;
					 
		void UpdateGizmo();
					 
		ZEVector3 MoveProjection_(ZEDGizmoAxis Axis, const ZERay& Ray);
		ZEQuaternion RotationProjection_(ZEDGizmoAxis Axis, const ZERay& Ray);
		ZEVector3 ScaleProjection_(ZEDGizmoAxis Axis, const ZERay& Ray);
					 
		virtual bool InitializeSelf();
		virtual bool DeinitializeSelf();

		ZEDGizmo();

	public:
		virtual ZEDrawFlags	GetDrawFlags() const;
	
		virtual void SetPosition(const ZEVector3& NewPosition);
		virtual void SetRotation(const ZEQuaternion& NewRotation);
	
		void SetMode(ZEDGizmoMode Mode);
		ZEDGizmoMode GetMode();
	
		void SetHoveredAxis(ZEDGizmoAxis HoveredAxis);
		ZEDGizmoAxis GetHoveredAxis();

		void SetSelectedAxis(ZEDGizmoAxis SelectedAxis);
		ZEDGizmoAxis GetSelectedAxis();
	
		void SetAxisLength(float Size);
		float GetAxisLength();
	
		ZEDGizmoAxis PickAxis(const ZERay& Ray, float& TRay);
	
		void StartMoveProjection(const ZERay& InitialRay);
		ZEVector3 MoveProjection(const ZERay& Ray);
	
		void StartRotationProjection(const ZERay& InitialRay);
		ZEQuaternion RotationProjection(const ZERay& Ray);
	
		void StartScaleProjection(const ZERay& InitialRay);
		ZEVector3 ScaleProjection(const ZERay& Ray);
	
		virtual void Draw(ZEDrawParameters* DrawParameters);
	
		static ZEDGizmo* CreateInstance();
};
