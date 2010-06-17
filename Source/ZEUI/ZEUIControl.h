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

#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEString.h"
#include "ZEMath\ZEVector.h"
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
	private:
		ZEString						Name;

		ZEUIControl*					ParentControl;
		ZEArray<ZEUIControl*>			ChildControls;

		ZERectangle						Rectangle;
		ZERectangle						VisibleRectangle;
	
		ZEVector4						BackgroundColor;
		ZETexture2D*					BackgroundTexture;
		ZEUIBackgroundType				BackgroundType;

		bool							HoverState;
		bool							FocusState;

		bool							Visibility;
		bool							Enabled;
		int								ZOrder;
		bool							DirtyVisibleRectangle;


		ZEUIEventMouseClicked			MouseClickedEvent;
		ZEUIEventMouseDoubleClicked		MouseDoubleClickedEvent;
		ZEUIEventMouseButtonPressed		MouseButtonPressedEvent;
		ZEUIEventMouseButtonReleased	MouseButtonReleasedEvent;
		ZEUIEventMouseEntered			MouseEnteredEvent;
		ZEUIEventMouseLeft				MouseLeftEvent;
		ZEUIEventMouseMoved				MouseMovedEvent;
		ZEUIEventKeyPressed				KeyPressedEvent;
		ZEUIEventKeyReleased			KeyReleasedEvent;
		ZEUIEventFocusGained			FocusGainedEvent;
		ZEUIEventFocusLost				FocusLostEvent;

	protected:
		void							SetParent(ZEUIControl *ParentName);

		const ZEArray<ZEUIControl*>&	GetChildControls();			
		void							AddChildControl(ZEUIControl* Control);
		void							RemoveChildControl(ZEUIControl* Control);

		ZEUIControl*					FindFocusedComponent();
		ZEUIControl*					FindChildComponent(const ZEVector2& Position);

		virtual void					KeyPressed(unsigned char Key);
		virtual void					KeyReleased(unsigned char Key);

		virtual void					MouseButtonPressed(ZEUIMouseKey Button, const ZEVector2& MousePosition);
		virtual void					MouseButtonReleased(ZEUIMouseKey Button, const ZEVector2& MousePosition);
		virtual void					MouseHovered(const ZEVector2& MousePosition);

		virtual void					FocusLost();
		virtual void					FocusGained();

	public:
		virtual bool					IsInactive();
		bool							IsVisible();

		ZEUIControl*					GetParentControl();

		void							SetName(const ZEString& Name);
		const ZEString&					GetName();

		virtual void					SetPosition(const ZEVector2& Position);
		const ZEVector2&				GetPosition();

		virtual void					SetSize(const ZEVector2& Size);
		ZEVector2						GetSize();

		void							SetWidth(float Width);
		float							GetWidth();

		void							SetHeight(float Height);
		float							GetHeight();

		const ZERectangle&				GetVisibleRectangle();
		const ZERectangle&				GetRectangle();

		bool							HasFocus();
		void							GainFocus();

		virtual void					SetZOrder(int Z);
		int								GetZOrder();

		virtual void					SetEnabled(bool Enabled);
		bool							GetEnabled();

		virtual void					SetVisiblity(bool Visible);
		bool							GetVisibilty();

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

										ZEUIControl();
										~ZEUIControl();
};


#endif


