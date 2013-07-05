//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUICursorControl.cpp
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

#include "ZEUICursorControl.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "zeui/ZEUIRenderer.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZETexture/ZETexture2DResource.h"

#define ACTIONID_UP				0
#define ACTIONID_DOWN			1
#define ACTIONID_LEFT			2
#define ACTIONID_RIGHT			3
#define ACTIONID_LEFT_PRESS		4
#define ACTIONID_RIGHT_PRESS	5
#define ACTIONID_MIDDLE_PRESS	6
#define ACTIONID_LEFT_RELEASE	7
#define ACTIONID_RIGHT_RELEASE	8
#define ACTIONID_MIDDLE_RELEASE	9
#define ACTIONID_SCROLL_UP		10
#define ACTIONID_SCROLL_DOWN	11


void ZEUICursorControl::Draw(ZEUIRenderer* Renderer)
{
	if (GetVisiblity() == false)
		return;

	ZEUIControl::Draw(Renderer);
	Cursor.ZOrder = GetZOrder();
	Renderer->AddRectangle(Cursor);
}

void ZEUICursorControl::Tick(float ElapsedTime)
{
	InputMap.Update();

	ZESize InputActionCount = InputMap.GetActionCount();
	const ZEInputAction* InputActions = InputMap.GetActions();
	for (size_t I = 0; I < InputActionCount; I++)
	{
		switch (InputActions[I].Id)
		{
			case ACTIONID_UP:
				SetPosition(ZEVector2(GetPosition().x, GetPosition().y - InputActions[I].AxisValue));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_DOWN:
				SetPosition(ZEVector2(GetPosition().x, GetPosition().y + InputActions[I].AxisValue));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_LEFT:
				SetPosition(ZEVector2(GetPosition().x - InputActions[I].AxisValue, GetPosition().y));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_RIGHT:
				SetPosition(ZEVector2(GetPosition().x + InputActions[I].AxisValue, GetPosition().y));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_LEFT_PRESS:
				CurentButton = ZE_UI_MOUSE_BUTTON_LEFT;
				break;

			case ACTIONID_RIGHT_PRESS:
				CurentButton = ZE_UI_MOUSE_BUTTON_RIGHT;
				break;

			case ACTIONID_MIDDLE_PRESS:
				CurentButton = ZE_UI_MOUSE_BUTTON_MIDDLE;
				break;

			case ACTIONID_LEFT_RELEASE:
				CurentButton = ZE_UI_MOUSE_BUTTON_NONE;
				break;

			case ACTIONID_RIGHT_RELEASE:
				CurentButton = ZE_UI_MOUSE_BUTTON_NONE;
				break;

			case ACTIONID_MIDDLE_RELEASE:
				CurentButton = ZE_UI_MOUSE_BUTTON_NONE;
				break;

			case ACTIONID_SCROLL_UP:
				break;

			case ACTIONID_SCROLL_DOWN:
				break;
		}
	}

	ZEInt32 WindowWidth, WindowHeight;
	zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);

	if (GetPosition().x > WindowWidth)
	{
		SetPosition(ZEVector2((float)WindowWidth, GetPosition().y));
	}

	else if (GetPosition().x < 0)
	{
		SetPosition(ZEVector2(0.0f, GetPosition().y));
	}

	if (GetPosition().y > WindowHeight)
	{
		SetPosition(ZEVector2(GetPosition().x, (float)WindowHeight));
	}

	else if (GetPosition().y < 0)
	{
		SetPosition(ZEVector2(GetPosition().x, 0));
	}
}

ZEUIMouseKey ZEUICursorControl::GetCurrentButton()
{
	return CurentButton;
}

void ZEUICursorControl::SetZOrder(ZEInt32 Z)
{
	ZEUIControl::SetZOrder(Z);
	Cursor.ZOrder = Z;
}

ZEMaterial* ZEUICursorControl::GetMaterial() const
{
	return CursorMaterial;
}

void ZEUICursorControl::SetMaterial(ZEMaterial* Material)
{
	Cursor.Material = (ZEMaterialUserInterface*)Material;
}

ZEUICursorControl::ZEUICursorControl()
{
	InputMap.AddAxisAction("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE, ZE_IS_CHANGED, ACTIONID_UP);
	InputMap.AddAxisAction("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE, ZE_IS_CHANGED, ACTIONID_DOWN);
	InputMap.AddAxisAction("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE, ZE_IS_CHANGED, ACTIONID_LEFT);
	InputMap.AddAxisAction("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE, ZE_IS_CHANGED, ACTIONID_RIGHT);

	InputMap.AddButtonAction("Mouse", ZE_IMB_BUTTON0, ZE_IS_PRESSED, ACTIONID_LEFT_PRESS);
	InputMap.AddButtonAction("Mouse", ZE_IMB_BUTTON1, ZE_IS_PRESSED, ACTIONID_RIGHT_PRESS);
	InputMap.AddButtonAction("Mouse", ZE_IMB_BUTTON2, ZE_IS_PRESSED, ACTIONID_MIDDLE_PRESS);												  
	InputMap.AddButtonAction("Mouse", ZE_IMB_BUTTON0, ZE_IS_RELEASED, ACTIONID_LEFT_RELEASE);
	InputMap.AddButtonAction("Mouse", ZE_IMB_BUTTON1, ZE_IS_RELEASED, ACTIONID_RIGHT_RELEASE);
	InputMap.AddButtonAction("Mouse", ZE_IMB_BUTTON2, ZE_IS_RELEASED, ACTIONID_MIDDLE_RELEASE);

	SetHeight(24);
	SetWidth(24);	

	ZEInt32 Width, Height;
	zeCore->GetWindow()->GetWindowSize(Width, Height);
	SetPosition(ZEVector2((Width / 2.0f) , (Height / 2.0f)));

	Cursor.Positions.LeftUp = GetVisibleRectangle().LeftUp;
	Cursor.Positions.RightDown = GetVisibleRectangle().RightDown;

	Cursor.Texcoords.LeftUp = ZEVector2::Zero;
	Cursor.Texcoords.RightDown = ZEVector2::One;

	CursorMaterial = ZEMaterialUserInterface::CreateInstance();
	CursorMaterial->SetTexture(ZETexture2DResource::LoadResource("ZEEngine/ZEGUI/Textures/Cursor.png")->GetTexture());
	Cursor.Material = CursorMaterial;
	Cursor.Color = GetBackgroundColor();

	SetEnabled(true);
	SetFocusable(false);

	CurentButton = ZE_UI_MOUSE_BUTTON_NONE;
	
}

ZEUICursorControl::~ZEUICursorControl()
{

}
