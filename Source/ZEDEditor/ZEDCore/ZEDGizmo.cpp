//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDGizmo.cpp
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

#include "ZEDGizmo.h"

#include "ZETypes.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEPlane.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZELineSegment.h"

#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNView.h"
#include "ZERenderer/ZERNStageID.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNSimpleMaterial.h"

#define ZED_GDF_GIZMO				0x01
#define ZED_GDF_VERTEX_BUFFER		0x02
#define ZED_GDF_CONSTANT_BUFFER		0x04

bool ZEDGizmo::DrawMoveGizmo(const ZERNView& View)
{
	float AxisLength = this->AxisLength * ZEVector3::Distance(View.Position, GetWorldPosition()) * View.ProjectionTransform.M11;
	float AxisLength_2 = AxisLength * 0.5f;
	float AxisLength_10 = AxisLength * 0.1f;
	float AxisLength_5 = AxisLength * 0.2f;

	GizmoLines.Clean();
	GizmoTriangles.Clean();

	// X-Axis
	ZEVector4 XAxisColor = (HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_XZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(AxisLength, 0.0f, 0.0f));

	GizmoTriangles.SetColor(XAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3(0.0f, 0.0f, 1.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(AxisLength - AxisLength_5, 0.0f, 0.0f));
	GizmoTriangles.AddPyramid(AxisLength_10, AxisLength_5, AxisLength_10);

	// Y-Axis
	ZEVector4 YAxisColor = (HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, AxisLength, 0.0f));

	GizmoTriangles.SetColor(YAxisColor);
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, AxisLength - AxisLength_5, 0.0f));
	GizmoTriangles.SetRotation(ZEQuaternion::Identity);
	GizmoTriangles.AddPyramid(AxisLength_10, AxisLength_5, AxisLength_10);

	// Z-Axis
	ZEVector4 ZAxisColor = (HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_XZ_AXIS || HoveredAxis == ZED_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, 0.0f, AxisLength));

	GizmoTriangles.SetColor(ZAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, 0.0f, AxisLength - AxisLength_5));
	GizmoTriangles.AddPyramid(AxisLength_10, AxisLength_5, AxisLength_10);


	GizmoLines.PushTransformation();

	GizmoLines.SetRotation(View.Rotation * GetRotation().Conjugate());

	// X Letter
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.SetTranslation(ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, -0.5f, 0.0f), ZEVector3(-0.25f, 0.5f, 0.0f));

	// Y Letter
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, -0.5f, 0.0f));

	// Z Letter
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(-0.25f, -0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, -0.5f, 0.0f));

	GizmoLines.PopTransformation();

	// XY-Axis
	GizmoLines.SetColor(HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XY_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength_2, AxisLength_2, 0.0f));

	GizmoLines.SetColor(HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_XY_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(AxisLength_2, AxisLength_2, 0.0f));

	if (HoveredAxis == ZED_GA_XY_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength_2, AxisLength_2, 0.0f), ZEVector3(0.0f, AxisLength_2, 0.0f));
	}


	// XZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XZ_AXIS ?	ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength_2, 0.0f, AxisLength_2));

	GizmoLines.SetColor(HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_XZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLength_2), ZEVector3(AxisLength_2, 0.0f, AxisLength_2));

	if (HoveredAxis == ZED_GA_XZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength_2, 0.0f, AxisLength_2), ZEVector3(0.0f, 0.0f, AxisLength_2));
	}			


	// YZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(0.0f, AxisLength_2, AxisLength_2));

	GizmoLines.SetColor(HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLength_2), ZEVector3(0.0f, AxisLength_2, AxisLength_2));

	if (HoveredAxis == ZED_GA_YZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(0.0f, AxisLength_2, AxisLength_2), ZEVector3(0.0f, 0.0f, AxisLength_2));
	}	

	return true;
}

bool ZEDGizmo::DrawRotateGizmo(const ZERNView& View)
{
	const ZEVector3& Position = GetWorldPosition();

	float AxisLength = this->AxisLength * ZEVector3::Distance(View.Position, Position) * View.ProjectionTransform.M11;
	float AxisLength_2 = AxisLength * 0.5f;
	float AxisLength_10 = AxisLength * 0.1f;
	float AxisLength_4 = AxisLength * 0.25f;

	GizmoLines.Clean();
	GizmoTriangles.Clean();

	const ZEMatrix4x4& Transformation = GetWorldTransform();
	const ZEQuaternion& Rotation = GetRotation();

	ZEVector3 ViewDirection;
	ZEQuaternion::VectorProduct(ViewDirection, View.Rotation, ZEVector3::UnitZ);
	ZEVector3::Scale(ViewDirection, ViewDirection, -1.0f);

	// General Circle 
	GizmoLines.SetRotation(View.Rotation * ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
	GizmoLines.SetTranslation(Position);
	GizmoLines.SetColor(ZEVector4(0.2f ,0.2f, 0.2f, 1.0f));
	GizmoLines.AddCircle(AxisLength, 32);
	GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddCircle(AxisLength + AxisLength_4, 32);

	// Small Axis
	GizmoLines.PushTransformation();
	GizmoLines.SetTransfomation(GetWorldTransform());

	// Small Axis X
	if (HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_XZ_AXIS)
		GizmoLines.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	else	
		GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(1.0f, 0.0f, 0.0f) * AxisLength_2);

	// Small Axis X Letter
	ZEVector3 LetterPosition = ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLength_2 + AxisLength_10);

	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, -0.5f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLength_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.25f, -0.5f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(-0.25f, 0.5f, 0.0f) * AxisLength_10);

	// Small Axis Y
	if (HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_YZ_AXIS)
		GizmoLines.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	else	
		GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 1.0f, 0.0f) * AxisLength_2);

	// Small Axis Y Letter
	LetterPosition = ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLength_2 + AxisLength_10);

	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, 0.5f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(0.0f, 0.0f, 0.0f) * AxisLength_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(0.0f, 0.0f, 0.0f) * AxisLength_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.0f, 0.0f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(0.0f, -0.5f, 0.0f) * AxisLength_10);

	// Small Axis Z
	if (HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_XZ_AXIS || HoveredAxis == ZED_GA_YZ_AXIS)
		GizmoLines.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	else	
		GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 0.0f, 1.0f) * AxisLength_2);

	// Small Axis Z Letter
	LetterPosition = ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLength_2 + AxisLength_10);

	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, 0.5f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLength_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(-0.25f, -0.5f, 0.0f) * AxisLength_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, -0.5f, 0.0f) * AxisLength_10, LetterPosition + ZEVector3(0.25f, -0.5f, 0.0f) * AxisLength_10);

	GizmoLines.PopTransformation();

	// Axis circles
	float HAngle = ZE_PIx2 / 64;

	// X Axis
	ZEVector3 AxisUp;
	ZEQuaternion::VectorProduct(AxisUp, Rotation, ZEVector3::UnitX);
	bool FullDraw =  (ZEMath::Abs(ZEVector3::DotProduct(AxisUp, ViewDirection)) > 0.98);

	ZEVector3 LineStart, LineEnd;
	for (ZESize X = 0; X < 64; X++)
	{
		ZEMatrix4x4::Transform(LineStart, Transformation, ZEVector3(0.0f, AxisLength * ZEAngle::Sin(X * HAngle), AxisLength * ZEAngle::Cos(X * HAngle)));
		ZEMatrix4x4::Transform(LineEnd, Transformation, ZEVector3(0.0f, AxisLength * ZEAngle::Sin((X + 1) * HAngle), AxisLength * ZEAngle::Cos((X + 1) * HAngle)));

		bool Backface = (!FullDraw && 
			ZEVector3::DotProduct((LineStart - Position).Normalize(), ViewDirection) < -0.01f &&
			ZEVector3::DotProduct((LineEnd - Position).Normalize(), ViewDirection) < -0.01f);

		if (HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_XZ_AXIS)
		{
			ZECanvasVertex* Verts = GizmoLines.AddVertices(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;

			if (!Backface)		
				Verts[0].Color = Verts[1].Color = ZEVector4(1.0f, 1.0f, 0.0f, 1.0f);
			else
				Verts[0].Color = Verts[1].Color = ZEVector4(0.3f, 0.3f, 0.3f, 1.0f);		
		}
		else
		{
			if (Backface)
				continue;

			ZECanvasVertex* Verts = GizmoLines.AddVertices(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;
			Verts[0].Color = Verts[1].Color = ZEVector4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	// Y Axis
	ZEQuaternion::VectorProduct(AxisUp, Rotation, ZEVector3::UnitY);
	FullDraw =  (ZEMath::Abs(ZEVector3::DotProduct(AxisUp, ViewDirection)) > 0.98);

	for (ZESize X = 0; X < 64; X++)
	{
		ZEMatrix4x4::Transform(LineStart, Transformation, ZEVector3(AxisLength * ZEAngle::Sin(X * HAngle), 0.0f, AxisLength * ZEAngle::Cos(X * HAngle)));
		ZEMatrix4x4::Transform(LineEnd, Transformation, ZEVector3(AxisLength * ZEAngle::Sin((X + 1) * HAngle), 0.0f, AxisLength * ZEAngle::Cos((X + 1) * HAngle)));

		bool Backface = (!FullDraw && 
			ZEVector3::DotProduct((LineStart - Position).Normalize(), ViewDirection) < -0.01f &&
			ZEVector3::DotProduct((LineEnd - Position).Normalize(), ViewDirection) < -0.01f);

		if (HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_YZ_AXIS)
		{
			ZECanvasVertex* Verts = GizmoLines.AddVertices(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;

			if (!Backface)		
				Verts[0].Color = Verts[1].Color = ZEVector4(1.0f, 1.0f, 0.0f, 1.0f);
			else
				Verts[0].Color = Verts[1].Color = ZEVector4(0.3f, 0.3f, 0.3f, 1.0f);		
		}
		else
		{
			if (Backface)
				continue;

			ZECanvasVertex* Verts = GizmoLines.AddVertices(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;
			Verts[0].Color = Verts[1].Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}


	// Z Axis
	ZEQuaternion::VectorProduct(AxisUp, Rotation, ZEVector3::UnitZ);
	FullDraw =  (ZEMath::Abs(ZEVector3::DotProduct(AxisUp, ViewDirection)) > 0.98);

	for (ZESize X = 0; X < 64; X++)
	{
		ZEMatrix4x4::Transform(LineStart, Transformation, ZEVector3(AxisLength * ZEAngle::Sin(X * HAngle), AxisLength * ZEAngle::Cos(X * HAngle), 0.0f));
		ZEMatrix4x4::Transform(LineEnd, Transformation, ZEVector3(AxisLength * ZEAngle::Sin((X + 1) * HAngle), AxisLength * ZEAngle::Cos((X + 1) * HAngle), 0.0f));

		bool Backface = (!FullDraw && 
			ZEVector3::DotProduct((LineStart - Position).Normalize(), ViewDirection) < -0.01f &&
			ZEVector3::DotProduct((LineEnd - Position).Normalize(), ViewDirection) < -0.01f);

		if (HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_XZ_AXIS || HoveredAxis == ZED_GA_YZ_AXIS)
		{
			ZECanvasVertex* Verts = GizmoLines.AddVertices(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;

			if (!Backface)		
				Verts[0].Color = Verts[1].Color = ZEVector4(1.0f, 1.0f, 0.0f, 1.0f);
			else
				Verts[0].Color = Verts[1].Color = ZEVector4(0.3f, 0.3f, 0.3f, 1.0f);		
		}
		else
		{
			if (Backface)
				continue;

			ZECanvasVertex* Verts = GizmoLines.AddVertices(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;
			Verts[0].Color = Verts[1].Color = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	return true;
}

bool ZEDGizmo::DrawScaleGizmo(const ZERNView& View)
{
	float AxisLength = this->AxisLength * ZEVector3::Distance(View.Position, GetWorldPosition()) * View.ProjectionTransform.M11;
	float AxisLength_2 = AxisLength * 0.5f;
	float AxisLength_10 = AxisLength * 0.1f;
	float AxisLength_5 = AxisLength * 0.2f;
	float AxisLength_4 = AxisLength * 0.25f;

	GizmoLines.Clean();
	GizmoTriangles.Clean();

	// X-Axis
	ZEVector4 XAxisColor = (HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_XZ_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(AxisLength, 0.0f, 0.0f));

	GizmoTriangles.SetColor(XAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3(0.0f, 0.0f, 1.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(AxisLength - AxisLength_5, 0.0f, 0.0f));
	GizmoTriangles.AddPyramid(AxisLength_10, AxisLength_5, AxisLength_10);

	// Y-Axis
	ZEVector4 YAxisColor = (HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_YZ_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, AxisLength, 0.0f));

	GizmoTriangles.SetColor(YAxisColor);
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, AxisLength - AxisLength_5, 0.0f));
	GizmoTriangles.SetRotation(ZEQuaternion::Identity);
	GizmoTriangles.AddPyramid(AxisLength_10, AxisLength_5, AxisLength_10);

	// Z-Axis
	ZEVector4 ZAxisColor = (HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_XZ_AXIS || HoveredAxis == ZED_GA_YZ_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, 0.0f, AxisLength));

	GizmoTriangles.SetColor(ZAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, 0.0f, AxisLength - AxisLength_5));
	GizmoTriangles.AddPyramid(AxisLength_10, AxisLength_5, AxisLength_10);


	GizmoLines.PushTransformation();

	GizmoLines.SetRotation(View.Rotation * GetRotation().Conjugate());

	// X Letter
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.SetTranslation(ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, -0.5f, 0.0f), ZEVector3(-0.25f, 0.5f, 0.0f));

	// Y Letter
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, -0.5f, 0.0f));

	// Z Letter
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(-0.25f, -0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, -0.5f, 0.0f));

	GizmoLines.PopTransformation();

	// XY-Axis
	GizmoLines.SetColor(HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength_4, AxisLength_4, 0.0f));
	GizmoLines.AddLine(ZEVector3(AxisLength * 0.7f, 0.0f, 0.0f), ZEVector3(AxisLength * 0.35f, AxisLength * 0.35f, 0.0f));

	GizmoLines.SetColor(HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_XY_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(AxisLength_4, AxisLength_4, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLength * 0.7f, 0.0f), ZEVector3(AxisLength * 0.35f, AxisLength * 0.35f, 0.0f));

	if (HoveredAxis == ZED_GA_XY_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength * 0.7f, 0.0f, 0.0f), 
			ZEVector3(0.0f, AxisLength * 0.7f, 0.0f), ZEVector3(0.0f, AxisLength_2, 0.0f));
	}


	// XZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZED_GA_X_AXIS || HoveredAxis == ZED_GA_XZ_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength_4, 0.0f, AxisLength_4));
	GizmoLines.AddLine(ZEVector3(AxisLength * 0.7f, 0.0f, 0.0f), ZEVector3(AxisLength * 0.35f, 0.0f, AxisLength * 0.35f));

	GizmoLines.SetColor(HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_XZ_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLength_2), ZEVector3(AxisLength_4, 0.0f, AxisLength_4));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLength * 0.7f), ZEVector3(AxisLength * 0.35f, 0.0f, AxisLength * 0.35f));

	if (HoveredAxis == ZED_GA_XZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(AxisLength * 0.7f, 0.0f, 0.0f), 
			ZEVector3(0.0f, 0.0f, AxisLength * 0.7f), ZEVector3(0.0f, 0.0f, AxisLength_2));
	}			


	// YZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZED_GA_Y_AXIS || HoveredAxis == ZED_GA_YZ_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(0.0f, AxisLength_4, AxisLength_4));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLength * 0.7f, 0.0f), ZEVector3(0.0f, AxisLength * 0.35f, AxisLength * 0.35f));

	GizmoLines.SetColor(HoveredAxis == ZED_GA_Z_AXIS || HoveredAxis == ZED_GA_YZ_AXIS || HoveredAxis == ZED_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLength_2), ZEVector3(0.0f, AxisLength_4, AxisLength_4));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLength * 0.7f), ZEVector3(0.0f, AxisLength * 0.35f, AxisLength * 0.35f));

	if (HoveredAxis == ZED_GA_YZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(0.0f, AxisLength * 0.7f, 0.0f), 
			ZEVector3(0.0f, 0.0f, AxisLength * 0.7f), ZEVector3(0.0f, 0.0f, AxisLength_2));
	}

	if (HoveredAxis == ZED_GA_XYZ_AXIS)
	{
		GizmoTriangles.SetTransfomation(ZEMatrix4x4::Identity);
		GizmoTriangles.AddTriangle(ZEVector3::Zero, ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(0.0f, AxisLength_2, 0.0f));
		GizmoTriangles.AddTriangle(ZEVector3::Zero, ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(0.0f, 0.0f, AxisLength_2));
		GizmoTriangles.AddTriangle(ZEVector3::Zero, ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(0.0f, 0.0f, AxisLength_2));
		GizmoTriangles.AddTriangle(ZEVector3(AxisLength_2, 0.0f, 0.0f), ZEVector3(0.0f, AxisLength_2, 0.0f), ZEVector3(0.0f, 0.0f, AxisLength_2));
	}

	return true;
}

bool ZEDGizmo::DrawHelperGizmo(const ZERNView& View)
{
	const ZEVector3& CameraPosition = View.Position;
	const ZEQuaternion& CameraRotation = View.Rotation;
	const ZEMatrix4x4& ProjectionTransform = View.ProjectionTransform;

	float AxisLength = this->AxisLength * ZEVector3::Distance(CameraPosition, GetWorldPosition()) * ProjectionTransform.M11;
	float AxisLength_5 = AxisLength * 0.2f;
	float AxisLength_10 = AxisLength * 0.1f;

	GizmoTriangles.Clean();
	GizmoLines.Clean();

	// Axis X
	GizmoLines.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(1.0f, 0.0f, 0.0f) * AxisLength);

	// Axis Y
	GizmoLines.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 1.0f, 0.0f) * AxisLength);

	// Small Axis Z
	GizmoLines.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 0.0f, 1.0f) * AxisLength);

	GizmoLines.SetRotation(CameraRotation * GetRotation().Conjugate());

	// X Letter
	GizmoLines.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.SetTranslation(ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, -0.5f, 0.0f), ZEVector3(-0.25f, 0.5f, 0.0f));


	// Y Letter
	GizmoLines.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.SetTranslation(ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, -0.5f, 0.0f));

	// Z Letter
	GizmoLines.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.SetTranslation(ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLength + AxisLength_5));
	GizmoLines.SetScale(ZEVector3(AxisLength_10, AxisLength_10, AxisLength_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(-0.25f, -0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, -0.5f, 0.0f));

	return true;
}

bool ZEDGizmo::DrawGizmo(const ZERNView& View)
{
	/*if (!DirtyGizmoFlags.GetFlags(ZED_GDF_GIZMO))
		return true;*/

	switch(Mode)
	{
		case ZED_GM_MOVE:
			if (!DrawMoveGizmo(View))
				return false;
			break;

		case ZED_GM_ROTATE:
			if (!DrawRotateGizmo(View))
				return false;
			break;

		case ZED_GM_SCALE:
			if (!DrawScaleGizmo(View))
				return false;
			break;

		case ZED_GM_HELPER:
			if (!DrawHelperGizmo(View))
				return false;
			break;
	}

	DirtyGizmoFlags.RaiseFlags(ZED_GDF_VERTEX_BUFFER);
	DirtyGizmoFlags.UnraiseFlags(ZED_GDF_GIZMO);

	return true;
}
	
bool ZEDGizmo::UpdateVertexBuffer()
{
	if (!DirtyGizmoFlags.GetFlags(ZED_GDF_VERTEX_BUFFER))
		return true;

	ZESize VertexBufferSize = GizmoLines.GetBufferSize() + GizmoTriangles.GetBufferSize();
	if (VertexBufferSize == 0)
		return false;

	if (VertexBuffer.IsNull() ||
		VertexBuffer->GetSize() < VertexBufferSize)
	{
		VertexBuffer = ZEGRVertexBuffer::CreateResource(VertexBufferSize / sizeof(ZECanvasVertex), sizeof(ZECanvasVertex));
		if (VertexBuffer.IsNull())
			return false;
	}

	void* Buffer;
	VertexBuffer->Lock(&Buffer);
		memcpy(Buffer, GizmoLines.GetBuffer(), GizmoLines.GetBufferSize());
		memcpy((ZEBYTE*)Buffer + GizmoLines.GetBufferSize(), GizmoTriangles.GetBuffer(), GizmoTriangles.GetBufferSize());
	VertexBuffer->Unlock();

	DirtyGizmoFlags.UnraiseFlags(ZED_GDF_VERTEX_BUFFER);

	return true;
}

bool ZEDGizmo::UpdateConstantBuffer()
{
	if (!DirtyGizmoFlags.GetFlags(ZED_GDF_CONSTANT_BUFFER))
		return true;

	if (ConstantBuffer.IsNull())
		ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));

	if (GetMode() == ZED_GM_ROTATE)
		ConstantBuffer->SetData(&ZEMatrix4x4::Identity);
	else
		ConstantBuffer->SetData(&GetWorldTransform());

	DirtyGizmoFlags.UnraiseFlags(ZED_GDF_CONSTANT_BUFFER);

	return true;
}

bool ZEDGizmo::Update()
{
	if (!UpdateConstantBuffer())
		return false;

	if (!UpdateVertexBuffer())
		return false;

	return true;
}

ZEVector3 ZEDGizmo::MoveProjectionInternal(const ZERNView& View, ZEDGizmoAxis Axis, const ZERay& Ray)
{
	const ZEVector3& Position = GetWorldPosition();

	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);

	switch(Axis)
	{
		case ZED_GA_X_AXIS:
		{
			ZELine AxisLine;
			AxisLine.p = Position;
			AxisLine.v = Right;

			float TRay, TLine;
			if (ZERay::MinimumDistance(Ray, AxisLine, TRay, TLine) < 10.0f)
				return AxisLine.GetPointOn(TLine);
			else
				return GetWorldPosition();
		}

		case ZED_GA_Y_AXIS:
		{
			ZELine AxisLine;
			AxisLine.p = Position;
			AxisLine.v = Up;

			float TRay, TLine;
			if (ZERay::MinimumDistance(Ray, AxisLine, TRay, TLine) < 10.0f)
				return AxisLine.GetPointOn(TLine);
			else
				return GetWorldPosition();
		}

		case ZED_GA_Z_AXIS:
		{
			ZELine AxisLine;
			AxisLine.p = Position;
			AxisLine.v = Front;

			float TRay, TLine;
			if (ZERay::MinimumDistance(Ray, AxisLine, TRay, TLine) < 10.0f)
				return AxisLine.GetPointOn(TLine);
			else
				return GetWorldPosition();
		}

		case ZED_GA_XY_AXIS:
		{
			ZEPlane AxisPlane(Front, Position);
			float TRay;
			if (ZEPlane::IntersectionTest(AxisPlane, Ray, TRay))
				return Ray.GetPointOn(TRay);
			else
				return GetWorldPosition();
		}

		case ZED_GA_XZ_AXIS:
		{
			ZEPlane AxisPlane(Up, Position);
			float TRay;
			if (ZEPlane::IntersectionTest(AxisPlane, Ray, TRay))
				return Ray.GetPointOn(TRay);
			else
				return GetWorldPosition();
		}

		case ZED_GA_YZ_AXIS:
		{
			ZEPlane AxisPlane(Right, Position);
			float TRay;
			if (ZEPlane::IntersectionTest(AxisPlane, Ray, TRay))
				return Ray.GetPointOn(TRay);
			else
				return GetWorldPosition();
		}

		default:
			return GetWorldPosition();
	}
}

ZEDGizmoAxis ZEDGizmo::PickMoveAxis(const ZERNView& View, const ZERay& Ray, float& TRay)
{
	const ZEVector3& Position = GetWorldPosition();

	float AxisLength = this->AxisLength * ZEVector3::Distance(View.Position, Position) * View.ProjectionTransform.M11;
	float AxisLength_2 = AxisLength * 0.5f;
	float AxisTreshold = AxisLength * 0.1f;

	ZEDGizmoAxis PickedAxis = ZED_GA_NONE;
	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetWorldRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);

	float NewTRay, TLineSegment;

	// Check Planes
	ZEPlane AxisXY(Front, Position);
	if (ZEPlane::IntersectionTest(AxisXY, Ray, NewTRay) && (NewTRay < TRay))
	{
		ZEVector3 TestPos(AxisXY.p, Ray.GetPointOn(NewTRay));
		float ProjU  = ZEVector3::DotProduct(TestPos, Right);
		float ProjV = ZEVector3::DotProduct(TestPos, Up);
		if (ProjU > 0.0f && ProjU < AxisLength_2 && ProjV > 0.0f && ProjV < AxisLength_2)
		{
			TRay = NewTRay;
			PickedAxis = ZED_GA_XY_AXIS;
		}
	}

	ZEPlane AxisXZ(Up, Position);
	if (ZEPlane::IntersectionTest(AxisXZ, Ray, NewTRay) && (NewTRay < TRay))
	{
		ZEVector3 TestPos(AxisXZ.p, Ray.GetPointOn(NewTRay));
		float ProjU  = ZEVector3::DotProduct(TestPos, Right);
		float ProjV = ZEVector3::DotProduct(TestPos, Front);
		if (ProjU > 0.0f && ProjU < AxisLength_2 && ProjV > 0.0f && ProjV < AxisLength_2)
		{
			TRay = NewTRay;
			PickedAxis = ZED_GA_XZ_AXIS;
		}
	}

	ZEPlane AxisYZ(Right, Position);
	if (ZEPlane::IntersectionTest(AxisYZ, Ray, NewTRay) && (NewTRay < TRay))
	{		
		ZEVector3 TestPos(AxisYZ.p, Ray.GetPointOn(NewTRay));
		float ProjU  = ZEVector3::DotProduct(TestPos, Up);
		float ProjV = ZEVector3::DotProduct(TestPos, Front);
		if (ProjU > 0.0f && ProjU < AxisLength_2 && ProjV > 0.0f && ProjV < AxisLength_2)
		{
			TRay = NewTRay;
			PickedAxis = ZED_GA_YZ_AXIS;
		}
	}

	// Check Line Segments
	ZELineSegment XAxis(Position, Position +  1.5f * Right * AxisLength);
	if ((ZELineSegment::MinimumDistance(XAxis, Ray, TLineSegment, NewTRay) < AxisTreshold) && (NewTRay < TRay))
	{
		TRay = NewTRay;
		PickedAxis = ZED_GA_X_AXIS;
	}

	ZELineSegment YAxis(Position, Position +  1.5f * Up * AxisLength);
	if ((ZELineSegment::MinimumDistance(YAxis, Ray, TLineSegment, NewTRay)) < AxisTreshold && (NewTRay < TRay))
	{
		TRay = NewTRay;
		PickedAxis = ZED_GA_Y_AXIS;
	}

	ZELineSegment ZAxis(Position, Position +  1.5f * Front * AxisLength);
	if ((ZELineSegment::MinimumDistance(ZAxis, Ray, TLineSegment, NewTRay) < AxisTreshold) && (NewTRay < TRay))
	{
		TRay = NewTRay;
		PickedAxis = ZED_GA_Z_AXIS;
	}
	
	return PickedAxis;
}

ZEDGizmoAxis ZEDGizmo::PickRotateAxis(const ZERNView& View, const ZERay& Ray, float& TRay)
{
	const ZEVector3& Position = GetWorldPosition();

	float AxisLength = this->AxisLength * ZEVector3::Distance(View.Position, Position) * View.ProjectionTransform.M11;
	float AxisTreshold = AxisLength * 0.1f;

	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetWorldRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);

	float NewTRay;
	ZEDGizmoAxis PickedAxis = ZED_GA_NONE;

	// X Axis
	ZEPlane AxisXPlane(Right, Position);
	if (ZEPlane::IntersectionTest(AxisXPlane, Ray, NewTRay) && (NewTRay < TRay))
	{
		ZEVector3 IntersectionPoint = Ray.GetPointOn(NewTRay);
		float Distance = ZEVector3::Distance(IntersectionPoint, Position);
		if (ZEMath::Abs(Distance - AxisLength) < AxisTreshold &&
			ZEVector3::DotProduct(IntersectionPoint.Normalize(), View.Direction) < 0.0f)
		{
			TRay = NewTRay;
			PickedAxis = ZED_GA_X_AXIS;
		}
	}
	
	ZEPlane AxisYPlane(Up, Position);
	if (ZEPlane::IntersectionTest(AxisYPlane, Ray, NewTRay) && (NewTRay < TRay))
	{
		ZEVector3 IntersectionPoint = Ray.GetPointOn(NewTRay);
		float Distance = ZEVector3::Distance(IntersectionPoint, Position);

		if (ZEMath::Abs(Distance - AxisLength) < AxisTreshold &&
			ZEVector3::DotProduct(IntersectionPoint.Normalize(), View.Direction) < 0.0f)
		{
			TRay = NewTRay;
			PickedAxis = ZED_GA_Y_AXIS;
		}
	}
	
	ZEPlane AxisZPlane(Front, Position);
	if (ZEPlane::IntersectionTest(AxisZPlane, Ray, NewTRay) && (NewTRay < TRay))
	{
		ZEVector3 IntersectionPoint = Ray.GetPointOn(NewTRay);
		float Distance = ZEVector3::Distance(IntersectionPoint, Position);

		if (ZEMath::Abs(Distance - AxisLength) < AxisTreshold) /* &&
			ZEVector3::DotProduct(IntersectionPoint.Normalize(), View.Direction) < 0.0f)*/
		{
			TRay = NewTRay;
			PickedAxis = ZED_GA_Z_AXIS;
		}
	}
	
	return PickedAxis;
}

ZEDGizmoAxis ZEDGizmo::PickScaleAxis(const ZERNView& View, const ZERay& Ray, float& TRay)
{
	const ZEVector3& Position = GetWorldPosition();

	float AxisLength = this->AxisLength * ZEVector3::Distance(View.Position, Position) * View.ProjectionTransform.M11;
	float AxisLength_2 = AxisLength * 0.5f;
	float AxisTreshold = AxisLength * 0.1f;

	ZEDGizmoAxis PickedAxis = ZED_GA_NONE;
	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);

	float NewTRay, TLineSegment;

	// Check Planes
	ZETriangle InnerAxisXY(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLength_2, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, AxisLength_2, 0.0f));
	ZETriangle OuterAxisXY(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLength * 0.7f, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, AxisLength * 0.7f, 0.0f));
	if (ZETriangle::IntersectionTest(OuterAxisXY, Ray, NewTRay) && (NewTRay < TRay))
	{	
		TRay = NewTRay;
		if (ZETriangle::IntersectionTest(InnerAxisXY, Ray, NewTRay))
			PickedAxis = ZED_GA_XYZ_AXIS;
		else
			PickedAxis = ZED_GA_XY_AXIS;
	}

	ZETriangle InnerAxisXZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLength_2, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLength_2));
	ZETriangle OuterAxisXZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLength * 0.7f, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLength * 0.7f));
	if (ZETriangle::IntersectionTest(OuterAxisXZ, Ray, NewTRay) && (NewTRay < TRay))
	{
		TRay = NewTRay;
		if (ZETriangle::IntersectionTest(InnerAxisXZ, Ray, NewTRay))
			PickedAxis = ZED_GA_XYZ_AXIS;
		else
			PickedAxis = ZED_GA_XZ_AXIS;
	}

	ZETriangle InnerAxisYZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(0.0f, AxisLength_2, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLength_2));
	ZETriangle OuterAxisYZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(0.0f, AxisLength * 0.7f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLength * 0.7f));
	if (ZETriangle::IntersectionTest(OuterAxisYZ, Ray, NewTRay) && (NewTRay < TRay))
	{
		TRay = NewTRay;
		if (ZETriangle::IntersectionTest(InnerAxisYZ, Ray, NewTRay))
			PickedAxis = ZED_GA_XYZ_AXIS;
		else
			PickedAxis = ZED_GA_YZ_AXIS;
	}

	// Check Line Segments
	ZELineSegment XAxis(Position, Position + Right * AxisLength);
	if ((ZELineSegment::MinimumDistance(XAxis, Ray, TLineSegment, NewTRay) < AxisTreshold) && (NewTRay < TRay))
	{
		TRay = NewTRay;
		PickedAxis = ZED_GA_X_AXIS;
	}

	ZELineSegment YAxis(Position, Position + Up * AxisLength);
	if ((ZELineSegment::MinimumDistance(YAxis, Ray, TLineSegment, NewTRay) < AxisTreshold) && (NewTRay < TRay))
	{
		TRay = NewTRay;
		PickedAxis = ZED_GA_Y_AXIS;
	}

	ZELineSegment ZAxis(Position, Position + Front * AxisLength);
	if ((ZELineSegment::MinimumDistance(ZAxis, Ray, TLineSegment, NewTRay) < AxisTreshold) && (NewTRay < TRay))
	{
		TRay = NewTRay;
		PickedAxis = ZED_GA_Z_AXIS;
	}
	
	return PickedAxis;
}

void ZEDGizmo::SetMode(ZEDGizmoMode Mode)
{
	if (this->Mode == Mode)
		return;

	this->Mode = Mode;

	DirtyGizmoFlags.RaiseFlags(ZED_GDF_GIZMO);
}

ZEDGizmoMode ZEDGizmo::GetMode()
{
	return Mode;
}

void ZEDGizmo::SetVisible(bool Visibility)
{
	ZEEntity::SetVisible(Visibility);
}

void ZEDGizmo::SetHoveredAxis(ZEDGizmoAxis Axis)
{
	if (HoveredAxis == Axis)
		return;

	HoveredAxis = Axis;

	DirtyGizmoFlags.RaiseFlags(ZED_GDF_GIZMO);
}

ZEDGizmoAxis ZEDGizmo::GetHoveredAxis()
{
	return HoveredAxis;
}

void ZEDGizmo::SetSelectedAxis(ZEDGizmoAxis Axis)
{
	if (SelectedAxis == Axis)
		return;

	SelectedAxis = Axis;

	DirtyGizmoFlags.RaiseFlags(ZED_GDF_GIZMO);
}

ZEDGizmoAxis ZEDGizmo::GetSelectedAxis()
{
	return SelectedAxis;
}

void ZEDGizmo::SetAxisLength(float Size)
{
	if (AxisLength == Size)
		return;

	AxisLength = Size;

	DirtyGizmoFlags.RaiseFlags(ZED_GDF_GIZMO);
}

float ZEDGizmo::GetAxisLength()
{
	return AxisLength;
}

bool ZEDGizmo::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));

	MaterialLines = ZERNSimpleMaterial::CreateInstance();
	MaterialLines->SetPrimitiveType(ZEGR_PT_LINE_LIST);
	MaterialLines->SetTwoSided(true);
	MaterialLines->SetDepthTestDisabled(true);
	MaterialLines->SetStageMask(ZERN_STAGE_FORWARD_POST_HDR);

	MaterialTriangles = ZERNSimpleMaterial::CreateInstance();
	MaterialTriangles->SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	MaterialTriangles->SetTwoSided(true);
	MaterialTriangles->SetDepthTestDisabled(true);
	MaterialTriangles->SetStageMask(ZERN_STAGE_FORWARD_POST_HDR);

	return true;
}

bool ZEDGizmo::DeinitializeSelf()
{	
	GizmoLines.Clean();
	GizmoTriangles.Clean();
	VertexBuffer.Release();
	ConstantBuffer.Release();
	MaterialLines.Release();
	MaterialTriangles.Release();
	DirtyGizmoFlags.RaiseAll();

	return ZEEntity::DeinitializeSelf();
}

void ZEDGizmo::LocalTransformChanged()
{
	DirtyGizmoFlags.RaiseFlags(ZED_GDF_CONSTANT_BUFFER);
	ZEEntity::LocalTransformChanged();
}

void ZEDGizmo::ParentTransformChanged()
{
	DirtyGizmoFlags.RaiseFlags(ZED_GDF_CONSTANT_BUFFER);
	ZEEntity::ParentTransformChanged();

}

bool ZEDGizmo::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return true;
	
	if (!MaterialLines->PreRender(RenderCommand))
		return false;

	if (!DrawGizmo(*Parameters->View))
		return false;

	if (!Update())
		return false;
	
	Parameters->Renderer->AddCommand(&RenderCommand);
	return true;
}

void ZEDGizmo::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (GizmoLines.GetBufferSize() == 0 && GizmoTriangles.GetBufferSize() == 0)
		return;

	ZEGRContext* Context = Parameters->Context;
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->SetVertexBuffer(0, VertexBuffer);
	
	if (GizmoLines.GetBufferSize() != 0)
	{
		if (!MaterialLines->SetupMaterial(Context, Parameters->Stage))
			return;

		Context->Draw(GizmoLines.GetVertexCount(), 0);
		MaterialLines->CleanupMaterial(Context, Parameters->Stage);
	}

	if (GizmoTriangles.GetBufferSize() != 0)
	{
		if (!MaterialTriangles->SetupMaterial(Context, Parameters->Stage))
			return;

		Context->Draw(GizmoTriangles.GetVertexCount(), GizmoLines.GetVertexCount());
		MaterialTriangles->CleanupMaterial(Context, Parameters->Stage);
	}
}

ZEDGizmoAxis ZEDGizmo::PickAxis(const ZERNView& View, const ZERay& Ray, float& TRay)
{
	switch(Mode)
	{
		case ZED_GM_MOVE:
			return PickMoveAxis(View, Ray, TRay);

		case ZED_GM_ROTATE:
			return PickRotateAxis(View, Ray, TRay);

		case ZED_GM_SCALE:
			return PickScaleAxis(View, Ray, TRay);

		default:
			return ZED_GA_NONE;
	}
}

void ZEDGizmo::StartMoveProjection(const ZERNView& View, const ZERay& InitialRay)
{
	ZEVector3 PickPosition = MoveProjectionInternal(View, SelectedAxis, InitialRay);
	PickOffset = PickPosition - GetWorldPosition();
	InitialPosition = GetWorldPosition();
}

ZEVector3 ZEDGizmo::MoveProjection(const ZERNView& View, const ZERay& Ray)
{
	ZEVector3 Translation = MoveProjectionInternal(View, SelectedAxis, Ray) - PickOffset;
	SetPosition(Translation);
	return Translation - InitialPosition;
}

void ZEDGizmo::StartRotationProjection(const ZEQuaternion& InitialRotation)
{
	this->InitialRotation = InitialRotation;
}

ZEQuaternion ZEDGizmo::RotationProjection(const ZERNView& View, float RotationAmount)
{
	ZEVector3 RotationAxis;
	switch (SelectedAxis)
	{
		default:
		case ZED_GA_XY_AXIS:
		case ZED_GA_XZ_AXIS:
		case ZED_GA_YZ_AXIS:
		case ZED_GA_XYZ_AXIS:
		case ZED_GA_NONE:
			return ZEQuaternion::Identity;
			break;

		case ZED_GA_X_AXIS:
			RotationAxis = ZEVector3::UnitX;
			break;

		case ZED_GA_Y_AXIS:
			RotationAxis = ZEVector3::UnitY;
			break;

		case ZED_GA_Z_AXIS:
			RotationAxis = ZEVector3::UnitZ;
			break;

		case ZED_GA_SCREEN_AXIS:
			RotationAxis = View.Rotation * ZEVector3::UnitZ;
			break;
	}

	ZEQuaternion Output;
	ZEQuaternion::CreateFromAngleAxis(Output, RotationAmount, RotationAxis);

	return Output;
}

void ZEDGizmo::StartScaleProjection(const ZEVector3& InitialScale)
{
	this->InitialScale = InitialScale;
}

ZEVector3 ZEDGizmo::ScaleProjection(float ScaleAmount)
{
	ZEVector3 Scale;
	switch (SelectedAxis)
	{
		default:
		case ZED_GA_NONE:
			return ZEVector3::One;

		case ZED_GA_X_AXIS:
			return ZEVector3(1.0f + ScaleAmount, 1.0f, 1.0f);

		case ZED_GA_Y_AXIS:
			return ZEVector3(1.0f, 1.0f + ScaleAmount, 1.0f);

		case ZED_GA_Z_AXIS:
			return ZEVector3(1.0f, 1.0f, 1.0f + ScaleAmount);

		case ZED_GA_XY_AXIS:
			return ZEVector3(1.0f + ScaleAmount, 1.0f + ScaleAmount, 1.0f);

		case ZED_GA_XZ_AXIS:
			return ZEVector3(1.0f + ScaleAmount, 1.0f, 1.0f + ScaleAmount);

		case ZED_GA_YZ_AXIS:
			return ZEVector3(1.0f, 1.0f + ScaleAmount, 1.0f + ScaleAmount);

		case ZED_GA_XYZ_AXIS:
			return ZEVector3(1.0f + ScaleAmount, 1.0f + ScaleAmount, 1.0f + ScaleAmount);
	}
}

ZEDGizmo::ZEDGizmo()
{
	DirtyGizmoFlags.RaiseAll();

	HoveredAxis = ZED_GA_NONE;
	SelectedAxis = ZED_GA_NONE;
	Mode = ZED_GM_NONE;
	AxisLength = 0.1f;

	PickOffset = ZEVector3::Zero;
	InitialRotation = ZEQuaternion::Identity;
	InitialScale = ZEVector3::One;

	RenderCommand.Entity = this;

	SetEntityFlags(ZE_EF_RENDERABLE);
}

ZEDGizmo* ZEDGizmo::CreateInstance()
{
	return new ZEDGizmo();
}
