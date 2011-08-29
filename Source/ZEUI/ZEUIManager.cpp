//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIManager.cpp
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

#include "ZEUIManager.h"
#include "ZEUIRenderer.h"
#include "ZEUIControl.h"
#include "ZECore\ZEError.h"
#include "ZEGraphics\ZEFixedMaterial.h"
#include "ZEInput\ZEInputDefinitions.h"
#include "ZEInput\ZEInputModule.h"

#define ACTIONID_Q 0
#define	ACTIONID_W 1
#define	ACTIONID_E 2
#define	ACTIONID_R 3
#define	ACTIONID_T 4
#define	ACTIONID_Y 5
#define	ACTIONID_U 6
#define	ACTIONID_I 7
#define	ACTIONID_O 8
#define	ACTIONID_P 9
#define	ACTIONID_A 10
#define	ACTIONID_S 11
#define	ACTIONID_D 12
#define	ACTIONID_F 13
#define	ACTIONID_G 14
#define	ACTIONID_H 15
#define	ACTIONID_J 16
#define	ACTIONID_K 17
#define	ACTIONID_L 18
#define	ACTIONID_Z 19
#define	ACTIONID_X 20
#define	ACTIONID_C 21
#define	ACTIONID_V 22
#define	ACTIONID_B 23
#define	ACTIONID_N 24
#define	ACTIONID_M 25
#define	ACTIONID_SPACE		26
#define	ACTIONID_BACKSPACE  27
#define	ACTIONID_LEFTARROW  28
#define	ACTIONID_RIGHTARROW 29
#define	ACTIONID_UPARROW    30
#define	ACTIONID_DOWNARROW  31

ZEUIManager::ZEUIManager() 
{
	UIRenderer = NULL;
	OldMousePosition = ZEVector2::Zero;
	LastHoveredControl = NULL;
	LastPressedControl = NULL;
	LastFocusedControl = NULL;
	MouseMoveEventFlag = false;
	Cursor = NULL;

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_Q, 			ZEInputEvent("Keyboard", ZE_IKB_Q, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_W, 			ZEInputEvent("Keyboard", ZE_IKB_W, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_E, 			ZEInputEvent("Keyboard", ZE_IKB_E, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_R, 			ZEInputEvent("Keyboard", ZE_IKB_R, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_T, 			ZEInputEvent("Keyboard", ZE_IKB_T, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_Y, 			ZEInputEvent("Keyboard", ZE_IKB_Y, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_U, 			ZEInputEvent("Keyboard", ZE_IKB_U, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_I, 			ZEInputEvent("Keyboard", ZE_IKB_I, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_O, 			ZEInputEvent("Keyboard", ZE_IKB_O, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_P, 			ZEInputEvent("Keyboard", ZE_IKB_P, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_A, 			ZEInputEvent("Keyboard", ZE_IKB_A, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_S, 			ZEInputEvent("Keyboard", ZE_IKB_S, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_D, 			ZEInputEvent("Keyboard", ZE_IKB_D, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_F, 			ZEInputEvent("Keyboard", ZE_IKB_F, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_G, 			ZEInputEvent("Keyboard", ZE_IKB_G, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_H, 			ZEInputEvent("Keyboard", ZE_IKB_H, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_J, 			ZEInputEvent("Keyboard", ZE_IKB_J, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_K, 			ZEInputEvent("Keyboard", ZE_IKB_K, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_L, 			ZEInputEvent("Keyboard", ZE_IKB_L, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_Z, 			ZEInputEvent("Keyboard", ZE_IKB_Z, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_X, 			ZEInputEvent("Keyboard", ZE_IKB_X, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_C, 			ZEInputEvent("Keyboard", ZE_IKB_C, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_V, 			ZEInputEvent("Keyboard", ZE_IKB_V, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_B, 			ZEInputEvent("Keyboard", ZE_IKB_B, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_N,			ZEInputEvent("Keyboard", ZE_IKB_N, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_M,			ZEInputEvent("Keyboard", ZE_IKB_M, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_SPACE,		ZEInputEvent("Keyboard", ZE_IKB_SPACE, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_BACKSPACE,	ZEInputEvent("Keyboard", ZE_IKB_BACKSPACE, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_LEFTARROW,	ZEInputEvent("Keyboard", ZE_IKB_LEFTARROW, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_RIGHTARROW,	ZEInputEvent("Keyboard", ZE_IKB_RIGHTARROW, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_UPARROW,		ZEInputEvent("Keyboard", ZE_IKB_UPARROW, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_DOWNARROW,	ZEInputEvent("Keyboard", ZE_IKB_DOWNARROW, ZE_IBS_PRESSED)));
}

ZEUIManager::~ZEUIManager() 
{
	if (UIRenderer != NULL)
		UIRenderer->Destroy();
}

void ZEUIManager::SetActiveCursor(ZEUICursorControl* Cursor)
{
	this->Cursor = Cursor;
}

void ZEUIManager::AddControl(ZEUIControl* Control)
{
	zeAssert(Controls.FindIndex(Control) != -1, "UI Control already added to ZEUIManager. (Control Name : %s)", Control->GetName());

	Controls.Add(Control);
}

void ZEUIManager::RemoveControl(ZEUIControl* Control)
{
	Controls.DeleteValue(Control);
}

ZEArray<ZEUIControl*>& ZEUIManager::GetControls()
{
	return Controls;
}

#include "ZEUITextControl.h"
#include "ZEFontResource.h"
#include "ZEUICursorControl.h"
#include "ZEUIButtonControl.h"

bool ZEUIManager::Initialize()
{
	if (UIRenderer == NULL)
		UIRenderer = ZEUIRenderer::CreateInstance();

	UIRenderer->Initialize();

	return true;
}

void ZEUIManager::Deinitialize()
{
	UIRenderer->Destroy();
	UIRenderer = NULL;
}

void ZEUIManager::ProcessEvents()
{
	if (Cursor)
	{
		ZEVector2 CursorPosition = Cursor->GetPosition();

		/************************************************************************/
		/*                          HOVER ENTER AND LEAVE                       */
		/************************************************************************/
		if (OldMousePosition != CursorPosition)
		{
			for (int I = 0; I < Controls.GetCount(); I++)
			{
				if (Controls[I] == Cursor)
				{
					continue;
				}

				if (Controls[I]->GetVisiblity() == false)
					continue;

				if (LastHoveredControl != NULL && ZERectangle::BoundingTest(LastHoveredControl->GetVisibleRectangle(), CursorPosition) == false)
				{
					LastHoveredControl->MouseLeaveEvent(CursorPosition);
					LastHoveredControl = NULL;
				}

				ZEUIControl* Reciever = NULL;

				for (int I = 0; I < Controls.GetCount(); I++)
				{
					if (Controls[I]->GetVisiblity() == false)
						continue;

					if (ZERectangle::BoundingTest(Controls[I]->GetVisibleRectangle(), CursorPosition))
					{

						if(Controls[I] == Cursor)
							continue;

						if (Controls[I]->GetChildControls().GetCount() == 0)
						{
							Reciever = Controls[I];
						}

						else
						{
							Reciever = FindEventReciever(Controls[I]);
						}
					}
				}

				if (Reciever != NULL && Reciever != LastHoveredControl && MouseMoveEventFlag == false && !Reciever->GetHovered())
				{
					Reciever->MouseEnterEvent(CursorPosition);

					if (LastHoveredControl != NULL)
						LastHoveredControl->SetHovered(false);

					LastHoveredControl = Reciever;
				}
			}
		}
		/************************************************************************/
		/*                          HOVER ENTER AND LEAVE                       */
		/************************************************************************/

		/************************************************************************/
		/*        MOUSE PRESS, RELEASE EVENT AND FOCUSGAIN, FOCUSLOST           */
		/************************************************************************/

		if (Cursor->GetCurrentButton() != ZEUIMouseKey::ZE_UI_MOUSE_BUTTON_NONE && PreviousPressedButton == ZE_UI_MOUSE_BUTTON_NONE && LastHoveredControl != NULL)
		{
			LastHoveredControl->MouseButtonPressed(Cursor->GetCurrentButton(), CursorPosition);
			LastPressedControl = LastHoveredControl;
			PreviousPressedButton = Cursor->GetCurrentButton();

			if (LastFocusedControl != NULL && LastFocusedControl != LastPressedControl && LastPressedControl->GetFocusable())
			{
				LastFocusedControl->FocusLost();
				LastFocusedControl = NULL;
			}

			if (LastPressedControl->GetFocusable() && !LastPressedControl->GetFocused() && LastFocusedControl == NULL)
			{
				LastPressedControl->FocusGained();
				LastFocusedControl = LastPressedControl;
			}
		}

		if (LastPressedControl != NULL && Cursor->GetCurrentButton() == ZEUIMouseKey::ZE_UI_MOUSE_BUTTON_NONE)
		{
			LastPressedControl->MouseButtonReleased(PressedButton, CursorPosition);
			MouseMoveEventFlag = false;

			if (ZERectangle::BoundingTest(LastPressedControl->GetVisibleRectangle(), CursorPosition))
			{
				LastHoveredControl = LastPressedControl;
			}

			LastPressedControl = NULL;
			PreviousPressedButton = ZE_UI_MOUSE_BUTTON_NONE;
		}

		if (LastPressedControl != NULL)
			MouseMoveEventFlag = true;

		if (MouseMoveEventFlag == true && LastPressedControl != NULL)
		{
			if (LastPressedControl != Cursor && OldMousePosition != CursorPosition)
				LastPressedControl->MouseMoveEvent(OldMousePosition - CursorPosition);
		}

		/************************************************************************/
		/*        MOUSE PRESS, RELEASE EVENT AND FOCUSGAIN, FOCUSLOST           */
		/************************************************************************/

		OldMousePosition = CursorPosition;
	}

	/************************************************************************/
	/*                          KEYPRESS EVENTS                             */
	/************************************************************************/

	if (LastFocusedControl != NULL)
	{
		zeInput->ProcessInputMap(&InputMap);

		for (size_t I = 0; I < InputMap.InputActionCount; I++)
		{
			switch (InputMap.InputActions[I].Id)
			{
				case ACTIONID_Q:
					LastFocusedControl->KeyPressed('Q');
					break;

				case ACTIONID_W:
					LastFocusedControl->KeyPressed('W');
					break;

				case ACTIONID_E:
					LastFocusedControl->KeyPressed('E');
					break;

				case ACTIONID_R:
					LastFocusedControl->KeyPressed('R');
					break;

				case ACTIONID_T:
					LastFocusedControl->KeyPressed('T');
					break;

				case ACTIONID_Y:
					LastFocusedControl->KeyPressed('Y');
					break;

				case ACTIONID_U:
					LastFocusedControl->KeyPressed('U');
					break;

				case ACTIONID_I:
					LastFocusedControl->KeyPressed('I');
					break;

				case ACTIONID_O:
					LastFocusedControl->KeyPressed('O');
					break;

				case ACTIONID_P:
					LastFocusedControl->KeyPressed('P');
					break;

				case ACTIONID_A:
					LastFocusedControl->KeyPressed('A');
					break;

				case ACTIONID_S:
					LastFocusedControl->KeyPressed('S');
					break;

				case ACTIONID_D:
					LastFocusedControl->KeyPressed('D');
					break;

				case ACTIONID_F:
					LastFocusedControl->KeyPressed('F');
					break;

				case ACTIONID_G:
					LastFocusedControl->KeyPressed('G');
					break;

				case ACTIONID_H:
					LastFocusedControl->KeyPressed('H');
					break;

				case ACTIONID_J:
					LastFocusedControl->KeyPressed('J');
					break;

				case ACTIONID_K:
					LastFocusedControl->KeyPressed('K');
					break;

				case ACTIONID_L:
					LastFocusedControl->KeyPressed('L');
					break;

				case ACTIONID_Z:
					LastFocusedControl->KeyPressed('Z');
					break;

				case ACTIONID_X:
					LastFocusedControl->KeyPressed('X');
					break;

				case ACTIONID_C:
					LastFocusedControl->KeyPressed('C');
					break;

				case ACTIONID_V:
					LastFocusedControl->KeyPressed('V');
					break;

				case ACTIONID_B:
					LastFocusedControl->KeyPressed('B');
					break;

				case ACTIONID_N:
					LastFocusedControl->KeyPressed('N');
					break;

				case ACTIONID_M:
					LastFocusedControl->KeyPressed('M');
					break;

				case ACTIONID_SPACE:
					LastFocusedControl->KeyPressed(' ');
					break;

				case ACTIONID_BACKSPACE:
					LastFocusedControl->KeyPressed('\b');
					break;

				/*case ACTIONID_LEFTARROW:
					LastFocusedControl->KeyPressed('');
					break;

				case ACTIONID_RIGHTARROW:
					LastFocusedControl->KeyPressed('');
					break;

				case ACTIONID_UPARROW:
					LastFocusedControl->KeyPressed('');
					break;

				case ACTIONID_DOWNARROW:
					LastFocusedControl->KeyPressed('');
					break;*/
			}
		}
	}

	/************************************************************************/
	/*                          KEYPRESS EVENTS                             */
	/************************************************************************/
}

ZEUIControl* ZEUIManager::FindEventReciever(ZEUIControl* ParentControl)
{
	for (int I = 0; I < ParentControl->GetChildControls().GetCount(); I++)
	{
		ZEUIControl* CurrentControl = ParentControl->GetChildControls()[I];
		if (ZERectangle::BoundingTest(CurrentControl->GetVisibleRectangle(), Cursor->GetPosition()))
		{
			if (CurrentControl->GetChildControls().GetCount() != 0)
			{
				return FindEventReciever(CurrentControl);
			}

			else 
				return CurrentControl;
		}
	}

	return ParentControl;
}

void ZEUIManager::Render(ZERenderer* Renderer)
{
	UIRenderer->Clean();
	for (size_t I = 0; I < Controls.GetCount(); I++)
		if (Controls[I]->GetVisiblity())
			Controls[I]->Draw(UIRenderer);

	UIRenderer->Render(Renderer);
}

void ZEUIManager::Tick(float ElapsedTime)
{
	for (int I = 0; I < Controls.GetCount(); I++)
		if (Controls[I]->GetEnabled())
			Controls[I]->Tick(ElapsedTime);

	ProcessEvents();
}

void ZEUIManager::Destroy()
{
	delete this;
}

ZEUIManager* ZEUIManager::CreateInstance()
{
	return new ZEUIManager();
}




