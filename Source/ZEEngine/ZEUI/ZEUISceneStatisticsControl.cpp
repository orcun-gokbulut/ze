//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUISceneStatisticsControl.cpp
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

#include "ZEUISceneStatisticsControl.h"
#include "ZEDS/ZEString.h"
#include "ZEUI/ZEFontResource.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZECamera.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"

#define TotalEntityText	   ZEString("Total Entity Count    : ")
#define DrawableEntityText ZEString("Drawable Entity Count : ")
#define VisibleEntityText  ZEString("Visible Entity Count  : ")
#define CulledEntityText   ZEString("Culled Entity Count   : ")
#define DrawedEntityText   ZEString("Drawn Entity Count    : ")
#define TotalLightText     ZEString("Total Light Count     : ")
#define VisibleLightText   ZEString("Visible Light Count   : ")
#define CulledLightText	   ZEString("Culled Light Count    : ")
#define DrawedLightText    ZEString("Drawn Light Count     : ")
#define FPSText				ZEString("Frame Per Second      : ")
#define CamPosText			ZEString("Camera Position       : ")

void ZEUISceneStatisticsControl::SetMaterial(ZEMaterial* Material)
{

}

ZEMaterial* ZEUISceneStatisticsControl::GetMaterial() const
{
	return NULL;
}

void ZEUISceneStatisticsControl::Draw(ZEUIRenderer* Renderer)
{
	ZEUIControl::Draw(Renderer);
}

void ZEUISceneStatisticsControl::Tick(float ElapsedTime)
{
	//FPS Calculation

	for(ZESize I = 0; I < GetChildControls().GetCount(); I++)
		GetChildControls()[I]->SetZOrder(GetZOrder());

	static float TotalElapsedTime = 0;
	static unsigned int AverageFPS = 0;
	static unsigned int FrameCount = 0;

	FrameCount++;
	TotalElapsedTime += ElapsedTime;
	if (TotalElapsedTime > 0.5f)
	{
		TotalElapsedTime = 0.0f;
		AverageFPS = FrameCount * 2;
		FrameCount = 0;
	}

	ZEScene* TempScene = zeScene;
	ZECullStatistics Stats = TempScene->GetCullerStatistics();
	ZEVector3 CamPos = TempScene->GetActiveCamera()->GetWorldPosition();
	ZEString Value;
	
	Value.SetValue(Stats.TotalEntityCount);
	TotalEntityCount->SetText(TotalEntityText + Value);

	Value.SetValue(Stats.DrawableEntityCount);
	DrawableEntityCount->SetText(DrawableEntityText + Value);

	Value.SetValue(Stats.VisibleEntityCount);
	VisibleEntityCount->SetText(VisibleEntityText + Value);

	Value.SetValue(Stats.CulledEntityCount);
	CulledEntityCount->SetText(CulledEntityText + Value);

	Value.SetValue(Stats.DrawedEntityCount);
	DrawedEntityCount->SetText(DrawedEntityText + Value);

	Value.SetValue(Stats.TotalLightCount);
	TotalLightCount->SetText(TotalLightText + Value);

	Value.SetValue(Stats.VisibleLightCount);
	VisibleLightCount->SetText(VisibleLightText + Value);

	Value.SetValue(Stats.CulledLightCount);
	CulledLightCount->SetText(CulledLightText + Value);

	Value.SetValue(Stats.DrawedLightCount);
	DrawedLightCount->SetText(DrawedLightText + Value);

	Value.SetValue(AverageFPS);
	FPSCount->SetText(FPSText + Value);

	static char FormatString[100];

	sprintf(FormatString, "X: %.3lf, Y: %.3lf, Z: %.3lf", CamPos.x, CamPos.y, CamPos.z);

	Value.SetValue(FormatString);
	CameraPosition->SetText(CamPosText + Value);

	ZEUIControl::Tick(ElapsedTime);
}

ZEUISceneStatisticsControl::ZEUISceneStatisticsControl()
{
	TotalEntityCount = new ZEUITextControl();
	TotalEntityCount->SetWidth(250.0f);
	TotalEntityCount->SetHeight(25.0f);
	TotalEntityCount->SetPosition(ZEVector2(0.0f, 0.0f));
	TotalEntityCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	TotalEntityCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(TotalEntityCount);

	DrawableEntityCount= new ZEUITextControl();
	DrawableEntityCount->SetWidth(250.0f);
	DrawableEntityCount->SetHeight(25.0f);
	DrawableEntityCount->SetPosition(ZEVector2(0.0f, 25.0f));
	DrawableEntityCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	DrawableEntityCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(DrawableEntityCount);

	VisibleEntityCount= new ZEUITextControl();
	VisibleEntityCount->SetWidth(250.0f);
	VisibleEntityCount->SetHeight(25.0f);
	VisibleEntityCount->SetPosition(ZEVector2(0.0f, 50.0f));
	VisibleEntityCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	VisibleEntityCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(VisibleEntityCount);

	CulledEntityCount = new ZEUITextControl();
	CulledEntityCount->SetWidth(250.0f);
	CulledEntityCount->SetHeight(25.0f);
	CulledEntityCount->SetPosition(ZEVector2(0.0f, 75.0f));
	CulledEntityCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	CulledEntityCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(CulledEntityCount);

	DrawedEntityCount = new ZEUITextControl();
	DrawedEntityCount->SetWidth(250.0f);
	DrawedEntityCount->SetHeight(25.0f);
	DrawedEntityCount->SetPosition(ZEVector2(0.0f, 100.0f));
	DrawedEntityCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	DrawedEntityCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(DrawedEntityCount);

	TotalLightCount = new ZEUITextControl();
	TotalLightCount->SetWidth(250.0f);
	TotalLightCount->SetHeight(25.0f);
	TotalLightCount->SetPosition(ZEVector2(0.0f, 150.0f));
	TotalLightCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	TotalLightCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(TotalLightCount);

	VisibleLightCount = new ZEUITextControl();
	VisibleLightCount->SetWidth(250.0f);
	VisibleLightCount->SetHeight(25.0f);
	VisibleLightCount->SetPosition(ZEVector2(0.0f, 175.0f));
	VisibleLightCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	VisibleLightCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(VisibleLightCount);

	CulledLightCount = new ZEUITextControl();
	CulledLightCount->SetWidth(250.0f);
	CulledLightCount->SetHeight(25.0f);
	CulledLightCount->SetPosition(ZEVector2(0.0f, 200.0f));
	CulledLightCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	CulledLightCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(CulledLightCount);
	
	DrawedLightCount = new ZEUITextControl();
	DrawedLightCount->SetWidth(250.0f);
	DrawedLightCount->SetHeight(25.0f);
	DrawedLightCount->SetPosition(ZEVector2(0.0f, 225.0f));
	DrawedLightCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	DrawedLightCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(DrawedLightCount);

	FPSCount = new ZEUITextControl();
	FPSCount->SetWidth(250.0f);
	FPSCount->SetHeight(25.0f);
	FPSCount->SetPosition(ZEVector2(0.0f, 250.0f));
	FPSCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	FPSCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(FPSCount);

	CameraPosition = new ZEUITextControl();
	CameraPosition->SetWidth(600.0f); //450.0f
	CameraPosition->SetHeight(25.0f);
	CameraPosition->SetPosition(ZEVector2(0.0f, 275.0f));
	CameraPosition->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	CameraPosition->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(CameraPosition);

	this->SetHeight(300.0f);
	this->SetWidth(600.0f);

// 	int WindowWidth = 0;
// 	int WindowHeight = 0;
// 	zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);
//	WindowWidth - GetWidth()

	this->SetPosition(ZEVector2(3.0f, 0.0f));
	
}
	
	
