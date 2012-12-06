//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIScrollArea.h
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
#ifndef __ZE_UI_SCROLL_AREA__
#define __ZE_UI_SCROLL_AREA__

#include "ZEUI/ZEUIFrameControl.h"
#include "ZEUI/ZEUIVerticalScrollBar.h"
#include "ZEUI/ZEUIHorizontalScrollbar.h"

enum ScrollbarPolicy
{
	SCROLLBAR_ALWAYS_ON,
	SCROLLBAR_ALWAYS_OFF,
	SCROLLBAR_AS_NEEDED
};

class ZEUIRenderer;
class ZEUIScrollArea : public ZEUIControl
{
	friend class ZEUIHorizontalScrollbar;
	friend class ZEUIVerticalScrollbar;
	private:

		ZEUIFrameControl				VisibleContentArea;
		ZEUIFrameControl				ContentArea;

		ZEUIVerticalScrollbar*			VerticalScrollbar;
		ZEUIHorizontalScrollbar*		HorizontalScrollbar;

		ScrollbarPolicy					HorizontalScrollbarPolicy;
		ScrollbarPolicy					VerticalScrollbarPolicy;

		void							MoveContent(const ZEVector2& MoveAmount);
		void							Update();		

	protected:
		

	public:

		void							SetHorizontalScrollbarPolicy(ScrollbarPolicy Policy);
		ScrollbarPolicy					GetHorizontalScrollbarPolicy();

		void							SetVerticalScrollbarPolicy(ScrollbarPolicy Policy);
		ScrollbarPolicy					GetVerticalScrollbarPolicy();

		ZEUIVerticalScrollbar*			GetVerticalScrollbar();
		ZEUIHorizontalScrollbar*		GetHorizontalScrollbar();

		virtual void					AddChildControl(ZEUIControl* Control);

		ZEUIFrameControl*				GetVisibleContentArea();
		ZEUIFrameControl*				GetContentArea();

		virtual void					SetSize(const ZEVector2& Size);

		virtual void					SetWidth(float Width);
		virtual void					SetHeight(float Height);

		virtual ZEMaterial*				GetMaterial() const;
		virtual void					SetMaterial(ZEMaterial* Material);

		void							UpdateScrollbars();

										ZEUIScrollArea();
};

#endif
