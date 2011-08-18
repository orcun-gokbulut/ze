//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMaterialEditorViewPort.cpp
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

#include "ZEDMaterialEditorViewPort.h"
#include <ZECore/ZECore.h>
#include <ZECore/ZEWindow.h>
#include <ZEGame/ZEGame.h>
#include <ZEGame/ZEScene.h>
#include <QResizeEvent>

#include "ZEDMaterialEditor.h"

using namespace Qt;

ZEDMaterialEditorViewPort::ZEDMaterialEditorViewPort(ZEDMaterialEditor*	ParentEditor, QWidget* Parent, Qt::WindowFlags F)
{
	Yawn = Pitch = Roll = 0;
	PitchLight1 = 0;
	YawnLight1 = ZE_PI;
	PitchLight2 = -ZE_PI_2;
	YawnLight2 = 0;
	PitchLight3 = ZE_PI_2;
	YawnLight3 = 0;
	this->ParentEditor = ParentEditor;
}

void ZEDMaterialEditorViewPort::Initialize()
{
	Camera = ZEPlayer::CreateInstance();
	zeCore->GetGame()->GetScene()->AddEntity(Camera);
	Camera->SetFOV(ZE_PI_4);
	zeCore->GetGame()->GetScene()->SetActiveCamera(Camera->GetCamera());
	zeCore->GetGame()->GetScene()->SetActiveListener(Camera->GetListener());
	Camera->SetPosition(ZEVector3(0.0f, 0.0f, -5.0f));

	Grid = ZEGrid::CreateInstance();
	zeCore->GetGame()->GetScene()->AddEntity(Grid);

	Model = ZEModel::CreateInstance();
	Model->SetModelFile("Cube.ZEMODEL");
	zeCore->GetGame()->GetScene()->AddEntity(Model);

	DirectLight1 = ZEDirectionalLight::CreateInstance();
	zeCore->GetGame()->GetScene()->AddEntity(DirectLight1);
	DirectLight1->SetIntensity(1.0f);
	DirectLight1->SetColor(ZEVector3(1, 0.596078, 0.596078));
	ZEQuaternion Rotation;
	ZEQuaternion::CreateFromEuler(Rotation,0, ZE_PI, 0);
	DirectLight1->SetRotation(Rotation);

	DirectLight2 = ZEDirectionalLight::CreateInstance();
	zeCore->GetGame()->GetScene()->AddEntity(DirectLight2);
	DirectLight2->SetIntensity(1.0f);
	DirectLight2->SetColor(ZEVector3(0.823529, 1.0, 0.988235));
	ZEQuaternion::CreateFromEuler(Rotation, -ZE_PI_2, 0, 0);
	DirectLight2->SetRotation(Rotation);

	DirectLight3 = ZEDirectionalLight::CreateInstance();
	zeCore->GetGame()->GetScene()->AddEntity(DirectLight3);
	DirectLight3->SetIntensity(1.0f);
	//DirectLight3->SetColor(ZEVector3(0.988235f; 1.0f; 0.827451f));
	DirectLight3->SetColor(ZEVector3(0.666667,  0.333333,  0));
	ZEQuaternion::CreateFromEuler(Rotation, ZE_PI_2, 0, 0);
	DirectLight3->SetRotation(Rotation);

	ZERunTimeProperty	CProperty1;
	CProperty1.Name = "CanRocks";
	CProperty1.Semantic = ZE_PS_COLOR;
	CProperty1.Type = ZE_VRT_VECTOR3;
	CProperty1.Value = ZEVariant(ZEVector3::One);
	DirectLight3->AddCustomProperty(CProperty1);

	CProperty1.Name = "OrcunRocks";
	CProperty1.Semantic = ZE_PS_FILENAME;
	CProperty1.Type = ZE_VRT_STRING;
	CProperty1.Value = ZEVariant("We Ride!!!");
	DirectLight3->AddCustomProperty(CProperty1);

	//DirectLight3->SetProperty("CanRocks", ZEVariant(ZEVector3::Zero));
}

void ZEDMaterialEditorViewPort::resizeEvent(QResizeEvent* ResizeEvent)
{
	QFrame::resizeEvent(ResizeEvent);
	zeCore->GetWindow()->SetWindowSize(ResizeEvent->size().width(), ResizeEvent->size().height());
}

void ZEDMaterialEditorViewPort::mouseMoveEvent(QMouseEvent * Event)
{
	QFrame::mouseMoveEvent(Event);

	if(Event->buttons() & LeftButton)
	{
		RotateModel(OldMousePosition - Event->pos());
	}

	else if(Event->buttons() & RightButton)
	{
		RotateLights(OldMousePosition - Event->pos());
	}

	else if(Event->buttons() & MidButton)
	{
		MoveLeftRight(OldMousePosition - Event->pos());
	}
	
	OldMousePosition = Event->pos();
}

void ZEDMaterialEditorViewPort::wheelEvent(QWheelEvent * Event)
{
	QFrame::wheelEvent(Event);

	if (Event->delta() < 0)
		Camera->SetPosition(Camera->GetPosition() - ZEVector3(0.0f, 0.0f, 0.1f));
	else
		Camera->SetPosition(Camera->GetPosition() - ZEVector3(0.0f, 0.0f, -0.1f));
}

void ZEDMaterialEditorViewPort::mousePressEvent(QMouseEvent * Event)
{
	QFrame::mousePressEvent(Event);

	OldMousePosition = Event->pos();
}

ZEFixedMaterial* ZEDMaterialEditorViewPort::GetModelMaterial()
{
	if (Model->GetModelResource() == NULL)
		return NULL;

	return (ZEFixedMaterial*)(Model->GetModelResource()->Materials[0]);
}

void ZEDMaterialEditorViewPort::SetModelFile(const char* FileName)
{
	ZEModelResource* TempResource = ZEModelResource::LoadSharedResource(FileName);

	for (int I = 0; I < Model->GetModelResource()->Materials[0]->GetClassDescription()->GetPropertyCount(); I++)
	{
		ZEVariant TempVariant;
		Model->GetModelResource()->Materials[0]->GetProperty(I, TempVariant);

		if (TempVariant.GetType() == ZE_VRT_STRING && strcmp(TempVariant.GetString(), "") == 0)
			continue;

		TempResource->Materials[0]->SetProperty(I, TempVariant);
	}

	Model->SetModelResource(TempResource);
}

void ZEDMaterialEditorViewPort::RotateModel(QPoint PositionDifference)
{
	Yawn = Yawn + 0.005f * PositionDifference.x();
	Pitch = Pitch + 0.005f * PositionDifference.y();

	ZEQuaternion Temp, Rotation;
	ZEQuaternion::CreateFromEuler(Temp, Pitch, Yawn, Roll);
	ZEQuaternion::Normalize(Rotation, Temp);
	Model->SetRotation(Rotation);
}

void ZEDMaterialEditorViewPort::RotateLights(QPoint PositionDifference)
{
	YawnLight1 = YawnLight1 + 0.005f * PositionDifference.x();
	PitchLight1 = PitchLight1 + 0.005f * PositionDifference.y();
	ZEQuaternion Temp1, Rotation1;
	ZEQuaternion::CreateFromEuler(Temp1, PitchLight1, YawnLight1, Roll);
	ZEQuaternion::Normalize(Rotation1, Temp1);
	DirectLight1->SetRotation(Rotation1);

	YawnLight2 = YawnLight2 + 0.005f * PositionDifference.x();
	PitchLight2 = PitchLight2 + 0.005f * PositionDifference.y();
	ZEQuaternion Temp2, Rotation2;
	ZEQuaternion::CreateFromEuler(Temp2, PitchLight2, YawnLight2, Roll);
	ZEQuaternion::Normalize(Rotation2, Temp2);
	DirectLight2->SetRotation(Rotation2);

	YawnLight3 = YawnLight3 + 0.005f * PositionDifference.x();
	PitchLight3 = PitchLight3 + 0.005f * PositionDifference.y();
	ZEQuaternion Temp3, Rotation3;
	ZEQuaternion::CreateFromEuler(Temp3, PitchLight3, YawnLight3, Roll);
	ZEQuaternion::Normalize(Rotation3, Temp3);
	DirectLight3->SetRotation(Rotation3);

	ParentEditor->UpdateLightPropertyWidgets();
}

void ZEDMaterialEditorViewPort::MoveLeftRight(QPoint PositionDifference)
{
	ZEVector3 CameraPos = Camera->GetPosition();
	CameraPos.x = CameraPos.x + PositionDifference.x() * 0.001f;
	CameraPos.y = CameraPos.y + PositionDifference.y() * -0.001f;
	Camera->SetPosition(CameraPos);
}

ZEDirectionalLight*	ZEDMaterialEditorViewPort::GetDirectLight1()
{
	return DirectLight1;
}

ZEDirectionalLight*	ZEDMaterialEditorViewPort::GetDirectLight2()
{
	return DirectLight2;
}

ZEDirectionalLight*	ZEDMaterialEditorViewPort::GetDirectLight3()
{
	return DirectLight3;
}
