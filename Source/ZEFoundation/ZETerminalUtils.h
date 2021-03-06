//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerminalUtils.h
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

#include "ZETypes.h"

enum ZETerminalColor
{
	ZE_TC_DEFAULT,
	ZE_TC_BLACK,
	ZE_TC_RED,
	ZE_TC_GREEN,
	ZE_TC_BLUE,
	ZE_TC_YELLOW,
	ZE_TC_MEGENTA,
	ZE_TC_CYAN,
	ZE_TC_GRAY,
	ZE_TC_DARK_GRAY,
	ZE_TC_LIGHT_RED,
	ZE_TC_LIGHT_GREEN,
	ZE_TC_LIGHT_BLUE,
	ZE_TC_LIGHT_YELLOW,
	ZE_TC_LIGHT_MEGENTA,
	ZE_TC_LIGHT_CYAN,
	ZE_TC_LIGHT_GRAY,
	ZE_TC_WHITE,

};

class ZETerminalUtils
{
	public:
		static void							SetBold(bool Enabled);
		static void							SetBlink(bool Enabled);
		static void							SetUnderlined(bool Enabled);
		
		static void							SetForgroundColor(ZETerminalColor Color);
		static void							SetBackgroundColor(ZETerminalColor Color);

		static void							SetCursorPosition(ZEUInt x, ZEUInt y);
		static void							GetCursorPosition(ZEUInt& x, ZEUInt& y);
		static void							SetCursorPositionX(ZEUInt x);
		static ZEUInt						GetCursorPositionX();
		static void							SetCursorPositionY(ZEUInt y);
		static ZEUInt						GetCursorPositionY();
		
		static void							Reset();
};
