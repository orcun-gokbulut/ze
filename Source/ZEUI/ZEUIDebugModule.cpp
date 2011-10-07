//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIDebugModule.cpp
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

#include "ZEUIDebugModule.h"
#include "ZEGame/ZEGrid.h"
#include "ZEGame/ZEPlayer.h"
#include "ZEGame/ZEGame.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZECamera.h"

#include "ZEUI/ZEUICursorControl.h"
#include "ZEUI/ZEUIManager.h"
#include "ZEUI/ZEUIButtonControl.h"
#include "ZEUI/ZEIUCheckBoxControl.h"
#include "ZEUI/ZEUIHorizontalSliderControl.h"
#include "ZEUI/ZEUIVerticalSliderControl.h"
#include "ZEUI/ZEUIFrameControl.h"
#include "ZEUI/ZEUIWindowControl.h"
#include "ZEUI/ZEUTextEditControl.h"
#include "ZEUI/ZEUIMultiLineTextEdit.h"
#include "ZEUI/ZEUIRadioButtonControl.h"
#include "ZEUI/ZEUIBorderedFrameControl.h"

#include <Windows.h>

bool ZEUIDebugModule::Initialize()
{
	ShowCursor(false);

	Scene = zeGame->GetScene();

	if (Player == NULL)
	{
		Player = ZEPlayer::CreateInstance();
		Player->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
		Player->SetRotation(ZEQuaternion::Identity);
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
		Scene->SetActiveCamera(Player->GetCamera());
		Scene->AddEntity(Player);
	}

	if (Grid == NULL)
		Grid = ZEGrid::CreateInstance();

	ZEUICursorControl* Cursor = new ZEUICursorControl();
	ZEUIButtonControl* Button = new ZEUIButtonControl();
	ZEUICheckBoxControl* CheckBox = new ZEUICheckBoxControl();
	ZEUIHorizontalSliderControl* HorizontalSlider = new ZEUIHorizontalSliderControl();
	ZEUIVerticalSliderControl*	 VerticalSlider = new ZEUIVerticalSliderControl();
	ZEUIFrameControl* Frame = new ZEUIFrameControl();
	ZEUIWindowControl* Window = new ZEUIWindowControl();
	ZEUIWindowControl* Window2 = new ZEUIWindowControl();
	ZEUITextEditControl* LineEdit = new ZEUITextEditControl();
	ZEUITextEditControl* LineEdit2 = new ZEUITextEditControl();
	ZEUIMultiLineTextEdit* MultiLine = new ZEUIMultiLineTextEdit();

	ZEUIRadioButtonControl* RadioButton1 = new ZEUIRadioButtonControl();
	ZEUIRadioButtonControl* RadioButton2 = new ZEUIRadioButtonControl();
	ZEUIRadioButtonControl* RadioButton3 = new ZEUIRadioButtonControl();

	ZEUIBorderedFrameControl*	BorderedFrame = new ZEUIBorderedFrameControl();

	RadioButton1->AddInteractingRadioButton(RadioButton2);
	RadioButton1->AddInteractingRadioButton(RadioButton3);

	RadioButton2->AddInteractingRadioButton(RadioButton1);
	RadioButton2->AddInteractingRadioButton(RadioButton3);

	RadioButton3->AddInteractingRadioButton(RadioButton2);
	RadioButton3->AddInteractingRadioButton(RadioButton1);

	
	//zeGame->UIManager->AddControl(Button);
	zeGame->UIManager->AddControl(CheckBox);
	zeGame->UIManager->AddControl(RadioButton1);
	zeGame->UIManager->AddControl(RadioButton2);
	zeGame->UIManager->AddControl(RadioButton3);
	//zeGame->UIManager->AddControl(HorizontalSlider);
	//zeGame->UIManager->AddControl(VerticalSlider);
	//zeGame->UIManager->AddControl(Frame);
	zeGame->UIManager->AddControl(Window);
	//zeGame->UIManager->AddControl(Window2);
	zeGame->UIManager->AddControl(LineEdit);
	//zeGame->UIManager->AddControl(LineEdit2);
	zeGame->UIManager->AddControl(MultiLine);
	zeGame->UIManager->AddControl(BorderedFrame);

	BorderedFrame->SetPosition(ZEVector2(100,100));
	BorderedFrame->SetHeight(25);
	BorderedFrame->SetWidth(80);
	//BorderedFrame->SetWidth(500);
	//BorderedFrame->SetHeight(500);
	//BorderedFrame->SetElementVisibilities(ZEUI_BFC_EL_TOPEDGE | ZEUI_BFC_EL_BOTTOMEDGE | ZEUI_BFC_EL_LEFTEDGE | ZEUI_BFC_EL_RIGHTEDGE, false);
	//BorderedFrame->SetElementVisibilities(ZEUI_BFC_EL_LEFTUPCORNER | ZEUI_BFC_EL_LEFTDOWNCORNER | ZEUI_BFC_EL_RIGHTUPCORNER | ZEUI_BFC_EL_RIGHTDOWNCORNER, false);
	BorderedFrame->SetBorderThickness(6);

	LineEdit->SetPosition(ZEVector2(500,450));
	LineEdit2->SetPosition(ZEVector2(70,100));
	MultiLine->SetPosition(ZEVector2(300,300));

	Window->AddChildControl(HorizontalSlider);
	//Window->AddChildControl(LineEdit2);
	Window2->AddChildControl(VerticalSlider);

	HorizontalSlider->SetPosition(ZEVector2(70,70));
	VerticalSlider->SetPosition(ZEVector2(70,70));

	Frame->SetPosition(ZEVector2(300,300));

	HorizontalSlider->SetWidth(200);
	VerticalSlider->SetHeight(200);

	Button->SetPosition(ZEVector2(20,100));

	CheckBox->SetPosition(ZEVector2(500,500));
	CheckBox->SetTriState(true);

	RadioButton1->SetPosition(ZEVector2(500,550));
	RadioButton2->SetPosition(ZEVector2(500,580));
	RadioButton3->SetPosition(ZEVector2(500,610));

	zeGame->UIManager->AddControl(Cursor);
	zeGame->UIManager->SetActiveCursor(Cursor);

	RadioButton1->SetText("RadioButton1");
	RadioButton2->SetText("RadioButton2");
	RadioButton3->SetText("RadioButton3");

	CheckBox->SetText("CheckBox");

	return true;
}

void ZEUIDebugModule::Deinitialize()
{

}

void ZEUIDebugModule::Process(float ElapsedTime)
{

}

ZEUIDebugModule::ZEUIDebugModule()
{
	Player = NULL;
	Grid = NULL;
	Scene = NULL;
}

ZEUIDebugModule::~ZEUIDebugModule()
{

}
