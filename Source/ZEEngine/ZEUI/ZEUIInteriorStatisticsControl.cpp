//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIInteriorStatisticsControl.cpp
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

#include "ZEUIInteriorStatisticsControl.h"
#include "ZEDS/ZEString.h"
#include "ZEUI/ZEFontResource.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZECamera.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZEInterior/ZEInterior.h"

#define TotalRoomText		ZEString("Total Room Count		: ")
#define CulledRoomText		ZEString("Culled Room Count		: ")
#define DrawedRoomText		ZEString("Drawn Room Count		: ")
#define TotalPolygonText    ZEString("Total Polygon Count   : ")
#define CulledPolygonText	ZEString("Culled Polygon Count  : ")
#define DrawedPolygonText   ZEString("Drawn Polygon Count   : ")
#define FPSText				ZEString("Frame Per Second      : ")
#define CamPosText			ZEString("Camera Position       : ")

void ZEUIInteriorStatisticsControl::SetMaterial( ZEMaterial* Material )
{

}

ZEMaterial* ZEUIInteriorStatisticsControl::GetMaterial() const
{
	return NULL;
}

void ZEUIInteriorStatisticsControl::Draw( ZEUIRenderer* Renderer )
{
	ZEUIControl::Draw(Renderer);
}

void ZEUIInteriorStatisticsControl::Tick( float ElapsedTime )
{
	//Portal Culling Calculation

	ZEScene* TempScene = zeScene;
	ZEInterior* TempMap = (ZEInterior*)TempScene->GetEntities(ZEInterior::Description()).GetFirstItem();
	ZEVector3 CamPos = TempScene->GetActiveCamera()->GetWorldPosition();
	ZEInteriorCullStatistics Stats = TempMap->GetCullStatistics();
	ZEString Value;

	Value.SetValue(Stats.TotalRoomCount);
	TotalRoomCount->SetText(TotalRoomText + Value);

	Value.SetValue(Stats.CulledRoomCount);
	CulledRoomCount->SetText(CulledRoomText + Value);

	Value.SetValue(Stats.DrawedRoomCount);
	DrawedRoomCount->SetText(DrawedRoomText + Value);

	Value.SetValue(Stats.TotalInteriorPolygonCount);
	TotalPolygonCount->SetText(TotalPolygonText + Value);

	Value.SetValue(Stats.CulledInteriorPolygonCount);
	CulledPolygonCount->SetText(CulledPolygonText + Value);

	Value.SetValue(Stats.DrawedInteriorPolygonCount);
	DrawedPolygonCount->SetText(DrawedPolygonText + Value);

	ZEUIControl::Tick(ElapsedTime);
}

ZEUIInteriorStatisticsControl::ZEUIInteriorStatisticsControl()
{
	TotalRoomCount = new ZEUITextControl();
	TotalRoomCount->SetWidth(250.0f);
	TotalRoomCount->SetHeight(25.0f);
	TotalRoomCount->SetPosition(ZEVector2(0.0f, 0.0f));
	TotalRoomCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	TotalRoomCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(TotalRoomCount);

	CulledRoomCount = new ZEUITextControl();
	CulledRoomCount->SetWidth(250.0f);
	CulledRoomCount->SetHeight(25.0f);
	CulledRoomCount->SetPosition(ZEVector2(0.0f, 25.0f));
	CulledRoomCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	CulledRoomCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(CulledRoomCount);

	DrawedRoomCount = new ZEUITextControl();
	DrawedRoomCount->SetWidth(250.0f);
	DrawedRoomCount->SetHeight(25.0f);
	DrawedRoomCount->SetPosition(ZEVector2(0.0f, 50.0f));
	DrawedRoomCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	DrawedRoomCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(DrawedRoomCount);

	TotalPolygonCount = new ZEUITextControl();
	TotalPolygonCount->SetWidth(250.0f);
	TotalPolygonCount->SetHeight(25.0f);
	TotalPolygonCount->SetPosition(ZEVector2(0.0f, 100.0f));
	TotalPolygonCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	TotalPolygonCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(TotalPolygonCount);

	CulledPolygonCount = new ZEUITextControl();
	CulledPolygonCount->SetWidth(250.0f);
	CulledPolygonCount->SetHeight(25.0f);
	CulledPolygonCount->SetPosition(ZEVector2(0.0f, 125.0f));
	CulledPolygonCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	CulledPolygonCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(CulledPolygonCount);

	DrawedPolygonCount = new ZEUITextControl();
	DrawedPolygonCount->SetWidth(250.0f);
	DrawedPolygonCount->SetHeight(25.0f);
	DrawedPolygonCount->SetPosition(ZEVector2(0.0f, 150.0f));
	DrawedPolygonCount->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
	DrawedPolygonCount->SetTextColor(ZEVector4::HomogenousUnitY);
	AddChildControl(DrawedPolygonCount);

	this->SetHeight(175.0f);
	this->SetWidth(250.0f);

	this->SetPosition(ZEVector2(3.0f, 0.0f));
}
