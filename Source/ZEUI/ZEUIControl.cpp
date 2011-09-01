//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIControl.cpp
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

#include "ZEUIControl.h"
#include "ZEUIRenderer.h"

void ZEUIControl::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEUIControl::GetName()
{
	return Name;
}

void ZEUIControl::SetToolTip(const ZEString& ToolTip)
{
	this->ToolTip = ToolTip;
}

const ZEString& ZEUIControl::GetToolTip()
{
	return ToolTip;
}

void ZEUIControl::SetVisiblity(bool Visiblity)
{
	IsVisible = Visiblity;
}

bool ZEUIControl::GetVisiblity() const
{
	return IsVisible;
}

void ZEUIControl::SetEnabled(bool Enabled)
{
	IsEnabled = Enabled;
}

bool ZEUIControl::GetEnabled() const
{
	return IsEnabled;
}

void ZEUIControl::SetModal(bool Modal)
{
	IsModal = Modal;
}

bool ZEUIControl::GetModal() const
{
	return IsModal;
}

void ZEUIControl::SetFocusable(bool Focusable)
{
	IsFocusable = Focusable;
}

bool ZEUIControl::GetFocusable()
{
	return IsFocusable;
}

void ZEUIControl::SetMoveable(bool Moveable)
{
	IsMoveable = Moveable;
}

bool ZEUIControl::GetMoveable() const
{
	return IsMoveable;
}

void ZEUIControl::SetFixedSized(bool FixedSized)
{
	IsFixedSized = FixedSized;
}

bool ZEUIControl::GetFixedSized() const
{
	return IsFixedSized;
}

void ZEUIControl::SetHovered(bool Hovered)
{
	IsHovered = Hovered;
}

bool ZEUIControl::GetHovered() const
{
	return IsHovered;
}

void ZEUIControl::SetFocused(bool Focused)
{
	IsFocused = Focused;
}

bool ZEUIControl::GetFocused() const
{
	return IsFocused;
}

void ZEUIControl::SetPressed(bool Pressed)
{
	IsPressed = Pressed;
}

bool ZEUIControl::GetPressed() const
{
	return IsPressed;
}

void ZEUIControl::SetZOrder(int Z)
{
	ZOrder = Z;
}

int ZEUIControl::GetZOrder() const
{
	return ZOrder;
}

void ZEUIControl::SetParent(ZEUIControl* Parent)
{
	if (ParentControl != NULL)
		ParentControl->RemoveChildControl(this);
	
	this->ParentControl = Parent; 
	//ParentControl->AddChildControl(this); !!! REVISION
}

const ZEArray<ZEUIControl*>& ZEUIControl::GetChildControls()
{
	return ChildControls;
}

void ZEUIControl::AddChildControl(ZEUIControl* Control)
{
	ChildControls.Add(Control);
	Control->SetParent(this);
}

void ZEUIControl::RemoveChildControl(ZEUIControl* Control)
{
	ChildControls.DeleteValue(Control);
	Control->SetParent(NULL);
}

ZEUIControl* ZEUIControl::GetParentControl() const
{
	return ParentControl;
}

void ZEUIControl::SetPosition(const ZEVector2& Position)
{
	DirtyVisibleRectangle = true;

	ZEVector2 PositionDifference = Position - GetPosition();

	for (int I = 0; I < GetChildControls().GetCount(); I++)
		GetChildControls()[I]->SetPosition(GetChildControls()[I]->GetPosition() + PositionDifference);

	Rectangle.SetPosition(Position);
}

const ZEVector2& ZEUIControl::GetPosition()
{
	return Rectangle.GetPosition();
}

void ZEUIControl::SetSize(const ZEVector2& Size)
{
	if (IsFixedSized)
		return;

	DirtyVisibleRectangle = true;

	if (Size.x <= MaximumSize.x && Size.x >= MinimumSize.x)
		SetWidth(Size.x);

	if (Size.y <= MaximumSize.y && Size.y >= MinimumSize.y)
		SetHeight(Size.y);

}

ZEVector2 ZEUIControl::GetSize()
{
	return Rectangle.GetSize();
}

void ZEUIControl::SetMinimumSize(ZEVector2 MinimumSize)
{
	this->MinimumSize = MinimumSize;
}

ZEVector2 ZEUIControl::GetMinimumSize() const
{
	return MinimumSize;
}

void ZEUIControl::SetMaximumSize(ZEVector2 MaximumSize)
{
	this->MaximumSize = MaximumSize;
}

ZEVector2 ZEUIControl::GetMaximumSize() const
{
	return MaximumSize;
}

void ZEUIControl::SetWidth(float Width)
{
	if (IsFixedSized)
		return;

	DirtyVisibleRectangle = true;
	
	if (Width <= MaximumSize.x && Width >= MinimumSize.x)
		Rectangle.SetWidth(Width);

}

float ZEUIControl::GetWidth()
{
	return Rectangle.GetWidth();
}

void ZEUIControl::SetHeight(float Height)
{
	if (IsFixedSized)
		return;

	DirtyVisibleRectangle = true;

	if (Height <= MaximumSize.x && Height >= MinimumSize.x)
		Rectangle.SetHeight(Height);
}

float ZEUIControl::GetHeight()
{
	return Rectangle.GetHeight();
}

const ZERectangle& ZEUIControl::GetRectangle()
{
	return Rectangle;
}

const ZERectangle& ZEUIControl::GetVisibleRectangle()
{
	if (ParentControl == NULL)
		return Rectangle;

	//if(DirtyVisibleRectangle)
	//{
		ZERectangle::Intersection(VisibleRectangle, Rectangle, ParentControl->GetVisibleRectangle());
		DirtyVisibleRectangle = false;
	//}

	return VisibleRectangle;
}

void ZEUIControl::SetBackgroundType(ZEUIBackgroundType Type)
{
	BackgroundType = Type;
}

ZEUIBackgroundType ZEUIControl::GetBackgroundType()
{
	return BackgroundType;
}

void ZEUIControl::SetBackgroundColor(const ZEVector4& Color)
{
	BackgroundColor = Color;
}

const ZEVector4& ZEUIControl::GetBackgroundColor()
{
	return BackgroundColor;
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
	IsPressed = true;

	if (MouseButtonPressedEvent != NULL)
		MouseButtonPressedEvent(Button, MousePosition);
}

void ZEUIControl::MouseButtonReleased(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if (IsPressed)
		MouseClickEvent(Button, MousePosition);

	IsPressed = false;

	if (MouseButtonReleasedEvent != NULL)
		MouseButtonReleasedEvent(Button, MousePosition);
}

void ZEUIControl::MouseClickEvent(ZEUIMouseKey Button, const ZEVector2& MousePosition)
{
	if (!IsHovered)
		return;

	if (MouseClickedEvent != NULL)
		MouseClickedEvent(Button, MousePosition);
}

void ZEUIControl::MouseHovered(const ZEVector2& MousePosition)
{
	if (MouseHoveredEvent != NULL)
		MouseHoveredEvent(MousePosition);
}

void ZEUIControl::MouseEnterEvent(const ZEVector2& MousePosition)
{
	IsHovered = true;

	if (MouseEnteredEvent != NULL)
		MouseEnteredEvent(MousePosition);
}

void ZEUIControl::MouseLeaveEvent(const ZEVector2& MousePosition)
{
	IsHovered = false;

	if (MouseLeftEvent != NULL)
		MouseLeftEvent(MousePosition);
}

void ZEUIControl::MouseMoveEvent(const ZEVector2& MoveAmount)
{
	if (MouseMovedEvent != NULL)
		MouseMovedEvent(MoveAmount);
}

void ZEUIControl::FocusLost()
{
	if (!IsFocusable)
		return;

	IsFocused = false;

	if (FocusLostEvent != NULL)
		FocusLostEvent();
}

void ZEUIControl::FocusGained()
{
	if (!IsFocusable)
		return;

	IsFocused = true;

	if (FocusGainedEvent != NULL)
		FocusGainedEvent();
}

void ZEUIControl::Draw(ZEUIRenderer* Renderer)
{
	if (!IsVisible)
		return;

	for (size_t I = 0; I < ChildControls.GetCount(); I++)
		ChildControls[I]->Draw(Renderer);
}

void ZEUIControl::Tick(float ElapsedTime)
{
	if (!IsEnabled)
		return;

	for (int I = 0; I < ChildControls.GetCount(); I++)
			ChildControls[I]->Tick(ElapsedTime);
}

ZEUIControl::ZEUIControl()
{
	ParentControl	= NULL;
	BackgroundType	= ZE_UI_BT_SOLID;

	BackgroundColor = ZEVector4::One;
	HoverColor		= ZEVector4::UnitX;
	PressedColor	= ZEVector4::UnitY;
	DisabledColor	= ZEVector4(0.2f, 0.2f, 0.2f, 1.0f);

	BackgroundTexture = NULL;

	Rectangle.LeftUp	= ZEVector2::Zero;
	Rectangle.RightDown = ZEVector2::One;

	IsFocusable		= false;
	IsMoveable		= false;
	IsFixedSized	= false;
	IsModal			= false;
	IsVisible		= true;
	IsEnabled		= true;
	IsHovered		= false;
	IsFocused		= false;
	IsPressed		= false; 

	MinimumSize = ZEVector2::Zero;
	MaximumSize = ZEVector2(1000000, 1000000);
}

ZEUIControl::~ZEUIControl()
{

}
