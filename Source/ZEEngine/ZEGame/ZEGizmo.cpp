//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGizmo.cpp
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


#include "ZETypes.h"
#include "ZEGizmo.h"
#include "ZEMath/ZERay.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEPlane.h"
#include "ZECore/ZEConsole.h"
#include "ZEMath/ZETriangle.h"
#include "ZEGraphics/ZECanvas.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEMath/ZEMath.h"
#include "ZEGraphics/ZESimpleMaterial.h"

ZESimpleMaterial* ZEGizmo::GizmoMaterial = NULL;

void ZEGizmo::UpdateMoveGizmo()
{
	ZEVector3 CameraPosition = zeScene->GetActiveCamera()->GetWorldPosition();
	ZEQuaternion CameraRotation = zeScene->GetActiveCamera()->GetWorldRotation();
	
	static ZEVector3 OldCameraPosition = ZEVector3::Zero;
	static ZEQuaternion OldCameraRotation = ZEQuaternion::Identity;

	if (!DirtyGizmoFlag && CameraPosition == OldCameraPosition && CameraRotation == OldCameraRotation)
		return;

	float AxisLenght = this->AxisLenght * ZEVector3::Distance(CameraPosition, GetPosition()) * zeScene->GetActiveCamera()->GetProjectionTransform().M11;
	float AxisLenght_2 = AxisLenght * 0.5f;
	float AxisLenght_10 = AxisLenght * 0.1f;
	float AxisLenght_5 = AxisLenght * 0.2f;

	GizmoLines.Clean();
	GizmoTriangles.Clean();

	// X-Axis
	ZEVector4 XAxisColor = (HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_XZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(AxisLenght, 0.0f, 0.0f));

	GizmoTriangles.SetColor(XAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3(0.0f, 0.0f, 1.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(AxisLenght - AxisLenght_5, 0.0f, 0.0f));
	GizmoTriangles.AddPyramid(AxisLenght_10, AxisLenght_5, AxisLenght_10);

	// Y-Axis
	ZEVector4 YAxisColor = (HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, AxisLenght, 0.0f));

	GizmoTriangles.SetColor(YAxisColor);
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, AxisLenght - AxisLenght_5, 0.0f));
	GizmoTriangles.SetRotation(ZEQuaternion::Identity);
	GizmoTriangles.AddPyramid(AxisLenght_10, AxisLenght_5, AxisLenght_10);

	// Z-Axis
	ZEVector4 ZAxisColor = (HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_XZ_AXIS || HoveredAxis == ZE_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, 0.0f, AxisLenght));

	GizmoTriangles.SetColor(ZAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, 0.0f, AxisLenght - AxisLenght_5));
	GizmoTriangles.AddPyramid(AxisLenght_10, AxisLenght_5, AxisLenght_10);


	GizmoLines.PushTransformation();

	GizmoLines.SetRotation(CameraRotation * GetRotation().Conjugate());

	// X Letter
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.SetTranslation(ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, -0.5f, 0.0f), ZEVector3(-0.25f, 0.5f, 0.0f));

	// Y Letter
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, -0.5f, 0.0f));

	// Z Letter
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(-0.25f, -0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, -0.5f, 0.0f));

	GizmoLines.PopTransformation();

	// XY-Axis
	GizmoLines.SetColor(HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XY_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght_2, AxisLenght_2, 0.0f));

	GizmoLines.SetColor(HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_XY_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(AxisLenght_2, AxisLenght_2, 0.0f));
	
	if (HoveredAxis == ZE_GA_XY_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght_2, AxisLenght_2, 0.0f), ZEVector3(0.0f, AxisLenght_2, 0.0f));
	}


	// XZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XZ_AXIS ?	ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght_2, 0.0f, AxisLenght_2));

	GizmoLines.SetColor(HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_XZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLenght_2), ZEVector3(AxisLenght_2, 0.0f, AxisLenght_2));
	
	if (HoveredAxis == ZE_GA_XZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght_2, 0.0f, AxisLenght_2), ZEVector3(0.0f, 0.0f, AxisLenght_2));
	}			
	

	// YZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(0.0f, AxisLenght_2, AxisLenght_2));

	GizmoLines.SetColor(HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_YZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLenght_2), ZEVector3(0.0f, AxisLenght_2, AxisLenght_2));

	if (HoveredAxis == ZE_GA_YZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(0.0f, AxisLenght_2, AxisLenght_2), ZEVector3(0.0f, 0.0f, AxisLenght_2));
	}	

	DirtyGizmoFlag = false;
}

void ZEGizmo::UpdateRotateGizmo()
{
	ZEVector3 CameraPosition = zeScene->GetActiveCamera()->GetWorldPosition();
	ZEQuaternion CameraRotation = zeScene->GetActiveCamera()->GetWorldRotation();
	
	static ZEVector3 OldCameraPosition = ZEVector3::Zero;
	static ZEQuaternion OldCameraRotation = ZEQuaternion::Identity;

	if (!DirtyGizmoFlag && CameraPosition == OldCameraPosition && CameraRotation == OldCameraRotation)
		return;


	float AxisLenght = this->AxisLenght * ZEVector3::Distance(CameraPosition, GetPosition()) * zeScene->GetActiveCamera()->GetProjectionTransform().M11;
	float AxisLenght_2 = AxisLenght * 0.5f;
	float AxisLenght_10 = AxisLenght * 0.1f;
	float AxisLenght_4 = AxisLenght * 0.25f;

	OldCameraPosition = CameraPosition;
	OldCameraRotation = CameraRotation;

	const ZEVector3& Position = GetPosition();

	GizmoLines.Clean();
	GizmoTriangles.Clean();

	const ZEMatrix4x4& Transformation = GetWorldTransform();
	const ZEQuaternion& Rotation = GetRotation();

	ZEVector3 CameraDirection;
	ZEQuaternion::VectorProduct(CameraDirection, CameraRotation, ZEVector3::UnitZ);
	ZEVector3::Scale(CameraDirection, CameraDirection, -1.0f);

	// General Circle 
	GizmoLines.SetRotation(CameraRotation * ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
	GizmoLines.SetTranslation(GetPosition());
	GizmoLines.SetColor(ZEVector4(0.2f ,0.2f, 0.2f, 1.0f));
	GizmoLines.AddCircle(AxisLenght, 32);
	GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddCircle(AxisLenght + AxisLenght_4, 32);

	// Small Axis
	GizmoLines.PushTransformation();
	GizmoLines.SetTransfomation(GetWorldTransform());

	// Small Axis X
	if (HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_XZ_AXIS)
		GizmoLines.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	else	
		GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(1.0f, 0.0f, 0.0f) * AxisLenght_2);

	// Small Axis X Letter
	ZEVector3 LetterPosition = ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLenght_2 + AxisLenght_10);

	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, -0.5f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLenght_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.25f, -0.5f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(-0.25f, 0.5f, 0.0f) * AxisLenght_10);

	// Small Axis Y
	if (HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_YZ_AXIS)
		GizmoLines.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	else	
		GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 1.0f, 0.0f) * AxisLenght_2);

	// Small Axis Y Letter
	LetterPosition = ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLenght_2 + AxisLenght_10);

	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, 0.5f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(0.0f, 0.0f, 0.0f) * AxisLenght_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(0.0f, 0.0f, 0.0f) * AxisLenght_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.0f, 0.0f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(0.0f, -0.5f, 0.0f) * AxisLenght_10);

	// Small Axis Z
	if (HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_XZ_AXIS || HoveredAxis == ZE_GA_YZ_AXIS)
		GizmoLines.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	else	
		GizmoLines.SetColor(ZEVector4(0.3f, 0.3f, 0.3f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 0.0f, 1.0f) * AxisLenght_2);

	// Small Axis Z Letter
	LetterPosition = ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLenght_2 + AxisLenght_10);

	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, 0.5f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLenght_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(0.25f, 0.5f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(-0.25f, -0.5f, 0.0f) * AxisLenght_10);
	GizmoLines.AddLine(LetterPosition + ZEVector3(-0.25f, -0.5f, 0.0f) * AxisLenght_10, LetterPosition + ZEVector3(0.25f, -0.5f, 0.0f) * AxisLenght_10);

	GizmoLines.PopTransformation();

	// Axis circles
	float HAngle = ZE_PIx2 / 64;

	// X Axis
	ZEVector3 AxisUp;
	ZEQuaternion::VectorProduct(AxisUp, Rotation, ZEVector3::UnitX);
	bool FullDraw =  (fabs(ZEVector3::DotProduct(AxisUp, CameraDirection)) > 0.98);

	ZEVector3 LineStart, LineEnd;
	for (ZESize X = 0; X < 64; X++)
	{
		ZEMatrix4x4::Transform(LineStart, Transformation, ZEVector3(0.0f, AxisLenght * sinf(X * HAngle), AxisLenght * cosf(X * HAngle)));
		ZEMatrix4x4::Transform(LineEnd, Transformation, ZEVector3(0.0f, AxisLenght * sinf((X + 1) * HAngle), AxisLenght * cosf((X + 1) * HAngle)));
	
		bool Backface = (!FullDraw && 
						ZEVector3::DotProduct((LineStart - Position).Normalize(), CameraDirection) < -0.01f &&
						ZEVector3::DotProduct((LineEnd - Position).Normalize(), CameraDirection) < -0.01f);
		
		if (HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_XZ_AXIS)
		{
			ZECanvasVertex* Verts = GizmoLines.Vertices.MassAdd(2);
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

			ZECanvasVertex* Verts = GizmoLines.Vertices.MassAdd(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;
			Verts[0].Color = Verts[1].Color = ZEVector4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	// Y Axis
	ZEQuaternion::VectorProduct(AxisUp, Rotation, ZEVector3::UnitY);
	FullDraw =  (fabs(ZEVector3::DotProduct(AxisUp, CameraDirection)) > 0.98);

	for (ZESize X = 0; X < 64; X++)
	{
		ZEMatrix4x4::Transform(LineStart, Transformation, ZEVector3(AxisLenght * sinf(X * HAngle), 0.0f, AxisLenght * cosf(X * HAngle)));
		ZEMatrix4x4::Transform(LineEnd, Transformation, ZEVector3(AxisLenght * sinf((X + 1) * HAngle), 0.0f, AxisLenght * cosf((X + 1) * HAngle)));

		bool Backface = (!FullDraw && 
						ZEVector3::DotProduct((LineStart - Position).Normalize(), CameraDirection) < -0.01f &&
						ZEVector3::DotProduct((LineEnd - Position).Normalize(), CameraDirection) < -0.01f);
		
		if (HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_YZ_AXIS)
		{
			ZECanvasVertex* Verts = GizmoLines.Vertices.MassAdd(2);
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

			ZECanvasVertex* Verts = GizmoLines.Vertices.MassAdd(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;
			Verts[0].Color = Verts[1].Color = ZEVector4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}


	// Z Axis
	ZEQuaternion::VectorProduct(AxisUp, Rotation, ZEVector3::UnitZ);
	FullDraw =  (fabs(ZEVector3::DotProduct(AxisUp, CameraDirection)) > 0.98);

	for (ZESize X = 0; X < 64; X++)
	{
		ZEMatrix4x4::Transform(LineStart, Transformation, ZEVector3(AxisLenght * sinf(X * HAngle), AxisLenght * cosf(X * HAngle), 0.0f));
		ZEMatrix4x4::Transform(LineEnd, Transformation, ZEVector3(AxisLenght * sinf((X + 1) * HAngle), AxisLenght * cosf((X + 1) * HAngle), 0.0f));
		
		bool Backface = (!FullDraw && 
						ZEVector3::DotProduct((LineStart - Position).Normalize(), CameraDirection) < -0.01f &&
						ZEVector3::DotProduct((LineEnd - Position).Normalize(), CameraDirection) < -0.01f);
		
		if (HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_XZ_AXIS || HoveredAxis == ZE_GA_YZ_AXIS)
		{
			ZECanvasVertex* Verts = GizmoLines.Vertices.MassAdd(2);
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

			ZECanvasVertex* Verts = GizmoLines.Vertices.MassAdd(2);
			Verts[0].Position = LineStart;
			Verts[1].Position = LineEnd;
			Verts[0].Color = Verts[1].Color = ZEVector4(0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	DirtyGizmoFlag = false;
}

void ZEGizmo::UpdateScaleGizmo()
{
	ZEVector3 CameraPosition = zeScene->GetActiveCamera()->GetWorldPosition();
	ZEQuaternion CameraRotation = zeScene->GetActiveCamera()->GetWorldRotation();
	
	static ZEVector3 OldCameraPosition = ZEVector3::Zero;
	static ZEQuaternion OldCameraRotation = ZEQuaternion::Identity;

	if (!DirtyGizmoFlag && CameraPosition == OldCameraPosition && CameraRotation == OldCameraRotation)
		return;

	float AxisLenght = this->AxisLenght * ZEVector3::Distance(CameraPosition, GetPosition()) * zeScene->GetActiveCamera()->GetProjectionTransform().M11;
	float AxisLenght_2 = AxisLenght * 0.5f;
	float AxisLenght_10 = AxisLenght * 0.1f;
	float AxisLenght_5 = AxisLenght * 0.2f;
	float AxisLenght_4 = AxisLenght * 0.25f;

	GizmoLines.Clean();
	GizmoTriangles.Clean();

	// X-Axis
	ZEVector4 XAxisColor = (HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_XZ_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(AxisLenght, 0.0f, 0.0f));

	GizmoTriangles.SetColor(XAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3(0.0f, 0.0f, 1.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(AxisLenght - AxisLenght_5, 0.0f, 0.0f));
	GizmoTriangles.AddPyramid(AxisLenght_10, AxisLenght_5, AxisLenght_10);

	// Y-Axis
	ZEVector4 YAxisColor = (HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_YZ_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, AxisLenght, 0.0f));

	GizmoTriangles.SetColor(YAxisColor);
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, AxisLenght - AxisLenght_5, 0.0f));
	GizmoTriangles.SetRotation(ZEQuaternion::Identity);
	GizmoTriangles.AddPyramid(AxisLenght_10, AxisLenght_5, AxisLenght_10);

	// Z-Axis
	ZEVector4 ZAxisColor = (HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_XZ_AXIS || HoveredAxis == ZE_GA_YZ_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f ,0.0f), ZEVector3(0.0f, 0.0f, AxisLenght));

	GizmoTriangles.SetColor(ZAxisColor);
	GizmoTriangles.SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
	GizmoTriangles.SetTranslation(ZEVector3(0.0f, 0.0f, AxisLenght - AxisLenght_5));
	GizmoTriangles.AddPyramid(AxisLenght_10, AxisLenght_5, AxisLenght_10);


	GizmoLines.PushTransformation();

	GizmoLines.SetRotation(CameraRotation * GetRotation().Conjugate());

	// X Letter
	GizmoLines.SetColor(XAxisColor);
	GizmoLines.SetTranslation(ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, -0.5f, 0.0f), ZEVector3(-0.25f, 0.5f, 0.0f));

	// Y Letter
	GizmoLines.SetColor(YAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, -0.5f, 0.0f));

	// Z Letter
	GizmoLines.SetColor(ZAxisColor);
	GizmoLines.SetTranslation(ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(-0.25f, -0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, -0.5f, 0.0f));

	GizmoLines.PopTransformation();

	// XY-Axis
	GizmoLines.SetColor(HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght_4, AxisLenght_4, 0.0f));
	GizmoLines.AddLine(ZEVector3(AxisLenght * 0.7f, 0.0f, 0.0f), ZEVector3(AxisLenght * 0.35f, AxisLenght * 0.35f, 0.0f));

	GizmoLines.SetColor(HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_XY_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(AxisLenght_4, AxisLenght_4, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLenght * 0.7f, 0.0f), ZEVector3(AxisLenght * 0.35f, AxisLenght * 0.35f, 0.0f));
	
	if (HoveredAxis == ZE_GA_XY_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght * 0.7f, 0.0f, 0.0f), 
			ZEVector3(0.0f, AxisLenght * 0.7f, 0.0f), ZEVector3(0.0f, AxisLenght_2, 0.0f));
	}


	// XZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZE_GA_X_AXIS || HoveredAxis == ZE_GA_XZ_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght_4, 0.0f, AxisLenght_4));
	GizmoLines.AddLine(ZEVector3(AxisLenght * 0.7f, 0.0f, 0.0f), ZEVector3(AxisLenght * 0.35f, 0.0f, AxisLenght * 0.35f));

	GizmoLines.SetColor(HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_XZ_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLenght_2), ZEVector3(AxisLenght_4, 0.0f, AxisLenght_4));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLenght * 0.7f), ZEVector3(AxisLenght * 0.35f, 0.0f, AxisLenght * 0.35f));

	if (HoveredAxis == ZE_GA_XZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(AxisLenght * 0.7f, 0.0f, 0.0f), 
			ZEVector3(0.0f, 0.0f, AxisLenght * 0.7f), ZEVector3(0.0f, 0.0f, AxisLenght_2));
	}			
	

	// YZ-Axis
	GizmoLines.SetColor(HoveredAxis == ZE_GA_Y_AXIS || HoveredAxis == ZE_GA_YZ_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(0.0f, AxisLenght_4, AxisLenght_4));
	GizmoLines.AddLine(ZEVector3(0.0f, AxisLenght * 0.7f, 0.0f), ZEVector3(0.0f, AxisLenght * 0.35f, AxisLenght * 0.35f));

	GizmoLines.SetColor(HoveredAxis == ZE_GA_Z_AXIS || HoveredAxis == ZE_GA_YZ_AXIS || HoveredAxis == ZE_GA_XYZ_AXIS ? ZEVector4(1.0f, 1.0f, 0.0f, 1.0f) : ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLenght_2), ZEVector3(0.0f, AxisLenght_4, AxisLenght_4));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, AxisLenght * 0.7f), ZEVector3(0.0f, AxisLenght * 0.35f, AxisLenght * 0.35f));

	if (HoveredAxis == ZE_GA_YZ_AXIS)
	{
		GizmoTriangles.SetColor(ZEVector4(1.0f, 1.0f, 0.0f, 1.0f));
		GizmoTriangles.SetTranslation(ZEVector3::Zero);
		GizmoTriangles.SetRotation(ZEQuaternion::Identity);
		GizmoTriangles.AddQuad(ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(0.0f, AxisLenght * 0.7f, 0.0f), 
			ZEVector3(0.0f, 0.0f, AxisLenght * 0.7f), ZEVector3(0.0f, 0.0f, AxisLenght_2));
	}

	if (HoveredAxis == ZE_GA_XYZ_AXIS)
	{
		GizmoTriangles.SetTransfomation(ZEMatrix4x4::Identity);
		GizmoTriangles.AddTriangle(ZEVector3::Zero, ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(0.0f, AxisLenght_2, 0.0f));
		GizmoTriangles.AddTriangle(ZEVector3::Zero, ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(0.0f, 0.0f, AxisLenght_2));
		GizmoTriangles.AddTriangle(ZEVector3::Zero, ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(0.0f, 0.0f, AxisLenght_2));
		GizmoTriangles.AddTriangle(ZEVector3(AxisLenght_2, 0.0f, 0.0f), ZEVector3(0.0f, AxisLenght_2, 0.0f), ZEVector3(0.0f, 0.0f, AxisLenght_2));
	}

	DirtyGizmoFlag = false;
}

void ZEGizmo::UpdateHelperGizmo()
{

	float AxisLenght = this->AxisLenght * ZEVector3::Distance(zeScene->GetActiveCamera()->GetWorldPosition(), GetPosition()) * zeScene->GetActiveCamera()->GetProjectionTransform().M11;
	float AxisLenght_5 = AxisLenght * 0.2f;
	float AxisLenght_10 = AxisLenght * 0.1f;

	GizmoTriangles.Clean();
	GizmoLines.Clean();

	// Axis X
	GizmoLines.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(1.0f, 0.0f, 0.0f) * AxisLenght);

	// Axis Y
	GizmoLines.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 1.0f, 0.0f) * AxisLenght);

	// Small Axis Z
	GizmoLines.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.AddLine(ZEVector3::Zero, ZEVector3(0.0f, 0.0f, 1.0f) * AxisLenght);

	GizmoLines.SetRotation(zeScene->GetActiveCamera()->GetWorldRotation() * GetRotation().Conjugate());

	// X Letter
	GizmoLines.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	GizmoLines.SetTranslation(ZEVector3(1.0f, 0.0f, 0.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, -0.5f, 0.0f), ZEVector3(-0.25f, 0.5f, 0.0f));


	// Y Letter
	GizmoLines.SetColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
	GizmoLines.SetTranslation(ZEVector3(0.0f, 1.0f, 0.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(0.0f, 0.0f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.0f, 0.0f, 0.0f), ZEVector3(0.0f, -0.5f, 0.0f));

	// Z Letter
	GizmoLines.SetColor(ZEVector4(0.0f, 0.0f, 1.0f, 1.0f));
	GizmoLines.SetTranslation(ZEVector3(0.0f, 0.0f, 1.0f) * (AxisLenght + AxisLenght_5));
	GizmoLines.SetScale(ZEVector3(AxisLenght_10, AxisLenght_10, AxisLenght_10));
	GizmoLines.AddLine(ZEVector3(-0.25f, 0.5f, 0.0f), ZEVector3(0.25f, 0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(0.25f, 0.5f, 0.0f), ZEVector3(-0.25f, -0.5f, 0.0f));
	GizmoLines.AddLine(ZEVector3(-0.25f, -0.5f, 0.0f), ZEVector3(0.25f, -0.5f, 0.0f));
}

void ZEGizmo::UpdateGizmo()
{
	switch(Mode)
	{
		case ZE_GM_MOVE:
			UpdateMoveGizmo();
			break;

		case ZE_GM_ROTATE:
			UpdateRotateGizmo();
			break;

		case ZE_GM_SCALE:
			UpdateScaleGizmo();
			break;

		case ZE_GM_NONE:
		case ZE_GM_HELPER:
			UpdateHelperGizmo();

	}
}

ZEVector3 ZEGizmo::MoveProjection_(ZEGizmoAxis Axis, const ZERay& Ray)
{
	const ZEVector3& Position = GetPosition();

	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);

	switch(Axis)
	{
		case ZE_GA_X_AXIS:
		{
			ZELine AxisLine;
			AxisLine.p = Position;
			AxisLine.v = Right;

			float TRay, TLine;
			if (ZERay::MinimumDistance(Ray, AxisLine, TRay, TLine) < 10.0f)
				return AxisLine.GetPointOn(TLine);
			else
				return GetPosition();
		}

		case ZE_GA_Y_AXIS:
		{
			ZELine AxisLine;
			AxisLine.p = Position;
			AxisLine.v = Up;

			float TRay, TLine;
			if (ZERay::MinimumDistance(Ray, AxisLine, TRay, TLine) < 10.0f)
				return AxisLine.GetPointOn(TLine);
			else
				return GetPosition();
		}

		case ZE_GA_Z_AXIS:
		{
			ZELine AxisLine;
			AxisLine.p = Position;
			AxisLine.v = Front;

			float TRay, TLine;
			if (ZERay::MinimumDistance(Ray, AxisLine, TRay, TLine) < 10.0f)
				return AxisLine.GetPointOn(TLine);
			else
				return GetPosition();
		}

		case ZE_GA_XY_AXIS:
		{
			ZEPlane AxisPlane(Front, Position);
			float TRay;
			if (ZEPlane::IntersectionTest(AxisPlane, Ray, TRay))
				return Ray.GetPointOn(TRay);
			else
				return GetPosition();
		}

		case ZE_GA_XZ_AXIS:
		{
			ZEPlane AxisPlane(Up, Position);
			float TRay;
			if (ZEPlane::IntersectionTest(AxisPlane, Ray, TRay))
				return Ray.GetPointOn(TRay);
			else
				return GetPosition();
		}

		case ZE_GA_YZ_AXIS:
		{
			ZEPlane AxisPlane(Right, Position);
			float TRay;
			if (ZEPlane::IntersectionTest(AxisPlane, Ray, TRay))
				return Ray.GetPointOn(TRay);
			else
				return GetPosition();
		}

		default:
			return GetPosition();
	}
}

ZEQuaternion ZEGizmo::RotationProjection_(ZEGizmoAxis Axis, const ZERay& Ray)
{
	return ZEQuaternion::Identity;
}

ZEVector3 ZEGizmo::ScaleProjection_(ZEGizmoAxis Axis, const ZERay& Ray)
{
	return ZEVector3::One;
}

ZEGizmoAxis ZEGizmo::PickMoveAxis(const ZERay& Ray, float& TRay)
{
	float AxisLenght = this->AxisLenght * ZEVector3::Distance(zeScene->GetActiveCamera()->GetWorldPosition(), GetPosition()) * zeScene->GetActiveCamera()->GetProjectionTransform().M11;
	float AxisLenght_2 = AxisLenght * 0.5f;
	float AxisTreshold = AxisLenght * 0.1f;

	ZEGizmoAxis PickedAxis = ZE_GA_NONE;

	const ZEVector3& Position = GetPosition();

	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);
	
	float NewTRay, TLineSegment;

	// Check Planes
	ZEPlane AxisXY(Front, Position), AxisXZ(Up, Position), AxisYZ(Right, Position);
	if (ZEPlane::IntersectionTest(AxisXY, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			ZEVector3 TestPos (AxisXY.p, Ray.GetPointOn(NewTRay));
			float ProjU  = ZEVector3::DotProduct(TestPos, Right);
			float ProjV = ZEVector3::DotProduct(TestPos, Up);
			if (ProjU > 0.0f && ProjU < AxisLenght_2 && ProjV > 0.0f && ProjV < AxisLenght_2)
			{
				TRay = NewTRay;
				PickedAxis = ZE_GA_XY_AXIS;
			}
		}

	if (ZEPlane::IntersectionTest(AxisXZ, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			ZEVector3 TestPos (AxisXZ.p, Ray.GetPointOn(NewTRay));
			float ProjU  = ZEVector3::DotProduct(TestPos, Right);
			float ProjV = ZEVector3::DotProduct(TestPos, Front);
			if (ProjU > 0.0f && ProjU < AxisLenght_2 && ProjV > 0.0f && ProjV < AxisLenght_2)
			{
				TRay = NewTRay;
				PickedAxis = ZE_GA_XZ_AXIS;
			}
		}

	if (ZEPlane::IntersectionTest(AxisYZ, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			ZEVector3 TestPos (AxisYZ.p, Ray.GetPointOn(NewTRay));
			float ProjU  = ZEVector3::DotProduct(TestPos, Up);
			float ProjV = ZEVector3::DotProduct(TestPos, Front);
			if (ProjU > 0.0f && ProjU < AxisLenght_2 && ProjV > 0.0f && ProjV < AxisLenght_2)
			{
				TRay = NewTRay;
				PickedAxis = ZE_GA_YZ_AXIS;
			}
		}

	// Check Line Segments
	ZELineSegment XAxis(Position, Position + Right * AxisLenght);
	if (ZELineSegment::MinimumDistance(XAxis, Ray, TLineSegment, NewTRay) < AxisTreshold)
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			PickedAxis = ZE_GA_X_AXIS;
		}

	ZELineSegment YAxis(GetPosition(), Position + Up * AxisLenght);
	if (ZELineSegment::MinimumDistance(YAxis, Ray, TLineSegment, NewTRay) < AxisTreshold)
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			PickedAxis = ZE_GA_Y_AXIS;
		}

	ZELineSegment ZAxis(GetPosition(), Position + Front * AxisLenght);
	if (ZELineSegment::MinimumDistance(ZAxis, Ray, TLineSegment, NewTRay) < AxisTreshold)
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			PickedAxis = ZE_GA_Z_AXIS;
		}

	return PickedAxis;
}

ZEGizmoAxis ZEGizmo::PickRotateAxis(const ZERay& Ray, float& TRay)
{
	float AxisLenght = this->AxisLenght * ZEVector3::Distance(zeScene->GetActiveCamera()->GetWorldPosition(), GetPosition()) * zeScene->GetActiveCamera()->GetProjectionTransform().M11;
	float AxisTreshold = AxisLenght * 0.1f;

	ZEVector3 CameraPosition = zeScene->GetActiveCamera()->GetWorldPosition().Normalize();
	const ZEVector3& Position = GetPosition();

	ZEVector3 CameraDirection = zeScene->GetActiveCamera()->GetWorldRotation() * ZEVector3::UnitZ;

	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);


	float NewTRay;

	ZEGizmoAxis PickedAxis = ZE_GA_NONE;

	// X Axis
	ZEPlane AxisXPlane(Right, Position), AxisYPlane(Up, Position), AxisZPlane(Front, Position);

	if (ZEPlane::IntersectionTest(AxisXPlane, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			ZEVector3 IntersectionPoint = Ray.GetPointOn(NewTRay);

			float Distance = ZEVector3::Distance(IntersectionPoint, Position);

			if (fabs(Distance - AxisLenght) < AxisTreshold &&
				ZEVector3::DotProduct(IntersectionPoint.Normalize(), CameraDirection) < 0.0f)
			{
				TRay = NewTRay;
				PickedAxis = ZE_GA_X_AXIS;
			}
		}
		
	if (ZEPlane::IntersectionTest(AxisYPlane, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			ZEVector3 IntersectionPoint = Ray.GetPointOn(NewTRay);
			float Distance = ZEVector3::Distance(IntersectionPoint, Position);

			if (fabs(Distance - AxisLenght) < AxisTreshold &&
				ZEVector3::DotProduct(IntersectionPoint.Normalize(), CameraDirection) < 0.0f)
			{
				TRay = NewTRay;
				PickedAxis = ZE_GA_Y_AXIS;
			}
		}

	if (ZEPlane::IntersectionTest(AxisZPlane, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			ZEVector3 IntersectionPoint = Ray.GetPointOn(NewTRay);
			float Distance = ZEVector3::Distance(IntersectionPoint, Position);

			if (fabs(Distance - AxisLenght) < AxisTreshold &&
				ZEVector3::DotProduct(IntersectionPoint.Normalize(), CameraDirection) < 0.0f)
			{
				TRay = NewTRay;
				PickedAxis = ZE_GA_Z_AXIS;
			}
		}

	return PickedAxis;
}

ZEGizmoAxis ZEGizmo::PickScaleAxis(const ZERay& Ray, float& TRay)
{
	float AxisLenght = this->AxisLenght * ZEVector3::Distance(zeScene->GetActiveCamera()->GetWorldPosition(), GetPosition()) * zeScene->GetActiveCamera()->GetProjectionTransform().M11;
	float AxisLenght_2 = AxisLenght * 0.5f;
	float AxisTreshold = AxisLenght * 0.1f;

	ZEGizmoAxis PickedAxis = ZE_GA_NONE;

	const ZEVector3& Position = GetPosition();

	ZEVector3 Right, Up, Front;
	ZEQuaternion::ConvertToLookAndUp(Front, Up, GetRotation());
	ZEVector3::CrossProduct(Right, Up, Front);
	ZEVector3::Normalize(Right, Right);
	
	float NewTRay, TLineSegment;

	// Check Planes
	ZETriangle InnerAxisXY(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLenght_2, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, AxisLenght_2, 0.0f));
	ZETriangle OuterAxisXY(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLenght * 0.7f, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, AxisLenght * 0.7f, 0.0f));
	if (ZETriangle::IntersectionTest(OuterAxisXY, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			if (ZETriangle::IntersectionTest(InnerAxisXY, Ray, NewTRay))
				PickedAxis = ZE_GA_XYZ_AXIS;
			else
				PickedAxis = ZE_GA_XY_AXIS;
		}


	ZETriangle InnerAxisXZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLenght_2, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLenght_2));
	ZETriangle OuterAxisXZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(AxisLenght * 0.7f, 0.0f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLenght * 0.7f));
	if (ZETriangle::IntersectionTest(OuterAxisXZ, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			if (ZETriangle::IntersectionTest(InnerAxisXZ, Ray, NewTRay))
				PickedAxis = ZE_GA_XYZ_AXIS;
			else
				PickedAxis = ZE_GA_XZ_AXIS;
		}

	ZETriangle InnerAxisYZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(0.0f, AxisLenght_2, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLenght_2));
	ZETriangle OuterAxisYZ(GetWorldTransform() * ZEVector3::Zero, GetWorldTransform() * ZEVector3(0.0f, AxisLenght * 0.7f, 0.0f), GetWorldTransform() * ZEVector3(0.0f, 0.0f, AxisLenght * 0.7f));
	if (ZETriangle::IntersectionTest(OuterAxisYZ, Ray, NewTRay))
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			if (ZETriangle::IntersectionTest(InnerAxisYZ, Ray, NewTRay))
				PickedAxis = ZE_GA_XYZ_AXIS;
			else
				PickedAxis = ZE_GA_YZ_AXIS;
		}

	// Check Line Segments
	ZELineSegment XAxis(Position, Position + Right * AxisLenght);
	if (ZELineSegment::MinimumDistance(XAxis, Ray, TLineSegment, NewTRay) < AxisTreshold)
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			PickedAxis = ZE_GA_X_AXIS;
		}

	ZELineSegment YAxis(GetPosition(), Position + Up * AxisLenght);
	if (ZELineSegment::MinimumDistance(YAxis, Ray, TLineSegment, NewTRay) < AxisTreshold)
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			PickedAxis = ZE_GA_Y_AXIS;
		}

	ZELineSegment ZAxis(GetPosition(), Position + Front * AxisLenght);
	if (ZELineSegment::MinimumDistance(ZAxis, Ray, TLineSegment, NewTRay) < AxisTreshold)
		if (NewTRay < TRay)
		{
			TRay = NewTRay;
			PickedAxis = ZE_GA_Z_AXIS;
		}

	return PickedAxis;
}

ZEUInt32 ZEGizmo::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEGizmo::SetPosition(const ZEVector3& NewPosition)
{
	OldPosition = GetPosition();

	if (Mode == ZE_GM_ROTATE)
		DirtyGizmoFlag = true;

	ZEEntity::SetPosition(NewPosition);
}

void ZEGizmo::SetRotation(const ZEQuaternion& NewRotation)
{
	if (Mode == ZE_GM_ROTATE)
		DirtyGizmoFlag = true;

	ZEEntity::SetRotation(NewRotation);
}

void ZEGizmo::SetMode(ZEGizmoMode Mode)
{
	if (this->Mode == Mode)
		return;

	this->Mode = Mode;
	DirtyGizmoFlag = true;
}

ZEGizmoMode ZEGizmo::GetMode()
{
	return Mode;
}

void ZEGizmo::SetHoveredAxis(ZEGizmoAxis GizmoAxis)
{
	if (GizmoAxis == HoveredAxis)
		return;

	HoveredAxis = GizmoAxis;

	DirtyGizmoFlag = true;
}

ZEGizmoAxis ZEGizmo::GetHoveredAxis()
{
	return HoveredAxis;
}

void ZEGizmo::SetAxisLenght(float Size)
{
	AxisLenght = Size;
	DirtyGizmoFlag = true;
}

float ZEGizmo::GetAxisLenght()
{
	return AxisLenght;
}

ZEVector3 ZEGizmo::GetPositionChange()
{
	ZEVector3 Result;
	ZEVector3::Sub(Result, GetPosition(), OldPosition);
	return Result;
}

bool ZEGizmo::Initialize()
{
	if (GetInitialized())
		return false;

	GizmoMaterial = ZESimpleMaterial::CreateInstance();

	RenderCommand.SetZero();
	RenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderCommand.Material = GizmoMaterial;
	
	return ZEEntity::Initialize();
}

void ZEGizmo::Deinitialize()
{	
	if (GizmoMaterial != NULL)
	{
		GizmoMaterial->Release();
		GizmoMaterial = NULL;
	}

	ZEEntity::Deinitialize();
}

void ZEGizmo::Draw(ZEDrawParameters* DrawParameters)
{
	UpdateGizmo();

	RenderCommand.WorldMatrix = GetWorldTransform();
	RenderCommand.Flags = (Mode == ZE_GM_ROTATE ? ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM : ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM);
	RenderCommand.PrimitiveType = ZE_ROPT_LINE;
	RenderCommand.PrimitiveCount = GizmoLines.Vertices.GetCount() / 2;
	RenderCommand.VertexBuffer = &GizmoLines;
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);

	RenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderCommand.PrimitiveCount = GizmoTriangles.Vertices.GetCount() / 3;
	RenderCommand.VertexBuffer = &GizmoTriangles;
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
}

ZEGizmoAxis ZEGizmo::PickAxis(const ZERay& Ray, float& TRay)
{
	switch(Mode)
	{
		case ZE_GM_MOVE:
			return PickMoveAxis(Ray, TRay);

		case ZE_GM_ROTATE:
			return PickRotateAxis(Ray, TRay);

		case ZE_GM_SCALE:
			return PickScaleAxis(Ray, TRay);

		default:
			return ZE_GA_NONE;
	}
}

void ZEGizmo::StartMoveProjection(ZEGizmoAxis Axis, const ZERay& InitialRay)
{
	PickPosition = MoveProjection_(Axis, InitialRay);
	MoveDifference = PickPosition - GetPosition();
	ProjectionAxis = Axis;
}

ZEVector3 ZEGizmo::MoveProjection(const ZERay& Ray)
{
	return MoveProjection_(ProjectionAxis, Ray) - MoveDifference;
}

void ZEGizmo::StartRotationProjection(ZEGizmoAxis& Axis, const ZERay& InitialRay)
{

}

ZEQuaternion ZEGizmo::RotationProjection(const ZERay& Ray)
{
	return ZEQuaternion::Identity;
}

void ZEGizmo::StartScaleProjection(ZEGizmoAxis Axis, const ZERay& InitialRay)
{
	PickPosition = MoveProjection_(Axis, InitialRay);
	MoveDifference = PickPosition - GetPosition();
	ProjectionAxis = Axis;
}

ZEVector3 ZEGizmo::ScaleProjection(const ZERay& Ray)
{
	return (MoveProjection_(ProjectionAxis, Ray) - MoveDifference);
}

ZEGizmo::ZEGizmo()
{
	DirtyGizmoFlag = true;
	HoveredAxis = ZE_GA_NONE;
	Mode = ZE_GM_NONE;
	AxisLenght = 1.0f;
}

ZEGizmo* ZEGizmo::CreateInstance()
{
	return new ZEGizmo();
}
