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

ZEUIManager::ZEUIManager() 
{
	UIRenderer = NULL;
	OldMousePosition = ZEVector2::Zero;
	LastHoveredControl = NULL;
	LastPressedControl = NULL;
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


	// Test Routines

	ZEUITextControl* TestControl = new ZEUITextControl();

	TestControl->SetPosition(ZEVector2(20, 20));
	TestControl->SetSize(ZEVector2(780.0f, 100.0f));
	TestControl->SetBackgroundColor(ZEVector4(1.0f, 0.0f, 0.0f, 1.0f));
	TestControl->SetTextColor(ZEVector4(1.0f, 1.0f, 1.0f, 1.0f));
	TestControl->SetText("Kan kokuyorum ulan !");
	TestControl->SetTextWrap(true);
	TestControl->SetFontSize(ZEVector2::One);
	TestControl->SetFont(ZEFontResource::LoadResource("OldEnglish.zeFont"));

	PressedButton = ZEUIMouseKey::ZE_UI_MOUSE_BUTTON_NONE;

	AddControl(TestControl);

	return true;
}

void ZEUIManager::Deinitialize()
{
	UIRenderer->Destroy();
	UIRenderer = NULL;
}

void ZEUIManager::ProcessEvents()
{
	ZEVector2 CursorPosition = Cursor->GetPosition();

	/************************************************************************/
	/*                          HOVER ENTER AND LEAVE                       */
	/************************************************************************/
	if (OldMousePosition != CursorPosition)
	{
		for (int I = 0; I < Controls.GetCount(); I++)
		{
			if (LastHoveredControl != NULL && ZERectangle::BoundingTest(LastHoveredControl->GetVisibleRectangle(), CursorPosition) == false)
			{
				LastHoveredControl->MouseLeaveEvent(CursorPosition);
			}

			ZEUIControl* Reciever = NULL;

			if (Controls[I] == Cursor)
				continue;

			for (int I = 0; I < Controls.GetCount(); I++)
			{
				if (ZERectangle::BoundingTest(Controls[I]->GetVisibleRectangle(), CursorPosition))
				{
					if (Controls[I]->GetChildControls().GetCount() == 0)
					{
						Reciever = Controls[I];
					}

					else
						Reciever = FindEventReciever(Controls[I]);
				}
			}

			if (Reciever != NULL && Reciever != LastHoveredControl)
			{
				Reciever->MouseEnterEvent(CursorPosition);
				LastHoveredControl = Reciever;
			}
		}
	}
	/************************************************************************/
	/*                          HOVER ENTER AND LEAVE                       */
	/************************************************************************/

	/************************************************************************/
	/*                         MOUSE PRESS AND RELEASE EVENT                */
	/************************************************************************/

	if (Cursor->GetCurrentButton() != ZEUIMouseKey::ZE_UI_MOUSE_BUTTON_NONE)
	{
		LastHoveredControl->MouseButtonPressed(Cursor->GetCurrentButton(), CursorPosition);
		LastPressedControl = LastHoveredControl;
	}

	if (LastPressedControl != NULL && Cursor->GetCurrentButton() == ZEUIMouseKey::ZE_UI_MOUSE_BUTTON_NONE)
	{
		LastPressedControl->MouseButtonReleased(PressedButton, CursorPosition);

		if (ZERectangle::BoundingTest(LastPressedControl->GetVisibleRectangle(), CursorPosition))
		{
			LastPressedControl->MouseEnterEvent(CursorPosition);
			LastHoveredControl = LastPressedControl;
		}

		LastPressedControl = NULL;
	}

	/************************************************************************/
	/*                         MOUSE PRESS RELEASE EVENT                    */
	/************************************************************************/

	OldMousePosition = CursorPosition;
}

ZEUIControl* ZEUIManager::FindEventReciever(ZEUIControl* ParentControl)
{
	for (int I = 0; I < ParentControl->GetChildControls().GetCount(); I++)
	{
		if (ZERectangle::BoundingTest(ParentControl->GetVisibleRectangle(), Cursor->GetPosition()))
		{
			if (ParentControl->GetChildControls().GetItem(I)->GetChildControls().GetCount() == 0)
			{
				return ParentControl->GetChildControls()[I];
			}

			else
				FindEventReciever(ParentControl->GetChildControls()[I]);
		}

		else 
			return  NULL;
	}
}

void ZEUIManager::Render(ZERenderer* Renderer)
{
	UIRenderer->Clean();
	for (size_t I = 0; I < Controls.GetCount(); I++)
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




