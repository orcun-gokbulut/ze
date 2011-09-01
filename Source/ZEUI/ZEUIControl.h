//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIControl.h
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

#pragma once
#ifndef __ZE_UI_COMPONENT__
#define __ZE_UI_COMPONENT__

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEUIRectangle.h"
#include "ZEUIEvents.h"
#include "ZEFastDelegate.h"

enum ZEUIControlVisibility
{
	ZE_IUCV_NOT_VISIBLE		= 0,
	ZE_IUCV_SEMI_VISIBLE	= 1,
	ZE_IUCV_FULLY_VISIBLE	= 2,
};

enum ZEUIBackgroundType
{
	ZE_UI_BT_NONE			= 1,
	ZE_UI_BT_SOLID			= 0,
};

class ZEUIRenderer;
class ZETexture2D;
class ZEUIControl
{
	friend class ZEUIManager;

	private:
		ZEString						Name;
		ZEString						ToolTip;

		ZEUIControl*					ParentControl;
		ZEArray<ZEUIControl*>			ChildControls;

		ZERectangle						Rectangle;
		ZERectangle						VisibleRectangle;
	
		ZEVector4						BackgroundColor;
		ZEVector4						HoverColor;
		ZEVector4						PressedColor;
		ZEVector4						DisabledColor;

		ZETexture2D*					BackgroundTexture;

		ZEUIBackgroundType				BackgroundType;

		bool							IsFocusable;
		bool							IsMoveable;
		bool							IsFixedSized;
		bool							IsModal;
		bool							IsVisible;
		bool							IsEnabled;
		bool							IsHovered;
		bool							IsFocused;
		bool							IsPressed;       
		
		int								ZOrder;

		ZEVector2						MaximumSize;
		ZEVector2						MinimumSize;

		ZEUIEventMouseClicked			MouseClickedEvent;			//Done
		ZEUIEventMouseDoubleClicked		MouseDoubleClickedEvent;
		ZEUIEventMouseButtonPressed		MouseButtonPressedEvent;	//Done
		ZEUIEventMouseButtonReleased	MouseButtonReleasedEvent;	//Done
		ZEUIEventMouseHovered			MouseHoveredEvent;			//Done
		ZEUIEventMouseEntered			MouseEnteredEvent;			//Done
		ZEUIEventMouseLeft				MouseLeftEvent;				//Done
		ZEUIEventMouseMoved				MouseMovedEvent;			//Done
		ZEUIEventKeyPressed				KeyPressedEvent;			//Done
		ZEUIEventKeyReleased			KeyReleasedEvent;			//Done
		ZEUIEventFocusGained			FocusGainedEvent;			//Done
		ZEUIEventFocusLost				FocusLostEvent;				//Done

	protected:

		bool							DirtyVisibleRectangle;
		void							SetParent(ZEUIControl *ParentName);

		void							SetHovered(bool Hovered);
		void							SetFocused(bool Focused);
		void							SetPressed(bool Pressed);							

		ZEUIControl*					FindFocusedComponent();
		ZEUIControl*					FindChildComponent(const ZEVector2& Position);

		virtual void					KeyPressed(unsigned char Key);
		virtual void					KeyReleased(unsigned char Key);

		virtual void					MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition);
		virtual void					MouseButtonReleased(ZEUIMouseKey Button, const ZEVector2& MousePosition);
		virtual void					MouseClickEvent(ZEUIMouseKey Button, const ZEVector2& MousePosition);
		virtual void					MouseHovered(const ZEVector2& MousePosition);
		virtual	void					MouseEnterEvent(const ZEVector2& MousePosition);
		virtual	void					MouseLeaveEvent(const ZEVector2& MousePosition);
		virtual void					MouseMoveEvent(const ZEVector2& MoveAmount);
		virtual void					FocusLost();
		virtual void					FocusGained();

	public:

		ZEUIControl*					GetParentControl() const;

		virtual void					AddChildControl(ZEUIControl* Control);
		const ZEArray<ZEUIControl*>&	GetChildControls();			
		void							RemoveChildControl(ZEUIControl* Control);

		void							SetName(const ZEString& Name);
		const ZEString&					GetName();

		void							SetToolTip(const ZEString& ToolTip);
		const ZEString&					GetToolTip();

		virtual void					SetVisiblity(bool Visiblity);
		bool							GetVisiblity() const;

		virtual void					SetEnabled(bool Enabled);
		bool							GetEnabled() const;

		void							SetModal(bool Modal);
		bool							GetModal() const;

		void							SetFocusable(bool Focusable);
		bool							GetFocusable();

		virtual void					SetMoveable(bool Moveable);
		bool							GetMoveable() const;

		void							SetFixedSized(bool FixedSized);
		bool							GetFixedSized() const; 

		bool							GetHovered() const;
		bool							GetFocused() const;
		bool							GetPressed() const;

		void							SetZOrder(int Z);
		int								GetZOrder() const;

		void							SetMinimumSize(ZEVector2 MinimumSize);
		ZEVector2						GetMinimumSize() const;

		void							SetMaximumSize(ZEVector2 MaximumSize);
		ZEVector2						GetMaximumSize() const;

		virtual void					SetPosition(const ZEVector2& Position);
		const ZEVector2&				GetPosition();

		virtual void					SetSize(const ZEVector2& Size);
		ZEVector2						GetSize();

		virtual void					SetWidth(float Width);
		float							GetWidth();

		virtual void					SetHeight(float Height);
		float							GetHeight();

		const ZERectangle&				GetVisibleRectangle();
		const ZERectangle&				GetRectangle();

		void							SetBackgroundColor(const ZEVector4& Color);
		const ZEVector4&				GetBackgroundColor();

		void							SetBackgroundType(ZEUIBackgroundType Type);
		ZEUIBackgroundType				GetBackgroundType();

		void 			 				SetMouseClickedEvent(const ZEUIEventMouseClicked& Event);
		void 				 			SetMouseDoubleClickedEvent(const ZEUIEventMouseDoubleClicked& Event);
		void 							SetMouseButtonPressedEvent(const ZEUIEventMouseButtonPressed& Event);
		void 			 				SetMouseButtonReleasedEvent(const ZEUIEventMouseButtonReleased& Event);
		void 			 				SetMouseEnteredEvent(const ZEUIEventMouseEntered& Event);
		void 			 				SetMouseLeftEvent(const ZEUIEventMouseLeft& Event);
		void 			 				SetMouseMovedEvent(const ZEUIEventMouseMoved& Event);
		void 			 				SetKeyPressedEvent(const ZEUIEventKeyPressed& Event);
		void 			 				SetKeyReleasedEvent(const ZEUIEventKeyReleased& Event);
		void 			 				SetFocusGainedEvent(const ZEUIEventFocusGained& Event);
		void 			 				SetFocusLostEvent(const ZEUIEventFocusLost& Event);

		virtual void					Draw(ZEUIRenderer* Renderer);
		virtual void					Tick(float ElapsedTime);

		virtual ZEMaterial*				GetMaterial() const = 0;
		virtual void					SetMaterial(ZEMaterial* Material) = 0;

										ZEUIControl();
										~ZEUIControl();
};


#endif
