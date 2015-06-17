//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicsPicker.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 YiÄŸit OrÃ§un GÃ–KBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "YiÄŸit  OrÃ§un  GÃ–KBULUT") and
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
  Name: YiÄŸit OrÃ§un GÃ–KBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZEPhysicsPicker.h"
#include "ZEGame\ZEScene.h"
#include "ZERenderer\ZECamera.h"
#include "ZECore\ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZEPhysicalWorld.h"
#include "ZEPhysicalRigidBody.h"
#include "ZEPhysicalJoint.h"
#include "ZEPhysicalShapes.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"

void ZEPhysicsPicker::ForwardViewProjection(ZEInt& CursorXOutput, ZEInt& CursorYOutput, float& Depth, const ZEVector3& Input)
{
	ZEVector4 WorldSpacePosition;
	WorldSpacePosition.x = Input.x;
	WorldSpacePosition.y = Input.y;
	WorldSpacePosition.z = Input.z;
	WorldSpacePosition.w = 1.0f;

	ZEInt WindowLeftPosition;
	ZEInt WindowTopPosition;
	ZEInt WindowWidth;
	ZEInt WindowHeight;

	zeCore->GetWindow()->GetWindowPosition(WindowLeftPosition, WindowTopPosition);
	zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);

	ZEMatrix4x4 ViewTransform = zeScene->GetActiveCamera()->GetViewTransform();

	ZEVector4 ViewPosition = ViewTransform * WorldSpacePosition;

	ZEMatrix4x4 ProjectionTransform = zeScene->GetActiveCamera()->GetProjectionTransform();

	ZEVector4 ProjectionPosition = ProjectionTransform * ViewPosition;

	ZEMatrix4x4 ViewPortTransform;
	ZEMatrix4x4::CreateViewPortTransform(ViewPortTransform, WindowLeftPosition, WindowLeftPosition + WindowWidth, WindowTopPosition + WindowHeight, WindowTopPosition, zeGraphics->GetNearZ(), zeGraphics->GetFarZ());

	ZEVector4 ScreenPosition = ViewPortTransform * ProjectionPosition;

	CursorXOutput = ScreenPosition.x;
	CursorYOutput = ScreenPosition.y;
	Depth = ScreenPosition.z;
}

void ZEPhysicsPicker::ReverseViewProjection(ZEVector3& Output, const ZEInt& CursorXInput, const ZEInt& CursorYInput, const float& Depth)
{
	ZEVector3 TempVector;
	const ZEMatrix4x4& ProjMatrix = zeScene->GetActiveCamera()->GetProjectionTransform();
	TempVector.x =  (((2.0f * CursorXInput ) / zeGraphics->GetScreenWidth()) - 1) / ProjMatrix.M11;
	TempVector.y = -(((2.0f * CursorYInput ) / zeGraphics->GetScreenHeight()) - 1) / ProjMatrix.M22;
	TempVector.z =  1.0f;

	ZEMatrix4x4 InvViewMatrix;
	ZEMatrix4x4::Inverse(InvViewMatrix, zeScene->GetActiveCamera()->GetViewTransform());

	ZEMatrix4x4::Transform3x3(Output, InvViewMatrix, TempVector);

// 	ZEInt WindowWidth;
// 	ZEInt WindowHeight;
// 
// 	ZEInt LeftPosition;
// 	ZEInt TopPosition;
// 
// 	zeCore->GetWindow()->GetWindowPosition(LeftPosition, TopPosition);
// 	zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);
// 
// 	//ZEMatrix4x4::CreateViewPortTransform(ViewPort, LeftPosition, LeftPosition + WindowWidth, TopPosition + WindowHeight, TopPosition, zeGraphics->GetNearZ(), zeGraphics->GetFarZ());
// 	//ZEMatrix4x4::CreateViewPortTransform(ViewPort, 0.0f, WindowWidth, WindowHeight, 0.0f, zeGraphics->GetNearZ(), zeGraphics->GetFarZ());
// 	
// 	ZEVector4 ViewPortVector;
// 	ViewPortVector.x = (2.0f * ((float)CursorXInput) / ((float)WindowWidth)) - 1.0f;
// 	ViewPortVector.y = -((2.0f * ((float)CursorYInput) / ((float)WindowHeight)) - 1.0f);
// 	ViewPortVector.z = Depth;
// 	ViewPortVector.w = 1.0f;
// 
// 	ZEVector4 ProjectionVector;
// 	ZEVector3 ViewVector;
// 
// 	ZEMatrix4x4 InvProjection = zeScene->GetActiveCamera()->GetProjectionTransform().Inverse();
// 
// 	ZEMatrix4x4::Transform(ProjectionVector, InvProjection, ViewPortVector);
// 
// 	ViewVector.x = ProjectionVector.x;
// 	ViewVector.y = ProjectionVector.y;
// 	ViewVector.z = ProjectionVector.z;
// 
// 	ViewVector /= ProjectionVector.w;
// 
// 	ZEMatrix4x4 InvView = zeScene->GetActiveCamera()->GetViewTransform().Inverse();
// 
// 	ZEMatrix4x4::Transform3x3(Output, InvView, ViewVector);
// 
// // 	ZEVector3 TempVector = InvView * (InvProjection * (InvViewPort * ViewPortVector));
// // 
// // 	Output.x = TempVector.x;
// // 	Output.y = TempVector.y;
// // 	Output.z = TempVector.z;
}

void ZEPhysicsPicker::Initialize()
{

}

void ZEPhysicsPicker::Deinitialize()
{
	ReleaseObject();
}

ZEPhysicsPicker::ZEPhysicsPicker()
{
	MouseJoint = NULL;
	MouseSphere = NULL;
	MouseDepth = 0.0f;
}

ZEPhysicsPicker::~ZEPhysicsPicker()
{
	Deinitialize();
	delete this;
}

bool ZEPhysicsPicker::PickObject(ZEInt X, ZEInt Y)
{
	ReleaseObject();

	MouseSphere = ZEPhysicalRigidBody::CreateInstance();
	ZEPhysicalSphereShape* Sphere = new ZEPhysicalSphereShape();
	Sphere->SetRadius(0.01f);

	MouseSphere->AddPhysicalShape(Sphere);
	MouseSphere->SetPhysicalWorld(zeScene->GetPhysicalWorld());
	MouseSphere->SetMass(0.005f);

	MouseJoint = ZEPhysicalJoint::CreateInstance();
	MouseJoint->SetSwing1Motion(ZE_PJMOTION_FREE);
	MouseJoint->SetTwistMotion(ZE_PJMOTION_FREE);
	MouseJoint->SetSwing2Motion(ZE_PJMOTION_FREE);
	MouseJoint->SetXMotion(ZE_PJMOTION_LOCKED);
	MouseJoint->SetYMotion(ZE_PJMOTION_LOCKED);
	MouseJoint->SetLinearLimitDamping(1.0f);
	MouseJoint->SetLinearLimitSpring(50.f);
	MouseJoint->SetZMotion(ZE_PJMOTION_LOCKED);

	ZERay Ray;

	zeScene->GetActiveCamera()->GetScreenRay(Ray, X, Y);

	RayCast = Ray;

	ZERayCastResultDetails ResultDetails;

	ZEPhysicalShape* ClosestShape = zeScene->GetPhysicalWorld()->RayCastToClosestShape(Ray, ZE_PRCFST_ALL_SHAPES, ResultDetails);

	if(ClosestShape == NULL)
		return false;

	ZEPhysicalObjectType ClosestObjectType = ClosestShape->GetOwner()->GetPhysicalObjectType();

	if (ClosestObjectType == ZE_POT_RIGID_BODY)
	{
		ZEPhysicalRigidBody* ClosestObject = (ZEPhysicalRigidBody*)ClosestShape->GetOwner();

		if (ClosestObject->GetPhysicalBodyType() != ZE_PBT_DYNAMIC)
			return false;

		//forwardproju rayin eye a distanceý ný yeni vector le çarp worldu çýksýn.
// 
// 		ZEInt RayCastResultX;
// 		ZEInt RayCastResultY;
// 		ForwardViewProjection(RayCastResultX, RayCastResultY, MouseDepth, ResultDetails.ImpactWorldPosition);

		MouseDepth = ResultDetails.ImpactDistance;

		MouseSphere->SetPosition(ResultDetails.ImpactWorldPosition);
		MouseSphere->SetPhysicalBodyType(ZE_PBT_KINEMATIC);

		//Sphere için collision u kapat.

		MouseSphere->Initialize();

		ZEMatrix4x4 InvRotationMatrix;

		MouseJoint->SetBodyA(MouseSphere);
				
// 		ZEMatrix4x4::CreateRotation(InvRotationMatrix, MouseSphere->GetRotation());
// 		InvRotationMatrix.InverseSelf();
// 		ZEMatrix4x4 Orientation;
// 		ZEMatrix4x4::CreateOrientation(Orientation, MouseSphere->GetPosition(), MouseSphere->GetRotation(), MouseSphere->GetScale());
// 		ZEMatrix4x4 ResultMatrix = Orientation * InvRotationMatrix;
// 		MouseJoint->SetBodyAConnectionPosition(ResultMatrix * ResultDetails.ImpactWorldPosition);
		//Orientation ayarla

		//Closest objecti uyandýr (sleep implementasyonu)

		MouseJoint->SetBodyB(ClosestObject);
// 		ZEMatrix4x4::CreateRotation(InvRotationMatrix, ClosestObject->GetRotation());
// 		InvRotationMatrix.InverseSelf();
// 		ZEMatrix4x4::CreateOrientation(Orientation, MouseJoint->GetPosition(), MouseJoint->GetRotation(), MouseJoint->GetScale());
// 		ResultMatrix = Orientation * InvRotationMatrix;
// 		MouseJoint->SetBodyBConnectionPosition(ResultMatrix * ResultDetails.ImpactWorldPosition);
		//Orientation ayarla

		MouseJoint->SetPhysicalWorld(zeScene->GetPhysicalWorld());
		MouseJoint->Initialize();

		delete Sphere;

		return true;
	}

	return false;
}

void ZEPhysicsPicker::MoveObject(ZEInt X, ZEInt Y)
{
	if (MouseSphere == NULL) 
		return;
	ZERay TargetPosition;
// 	ReverseViewProjection(TargetPosition, X, Y, MouseDepth);
	zeScene->GetActiveCamera()->GetScreenRay(TargetPosition, X, Y);

	TargetPosition.v *= MouseDepth;
	TargetPosition.v += TargetPosition.p;
	
// 
// 	TargetPosition *= MouseDepth;

	MouseSphere->SetPosition(TargetPosition.v);

	// wake selected actor up;
}

void ZEPhysicsPicker::ReleaseObject()
{
	if (MouseJoint != NULL)
	{
		MouseJoint->Destroy();
		MouseJoint = NULL;
	}

	if (MouseSphere != NULL)
	{
		MouseSphere->Destroy();
		MouseSphere = NULL;
	}
}

ZEPhysicsPicker* ZEPhysicsPicker::CreateInstance()
{
	return new ZEPhysicsPicker();
}
