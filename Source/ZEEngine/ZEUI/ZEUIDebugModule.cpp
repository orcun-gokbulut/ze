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
#include "ZEUI/ZEUIScrollArea.h"
#include "ZEUI/ZEUILoadingBar.h"
#include "ZETexture/ZETexture2DResource.h"


#include <Windows.h>
#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZEUIConsole.h"

void ZEUIDebugModule::AddFrameInFrameControl()
{
	ZEUIFrameControl* Frame1 = new ZEUIFrameControl();
	zeGame->UIManager->AddControl(Frame1);
	Frame1->SetPosition(30, 30);
	Frame1->SetSize(ZEVector2(700, 700));
	//	Frame1->SetBackgroundColor(ZEVector4(0.0f, 0.0f, 0.0f, 0.1f));

	ZEUIFrameControl* Frame = new ZEUIFrameControl();
	//	zeGame->UIManager->AddControl(Frame);
	Frame1->AddChildControl(Frame);
	Frame->SetPosition(ZEVector2(20, 20));
	Frame->SetSize(ZEVector2(200, 200));

	ZEUIMaterial* Mate = ZEUIMaterial::CreateInstance();
	Mate->SetTexture((ZETexture2DResource::LoadResource("50-cent1.jpg")->GetTexture())); 	
	Frame->SetMaterial(Mate);

	Frame1->SetMaterial(Mate);
}

void ZEUIDebugModule::AddButtonControl()
{
	ZEUIButtonControl* PostButton = new ZEUIButtonControl();
	zeGame->UIManager->AddControl(PostButton);
	PostButton->SetPosition(ZEVector2(200, 200));
	PostButton->SetHeight(25);
	PostButton->SetWidth(80);
}

void ZEUIDebugModule::AddCheckboxControlInFrame()
{
	ZEUIFrameControl* Frame1 = new ZEUIFrameControl();
	zeGame->UIManager->AddControl(Frame1);
	Frame1->SetPosition(30, 30);
	Frame1->SetSize(ZEVector2(200, 200));

	ZEUICheckBoxControl* Checkbox1 = new ZEUICheckBoxControl();
//	zeGame->UIManager->AddControl(Checkbox1);
	Frame1->AddChildControl(Checkbox1);
	Checkbox1->SetPosition(ZEVector2(10, 10));
	Checkbox1->SetText("ccc");

	ZEUIMaterial* Mate = ZEUIMaterial::CreateInstance();
	Mate->SetTexture((ZETexture2DResource::LoadResource("50-cent1.jpg")->GetTexture())); 

	Frame1->SetMaterial(Mate);
}

void ZEUIDebugModule::AddCheckboxControl()
{
	ZEUICheckBoxControl* Checkbox1 = new ZEUICheckBoxControl();
	zeGame->UIManager->AddControl(Checkbox1);
	Checkbox1->SetPosition(ZEVector2(10, 10));
	Checkbox1->SetText("ccc");
}

void ZEUIDebugModule::AddFrameInBorderedFrameControl()
{
	ZEUIBorderedFrameControl* BorderedFrame = new ZEUIBorderedFrameControl();
	zeGame->UIManager->AddControl(BorderedFrame);
	BorderedFrame->SetPosition(ZEVector2(200, 200));
	BorderedFrame->SetSize(ZEVector2(500, 300));
	BorderedFrame->SetBorderThickness(2);

	ZEUIFrameControl* Frame = new ZEUIFrameControl();
	//	zeGame->UIManager->AddControl(Frame);
	BorderedFrame->AddChildControl(Frame);
	Frame->SetPosition(ZEVector2(20, 20));
	Frame->SetSize(ZEVector2(300, 200));

	ZEUIMaterial* Mate = ZEUIMaterial::CreateInstance();
	Mate->SetTexture((ZETexture2DResource::LoadResource("50-cent1.jpg")->GetTexture())); 	
	Frame->SetMaterial(Mate);

	BorderedFrame->SetMaterial(Mate);
}

void ZEUIDebugModule::AddConsole()
{
	ZEUIConsole* Console = new ZEUIConsole();
	zeGame->UIManager->AddControl(Console);
	zeCore->GetConsole()->SetConsoleInterface(Console);
	Console->ShowConsole();
	Console->SetNotificationEnabled(true);
	Console->SetPosition(ZEVector2(0 , 400));
}

void ZEUIDebugModule::AddFrameInScrollArea()
{
	ZEUIScrollArea* Scroll = new ZEUIScrollArea();
	zeGame->UIManager->AddControl(Scroll);
	Scroll->SetPosition(ZEVector2(30, 30));
	Scroll->SetSize(ZEVector2(200, 200));

	ZEUIFrameControl* Frame = new ZEUIFrameControl();
	//	zeGame->UIManager->AddControl(Frame);
	Scroll->AddChildControl(Frame);
	Frame->SetPosition(ZEVector2(20, 20));
	Frame->SetSize(ZEVector2(300, 300));

	ZEUIMaterial* Mate = ZEUIMaterial::CreateInstance();
	Mate->SetTexture((ZETexture2DResource::LoadResource("50-cent1.jpg")->GetTexture())); 	
	Frame->SetMaterial(Mate);

	Scroll->UpdateScrollbars();
	
}

void ZEUIDebugModule::AddCursor()
{
	ZEUICursorControl* Cursor = new ZEUICursorControl();
	zeGame->UIManager->AddControl(Cursor);
	zeGame->UIManager->SetActiveCursor(Cursor);
	Cursor->SetZOrder(4000000);
}

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

	/*
	MEANING INDEX
	1 - Positioning Done

	*/

//	AddFrameInFrameControl(); // 1
//	AddButtonControl(); // 1
//	AddCheckboxControlInFrame(); // 
	AddCheckboxControl(); // 1
//	AddFrameInBorderedFrameControl(); // 1
//	AddFrameInScrollArea();

	AddConsole();
	AddCursor();
	
// 
// 	ZEUIMultiLineTextEdit* MLTextEdit = new ZEUIMultiLineTextEdit();
// 	zeGame->UIManager->AddControl(MLTextEdit);
// 	MLTextEdit->SetPosition(ZEVector2(200, 100));
// 	MLTextEdit->SetWidth(300);
// 	MLTextEdit->SetHeight(100);
// 	MLTextEdit->SetText("asfdafsafsdasfdasfdasfd\nhghdgfdhgfghdfdhgfhgdf");
// 	MLTextEdit->SetVerticalScrollbarPolicy(SCROLLBAR_AS_NEEDED);
// 
// 	ZEUITextEditControl* TextEdit = new ZEUITextEditControl();
// 	zeGame->UIManager->AddControl(TextEdit);
// 	TextEdit->SetPosition(ZEVector2(200, 200));
// 	TextEdit->SetWidth(250);
// 	TextEdit->SetHeight(20);
// 	TextEdit->SetText("asfdasfdasfdasfdasdf");

// 	ZEUIButtonControl* PostButton = new ZEUIButtonControl();
// 	zeGame->UIManager->AddControl(PostButton);
// 	PostButton->SetPosition(ZEVector2(451, 200));
// 	PostButton->SetHeight(20);
// 	PostButton->SetWidth(49);

// 	ZERectangle cc = ScrollArea->GetViewport()->GetVisibleRectangle();
// 	ZERectangle ccc = ScrollArea->GetViewport()->GetChildControls().;

// 	ZEUIBorderedFrameControl* BorderedFrameControl = new ZEUIBorderedFrameControl();
// 	zeGame->UIManager->AddControl(BorderedFrameControl);
// 	BorderedFrameControl->SetPosition(ZEVector2(500,50));

// 	BorderedFrameControl->SetElementVisibilities(ZEUI_BFC_EL_ALL, true);
// 	BorderedFrameControl->SetBorderThickness(20.0f);
// 	BorderedFrameControl->SetMoveable(true);


// 	ZEUITextControl* Text = new ZEUITextControl();
// 	zeGame->UIManager->AddControl(Text);
// 	Text->SetPosition(ZEVector2(500, 500));
// 	Text->SetText(ZEString("Text Deneme"));

// 	ZEUIRadioButtonControl* RadioButton1 = new ZEUIRadioButtonControl();
// 	zeGame->UIManager->AddControl(RadioButton1);
// 	RadioButton1->SetPosition(ZEVector2(0, 40));
// 
// 	ZEUIRadioButtonControl* RadioButton2 = new ZEUIRadioButtonControl();
// 	zeGame->UIManager->AddControl(RadioButton2);
// 	RadioButton2->SetPosition(ZEVector2(0, 70));
// 
// 	RadioButton1->AddInteractingRadioButton(RadioButton2);
// 	RadioButton2->AddInteractingRadioButton(RadioButton1);

// 	ZEUIVerticalSliderControl* VSlider = new ZEUIVerticalSliderControl();
// 	zeGame->UIManager->AddControl(VSlider);
// 	VSlider->SetPosition(ZEVector2(0, 130));
// 
// 	ZEUIHorizontalSliderControl* HSlider = new ZEUIHorizontalSliderControl();
// 	zeGame->UIManager->AddControl(HSlider);
// 	HSlider->SetPosition(ZEVector2(0, 200));

	/*ZEUICheckBoxControl* Checkbox1 = new ZEUICheckBoxControl();
	zeGame->UIManager->AddControl(Checkbox1);
	Checkbox1->SetPosition(ZEVector2(0, 250));

	ZEUICheckBoxControl* Checkbox2 = new ZEUICheckBoxControl();
	zeGame->UIManager->AddControl(Checkbox2);
	Checkbox2->SetPosition(ZEVector2(30, 280));

	ZEUICheckBoxControl* Checkbox3 = new ZEUICheckBoxControl();
	zeGame->UIManager->AddControl(Checkbox3);
	Checkbox3->SetPosition(ZEVector2(30, 310));
*/

// 	ZEUIWindowControl* Window = new ZEUIWindowControl();
// 
// 	ZEUILoadingBar* LoadingBar = new ZEUILoadingBar();
// //	zeGame->UIManager->AddControl(LoadingBar);
// 	Window->AddChildControl(LoadingBar);
// 	LoadingBar->SetPosition(ZEVector2(10, 10));
//  	LoadingBar->SetBarTexture("SemiChecked.png");
// 	LoadingBar->SetFrameTexture("UnChecked.png");
// // 	LoadingBar->SetBarColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
// // 	LoadingBar->SetFrameColor(ZEVector4(0.0f, 1.0f, 0.0f, 1.0f));
// 	LoadingBar->SetBarUVMode(ZE_UI_UVM_TILE);
// 	LoadingBar->SetFrameUVMode(ZE_UI_UVM_TILE);
// 	LoadingBar->SetValue(100);



// 	ZEUITextControl* Text = new ZEUITextControl();
// //	zeGame->UIManager->AddControl(Text);
// //	Window->AddChildControl(Text);
// 	Text->SetPosition(ZEVector2(0, 0));
// 	Text->SetSize(ZEVector2(250, 250));
// 	Text->SetTextColor(ZEVector4(0.0f, 0.0f, 0.0f, 1.0f));
// 	Text->SetFontSize(ZEVector2::One);
// 	Text->SetTextWrap(true);
// 	Text->SetFont(ZEFontResource::LoadResource("Courier New.zeFont"));
// 	Text->SetText(ZEString("Text Deneme adsfsadsfdafsd sfdas fadsafd asfd asfd sadf asdf sd fs afdsdf sdafdfas safdsadf "));
// 
// 	zeGame->UIManager->AddControl(Window);
// 	Window->SetPosition(ZEVector2(50, 50));
// 	Window->SetSize(ZEVector2(250, 250));
// 
// 
// 	ZEUIScrollArea* Scroll = new ZEUIScrollArea();
// 	zeGame->UIManager->AddControl(Scroll);
// //	Window->AddChildControl(Scroll);
// 	Scroll->SetPosition(ZEVector2(0, 30));
// 	Scroll->SetSize(ZEVector2(200, 200));
// 
// 



// 	Scroll->UpdateScrollbars();
// 
// 	ZEUIRadioButtonControl* RadioButton = new ZEUIRadioButtonControl();
// 	zeGame->UIManager->AddControl(RadioButton);
// 	RadioButton->SetPosition(ZEVector2(0, 300));
// 
// 	ZEUIConsole* Console = new ZEUIConsole();
// 	zeGame->UIManager->AddControl(Console);
// 	zeCore->GetConsole()->SetConsoleInterface(Console);
// 	Console->ShowConsole();
// 	Console->SetNotificationEnabled(true);


// 
// 	ZEGrid* Grid = ZEGrid::CreateInstance();
// 	Scene->AddEntity(Grid);
// 
// 	ZEVector2 a = Window->TitleBar.GetScreenPosition();
// 	ZEVector2 b = Window->ContentArea.GetScreenPosition();
// 	ZEVector2 c = Window->ContentArea.GetChildControls()[0]->GetScreenPosition();
// 	ZEVector2 d = Window->ContentArea.GetChildControls()[1]->GetScreenPosition();
// 
// 	ZEVector2 e = Window->TitleBar.GetPosition();
// 	ZEVector2 f = Window->ContentArea.GetPosition();
// 	ZEVector2 g = Window->ContentArea.GetChildControls()[0]->GetPosition();
// 	ZEVector2 h = Window->ContentArea.GetChildControls()[1]->GetPosition();
// 
// 	ZERectangle aa = Window->TitleBar.GetScreenRectangle();
// 	ZERectangle bb = Window->ContentArea.GetScreenRectangle();
// 	ZERectangle cc = Window->ContentArea.GetChildControls()[0]->GetScreenRectangle();
// 	ZERectangle dd = Window->ContentArea.GetChildControls()[1]->GetScreenRectangle();
// 
// 	ZERectangle ee = Window->TitleBar.GetRectangle();
// 	ZERectangle ff = Window->ContentArea.GetRectangle();
// 	ZERectangle gg = Window->ContentArea.GetChildControls()[0]->GetRectangle();
// 	ZERectangle hh = Window->ContentArea.GetChildControls()[1]->GetRectangle();
// 
// 	ZERectangle aaa = Window->GetVisibleRectangle();
// 	ZERectangle bbb = Window->ContentArea.GetVisibleRectangle();
// 	ZERectangle ccc = Window->ContentArea.GetChildControls()[0]->GetVisibleRectangle();
// 	ZERectangle ddd = Window->ContentArea.GetChildControls()[1]->GetVisibleRectangle();

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
