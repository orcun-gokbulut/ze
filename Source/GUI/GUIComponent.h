//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIComponent.h
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

#ifndef __ZE_GUI_COMPONENT__
#define __ZE_GUI_COMPONENT__

#include "Common/ZEDS/ZEDS.h"
#include "fastdelegate.h"

enum ZEGUIMouseKey
{
	ZEGUIMOUSEKEY_LEFT,
	ZEGUIMOUSEKEY_RIGHT,
	ZEGUIMOUSEKEY_MIDDLE
};

typedef fastdelegate::FastDelegate(const ZEVector2&, void) ZEGUIEventOnMouseClick;
typedef fastdelegate::FastDelegate1(const ZEVector2&, void) ZEGUIEventOnMouseDoubleClick;
typedef fastdelegate::FastDelegate2(ZEMouseKey, const ZEVector2, void) ZEGUIEventOnMouseKeyDown;
typedef fastdelegate::FastDelegate2(ZEMouseKey, const ZEVector2, void) ZEGUIEventOnMouseKeyUp;
typedef fastdelegate::FastDelegate2(ZEMouseKey, const ZEVector2, void) ZEGUIEventOnMouseHover;
typedef fastdelegate::FastDelegate1(char Key, void) ZEGUIEventOnKeyPressed;

class ZEGUIEngine;
class ZEGUIComponent
{
	private:
		ZERectangle						Rectangle;
		int								Z;
		bool							HoverState;
		bool							FocusState;
		bool							VisibleState;

		ZEGUIEventOnMouseClick			OnMouseClickEvent;
		ZEGUIEventOnMouseDoubleClick	OnMouseDoubleClickEvent;
		ZEGUIEventOnMouseKeyDown		OnMouseKeyDownEvent;
		ZEGUIEventOnMouseKeyUp			OnMouseKeyUpEvent;
		ZEGUIEventOnMouseHover			OnMouseHoverEvent;
		ZEGUIEventOnKeyPressed			OnKeyPressedEvent;

	protected:
		ZEGUIComponent*					ParentComponent;
		ZEArray<ZEGUIComponent*>		ChildComponents;

		virtual void					KeyPressed(unsigned char Key);
		virtual void					MouseClicked(int Button, const ZEVector2& MousePosition);
		virtual void					MouseHovered(const ZEVector2& MousePosition);
		virtual void					MouseLeaved();

		ZEGUIComponent*					GetFocusedComponent();
		ZEGUIComponent*					GetChildComponent(const ZEVector2& MousePosition, const ZEVector2& ParentPosition);

	public:
		void							SetRectangle(const ZERectangle& Rectangle);
		const ZERectangle&				GetRectangle();

		int								GetZ();
		void							SetZ(int Z);

		void							SetParent(ZEGUIWindow *ParentName);
		ZEGUIWindow*					GetParent();
		
		void							SetVisiblity(bool State);
		bool							IsVisible();

		void							SetOnMouseClickEvent(ZEGUIEventOnMouseClick Event);
		void							SetOnMouseDoubleClickEvent(ZEGUIEventOnMouseDoubleClick Event);
		void							SetOnMouseKeyDownEvent(ZEGUIEventOnMouseKeyDown Event);
		void							SetOnMouseKeyUpEvent(ZEGUIEventOnMouseKeyUp Event);
		void							SetOnMouseHoverEvent(ZEGUIEventOnMouseHover Event);
		void							SetOnKeyPressedEvent(ZEGUIEventOnKeyPressed Event);

		virtual void					Draw() = 0;



		/*virtual bool					MouseMotionEvent(float Mx,float My);
		virtual bool					MouseActionEvent(unsigned char ActionType);
		virtual bool					KeyboardEvent(unsigned char Key, bool ActionType);
		virtual void					HoverExited()	{ HoverState = 0; }
		virtual void					FocusExited()	{ FocusState = 0; }*/

										ZEGUIComponent();
										~ZEGUIComponent();
};


#endif
