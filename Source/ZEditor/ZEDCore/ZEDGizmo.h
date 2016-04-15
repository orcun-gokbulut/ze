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

#include "ZERenderer/ZECanvas.h"
#include "ZERenderer/ZERNCommand.h"

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

class ZEGRVertexBuffer;
class ZEGRConstantBuffer;
class ZERNSimpleMaterial;
class ZERNView;

class ZEDGizmo : public ZEEntity
{
	private:
		bool							DirtyGizmoFlag;
		float							AxisLength;

		ZEDGizmoMode					Mode;
		ZEDGizmoAxis					HoveredAxis;
		ZEDGizmoAxis					SelectedAxis;

		ZEVector3						MoveDifference;
		ZEQuaternion					InitialRotation;
		ZEVector3						InitialScale;

		ZEVector3						OldPosition;
		ZEVector2						StartScreenPosition;

		ZECanvas						GizmoLines;
		ZECanvas						GizmoTriangles;
		
		ZEHolder<ZEGRVertexBuffer>		VertexBuffer;
		ZEHolder<ZEGRConstantBuffer>	ConstantBuffer;
		ZEHolder<ZERNSimpleMaterial>	MaterialLines;
		ZEHolder<ZERNSimpleMaterial>	MaterialTriangles;

		ZERNCommand						RenderCommand;

		ZEDGizmoAxis					PickMoveAxis(const ZERNView& View, const ZERay& Ray, float& TRay);
		ZEDGizmoAxis					PickRotateAxis(const ZERNView& View, const ZERay& Ray, float& TRay);
		ZEDGizmoAxis					PickScaleAxis(const ZERNView& View, const ZERay& Ray, float& TRay);

		ZEVector3						MoveProjectionInternal(const ZERNView& View, ZEDGizmoAxis Axis, const ZERay& Ray);
		ZEQuaternion					RotationProjectionInternal(const ZERNView& View, ZEDGizmoAxis Axis, const ZERay& Ray);
		ZEVector3						ScaleProjectionInternal(const ZERNView& View, ZEDGizmoAxis Axis, const ZERay& Ray);

		void							UpdateMoveGizmo(const ZERNView& View);
		void							UpdateRotateGizmo(const ZERNView& View);
		void							UpdateScaleGizmo(const ZERNView& View);
		void							UpdateHelperGizmo(const ZERNView& View);				 
		void							UpdateGizmo(const ZERNView& View);		 

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

		virtual void					LocalTransformChanged();

										ZEDGizmo();

	public:
		virtual void					SetPosition(const ZEVector3& NewPosition);
		virtual void					SetRotation(const ZEQuaternion& NewRotation);
	
		void							SetMode(ZEDGizmoMode Mode);
		ZEDGizmoMode					GetMode();
	
		void							SetHoveredAxis(ZEDGizmoAxis HoveredAxis);
		ZEDGizmoAxis					GetHoveredAxis();

		void							SetSelectedAxis(ZEDGizmoAxis SelectedAxis);
		ZEDGizmoAxis					GetSelectedAxis();
	
		void							SetAxisLength(float Size);
		float							GetAxisLength();
	
		ZEDGizmoAxis					PickAxis(const ZERNView& View, const ZERay& Ray, float& TRay);
	
		void							StartMoveProjection(const ZERNView& View, const ZERay& InitialRay);
		ZEVector3						MoveProjection(const ZERNView& View, const ZERay& Ray);
	
		void							StartRotationProjection(const ZERNView& View, const ZERay& InitialRay);
		ZEQuaternion					RotationProjection(const ZERNView& View, const ZERay& Ray);
	
		void							StartScaleProjection(const ZERNView& View, const ZERay& InitialRay);
		ZEVector3						ScaleProjection(const ZERNView& View, const ZERay& Ray);
	
		virtual bool					PreRender(const ZERNCullParameters* CullParameters);
		virtual void					Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);
	
		static ZEDGizmo*				CreateInstance();
};
