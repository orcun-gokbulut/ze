//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - UIControl.cpp
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

#include "UIControl.h"
#include "UIRenderer.h"

void ZEUIControl::KeyPressed(unsigned char Key)
{
	if (KeyPressedEvent != NULL)
		KeyPressedEvent(Key);
}

void ZEUIControl::KeyReleased(unsigned char Key)
{
	if (KeyReleasedEvent != NULL)
		KeyReleasedEvent(Key);
}

void ZEUIControl::MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if (MouseButtonPressedEvent != NULL)
		MouseButtonPressedEvent(Button, MousePosition);
}

void ZEUIControl::MouseButtonReleased(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if (MouseButtonReleasedEvent != NULL)
		MouseButtonReleasedEvent(Button, MousePosition);
}

void ZEUIControl::MouseHovered(const ZEVector2& MousePosition)
{
	if (HoverState)
	{
		if (MouseMovedEvent != NULL)
			MouseMovedEvent(MousePosition);
	}
	else
		if (MouseEnteredEvent != NULL)
			MouseEnteredEvent(MousePosition);
}

void ZEUIControl::FocusLost()
{
	FocusState = false;
	if (FocusLostEvent != NULL)
		FocusLostEvent();
}

void ZEUIControl::FocusGained()
{
	FocusState = true;
	if (FocusGainedEvent != NULL)
		FocusGainedEvent();
}

void ZEUIControl::SetParent(ZEUIControl* Parent)
{
	this->ParentControl = Parent; 
}

ZEUIControl* ZEUIControl::GetParentControl()
{
	return ParentControl;
}

bool ZEUIControl::IsInactive()
{
	return false;
}

void ZEUIControl::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEUIControl::GetName()
{
	return Name;
}

void ZEUIControl::SetPosition(const ZEVector2& Position)
{
	DirtyVisibleRectangle = true;
	Rectangle.LeftUp = Position;
}

const ZEVector2& ZEUIControl::GetPosition()
{
	return Rectangle.LeftUp;
}

void ZEUIControl::SetSize(const ZEVector2& Size)
{
	DirtyVisibleRectangle = true;
	ZEVector2::Add(Rectangle.RightDown, Rectangle.LeftUp, Size);
}

ZEVector2 ZEUIControl::GetSize()
{
	return Rectangle.RightDown - Rectangle.LeftUp;
}

void ZEUIControl::SetWidth(float Width)
{
	DirtyVisibleRectangle = true;
	Rectangle.RightDown.x = Rectangle.LeftUp.x + Width;
}

float ZEUIControl::GetWidth()
{
	return Rectangle.RightDown.x - Rectangle.LeftUp.x;
}

void ZEUIControl::SetHeight(float Height)
{
	DirtyVisibleRectangle = true;
	Rectangle.RightDown.y = Rectangle.LeftUp.y + Height;
}

float ZEUIControl::GetHeight()
{
	return Rectangle.RightDown.y - Rectangle.LeftUp.y;
}

bool ZEUIControl::IsVisible()
{
	if (ParentControl != NULL)
		return Visibility && ParentControl->IsVisible() && ZERectangle::IntersectionTest(Rectangle, ParentControl->GetVisibleRectangle());
	else
		return Visibility;
}

const ZERectangle& ZEUIControl::GetRectangle()
{
	return Rectangle;
}

const ZERectangle& ZEUIControl::GetVisibleRectangle()
{
	if (ParentControl == NULL)
		return Rectangle;

	if(DirtyVisibleRectangle)
	{
		ZERectangle::Intersection(VisibleRectangle, Rectangle, ParentControl->GetVisibleRectangle());
		DirtyVisibleRectangle = false;
	}

	return VisibleRectangle;
}


void ZEUIControl::SetZOrder(int ZOrder)
{
	this->ZOrder = ZOrder;
}

int ZEUIControl::GetZOrder()
{
	return ZOrder;
}

void ZEUIControl::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEUIControl::GetEnabled()
{
	return Enabled;
}

void ZEUIControl::SetVisiblity(bool Visibility)
{
	this->Visibility = Visibility;
}

bool ZEUIControl::GetVisibilty()
{
	return Visibility;
}

bool ZEUIControl::HasFocus()
{
	return FocusState;
}

void ZEUIControl::GainFocus()
{
	// IMPLAMENT !!!!!
}

void ZEUIControl::SetMouseClickedEvent(const ZEUIEventMouseClicked& Event)
{
	MouseClickedEvent = Event;
}

void ZEUIControl::SetMouseDoubleClickedEvent(const ZEUIEventMouseDoubleClicked& Event)
{
	MouseDoubleClickedEvent = Event;
}

void ZEUIControl::SetMouseButtonPressedEvent(const ZEUIEventMouseButtonPressed& Event)
{
	MouseButtonPressedEvent = Event;
}

void ZEUIControl::SetMouseButtonReleasedEvent(const ZEUIEventMouseButtonReleased& Event)
{
	MouseButtonReleasedEvent = Event;
}

void ZEUIControl::SetMouseEnteredEvent(const ZEUIEventMouseEntered& Event)
{
	MouseEnteredEvent = Event;
}

void ZEUIControl::SetMouseLeftEvent(const ZEUIEventMouseLeft& Event)
{
	MouseLeftEvent = Event;
}

void ZEUIControl::SetMouseMovedEvent(const ZEUIEventMouseMoved& Event)
{
	MouseMovedEvent = Event;
}

void ZEUIControl::SetKeyPressedEvent(const ZEUIEventKeyPressed& Event)
{
	KeyPressedEvent = Event;
}

void ZEUIControl::SetKeyReleasedEvent(const ZEUIEventKeyReleased& Event)
{
	KeyReleasedEvent = Event;
}

void ZEUIControl::SetFocusGainedEvent(const ZEUIEventFocusGained& Event)
{
	FocusGainedEvent = Event;
}

void ZEUIControl::SetFocusLostEvent(const ZEUIEventFocusLost& Event)
{
	FocusLostEvent = Event;
}

void ZEUIControl::Draw(ZEUIRenderer* Renderer)
{
	if (!IsVisible())
		return;

	if (ParentControl != NULL)
	{
		ZEUIRectangle UIRectangle, Output;
		UIRectangle.Positions = Rectangle;
		UIRectangle.Texcoords = ZERectangle(ZEVector2(0.0f, 0.0f), ZEVector2(1.0f, 1.0f));

		if (ZEUIRectangle::Clip(Output, UIRectangle, ParentControl->GetVisibleRectangle()))
		{
			Output.Material = NULL;
			Output.ZOrder = ZOrder;
			Output.Color = ZEVector4(1.0f, 1.0f, 1.0f, 1.0f);
			Renderer->AddRectangle(Output);
		}
	}
	else
	{
		ZEUIRectangle Output;
		Output.Material = NULL;
		Output.Positions = Rectangle;
		Output.Texcoords = ZERectangle(ZEVector2(0.0f, 0.0f), ZEVector2(1.0f, 1.0f));
		Output.ZOrder = ZOrder;
		Renderer->AddRectangle(Output);
	}

	// Draw Self
	for (size_t I = 0; I < ChildControls.GetCount(); I++)
		if (ChildControls[I]->IsVisible())
			ChildControls[I]->Draw(Renderer);
}

ZEUIControl::ZEUIControl()
{
	ParentControl = NULL;
	HoverState = false;
	FocusState = false;
}

ZEUIControl::~ZEUIControl()
{
}
