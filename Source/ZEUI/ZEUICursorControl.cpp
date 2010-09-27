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
#include "ZEMath/ZEVector.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "zeui/ZEUIRenderer.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZEGraphics/ZETexture2DResource.h"

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
	ZEUIControl::Draw(Renderer);
	Renderer->AddRectangle(Cursor);
}

void ZEUICursorControl::Tick(float ElapsedTime)
{
	zeInput->ProcessInputMap(&InputMap);

	for (size_t I = 0; I < InputMap.InputActionCount; I++)
	{
		switch (InputMap.InputActions[I].Id)
		{
			case ACTIONID_UP:
				SetPosition(ZEVector2(GetPosition().x, GetPosition().y - InputMap.InputActions[I].AxisValue));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_DOWN:
				SetPosition(ZEVector2(GetPosition().x, GetPosition().y + InputMap.InputActions[I].AxisValue));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_LEFT:
				SetPosition(ZEVector2(GetPosition().x - InputMap.InputActions[I].AxisValue, GetPosition().y));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_RIGHT:
				SetPosition(ZEVector2(GetPosition().x + InputMap.InputActions[I].AxisValue, GetPosition().y));
				Cursor.Positions.LeftUp = GetRectangle().LeftUp;
				Cursor.Positions.RightDown = GetRectangle().RightDown;
				break;

			case ACTIONID_LEFT_PRESS:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3::UnitX);
				break;

			case ACTIONID_RIGHT_PRESS:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3::UnitY);
				break;

			case ACTIONID_MIDDLE_PRESS:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3::UnitZ);
				break;

			case ACTIONID_LEFT_RELEASE:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3::One);
				break;

			case ACTIONID_RIGHT_RELEASE:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3::One);
				break;

			case ACTIONID_MIDDLE_RELEASE:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3::One);
				break;

			case ACTIONID_SCROLL_UP:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3(1.0f, 1.0f, 0.0f));
				break;

			case ACTIONID_SCROLL_DOWN:
				((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3(0.0f, 1.0f, 1.0f));
				break;
		}
	}

	int WindowWidth, WindowHeight;
	zeCore->GetWindow()->GetWindowSize(WindowWidth, WindowHeight);

	if (GetPosition().x > WindowWidth)
	{
		SetPosition(ZEVector2(WindowWidth, GetPosition().y));
	}

	else if (GetPosition().x < 0)
	{
		SetPosition(ZEVector2(0, GetPosition().y));
	}

	if (GetPosition().y > WindowHeight)
	{
		SetPosition(ZEVector2(GetPosition().x, WindowHeight));
	}

	else if (GetPosition().y < 0)
	{
		SetPosition(ZEVector2(GetPosition().x, 0));
	}
}

ZEUICursorControl::ZEUICursorControl()
{
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_UP,				"Move Up",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS,		ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_DOWN,			"Move Down",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS,		ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_LEFT,			"Move Left",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS,	ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_RIGHT,			"Move Right",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS,	ZE_IAS_POSITIVE)));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_LEFT_PRESS,		"Left Press",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_BUTTON0,			ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_RIGHT_PRESS,		"Right Press",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_BUTTON1,			ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_MIDDLE_PRESS,	"Middle Press",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_BUTTON2,			ZE_IBS_PRESSED)));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_LEFT_RELEASE,	"Left Release",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_BUTTON0,			ZE_IBS_RELEASED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_RIGHT_RELEASE,	"Right Release",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_BUTTON1,			ZE_IBS_RELEASED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_MIDDLE_RELEASE,	"Middle Release",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_BUTTON2,			ZE_IBS_RELEASED)));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_SCROLL_UP,		"Scroll Up",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS,		ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_SCROLL_DOWN,		"Scroll Down",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS,		ZE_IAS_NEGATIVE)));

	SetHeight(16);
	SetWidth(16);	

	int Width, Height;
	zeCore->GetWindow()->GetWindowSize(Width, Height);
	SetPosition(ZEVector2((Width / 2) , (Height / 2)));

	Cursor.Positions.LeftUp = GetVisibleRectangle().LeftUp;
	Cursor.Positions.RightDown = GetVisibleRectangle().RightDown;

	Cursor.Texcoords.LeftUp = ZEVector2::Zero;
	Cursor.Texcoords.RightDown = ZEVector2::One;

	Cursor.Material = ZEFixedMaterial::CreateInstance();

	((ZEFixedMaterial*)(Cursor.Material))->SetZero();
	((ZEFixedMaterial*)(Cursor.Material))->SetAmbientEnabled(true);
	((ZEFixedMaterial*)(Cursor.Material))->SetAmbientColor(ZEVector3::One);
	((ZEFixedMaterial*)(Cursor.Material))->SetAmbientFactor(1.0f);
	//((ZEFixedMaterial*)(Cursor.Material))->SetDiffuseEnabled(true);
	//((ZEFixedMaterial*)(Cursor.Material))->SetDiffuseMap(ZETexture2DResource::LoadResource("ZECursor.png")->GetTexture());
	((ZEFixedMaterial*)(Cursor.Material))->UpdateMaterial();

	SetZOrder(1000);


	SetEnabled(true);
	
}

ZEUICursorControl::~ZEUICursorControl()
{

}
