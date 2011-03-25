#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - CMakeLists.txt
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
*****************************************************************************]]
#ZE_SOURCE_PROCESSOR_END()

cmake_minimum_required (VERSION 2.8)

add_source (ZEFontFile.cpp						Sources)
add_source (ZEFontFile.h						Sources ZESDK)
add_source (ZEFontResource.cpp					Sources)
add_source (ZEFontResource.h					Sources ZESDK)
add_source (ZEIUCheckBoxControl.cpp				Sources)
add_source (ZEIUCheckBoxControl.h				Sources ZESDK)
add_source (ZEUIBorderedFrameControl.cpp		Sources)
add_source (ZEUIBorderedFrameControl.h			Sources ZESDK)
add_source (ZEUIButtonControl.cpp				Sources)
add_source (ZEUIButtonControl.h					Sources ZESDK)
add_source (ZEUIControl.cpp						Sources)
add_source (ZEUIControl.h						Sources ZESDK)
add_source (ZEUICursorControl.cpp				Sources)
add_source (ZEUICursorControl.h					Sources ZESDK)
add_source (ZEUIDebugModule.cpp					Sources)
add_source (ZEUIDebugModule.h					Sources ZESDK)
add_source (ZEUIEvents.cpp						Sources)
add_source (ZEUIEvents.h						Sources ZESDK)
add_source (ZEUIFrameControl.cpp				Sources)
add_source (ZEUIFrameControl.h					Sources ZESDK)
add_source (ZEUIHorizontalSliderControl.cpp		Sources)
add_source (ZEUIHorizontalSliderControl.h		Sources ZESDK)
add_source (ZEUIManager.cpp						Sources)
add_source (ZEUIManager.h						Sources ZESDK)
add_source (ZEUIMultiLineTextEdit.cpp			Sources)
add_source (ZEUIMultiLineTextEdit.h				Sources ZESDK)
add_source (ZEUIRadioButtonControl.cpp			Sources)
add_source (ZEUIRadioButtonControl.h			Sources ZESDK)
add_source (ZEUIRectangle.cpp					Sources)
add_source (ZEUIRectangle.h						Sources ZESDK)
add_source (ZEUIRenderer.cpp					Sources)
add_source (ZEUIRenderer.h						Sources ZESDK)
add_source (ZEUISliderControl.cpp				Sources)
add_source (ZEUISliderControl.h					Sources ZESDK)
add_source (ZEUITextControl.cpp					Sources)
add_source (ZEUITextControl.h					Sources ZESDK)
add_source (ZEUIVerticalSliderControl.cpp		Sources)
add_source (ZEUIVerticalSliderControl.h			Sources ZESDK)
add_source (ZEUIWindowControl.cpp				Sources)
add_source (ZEUIWindowControl.h					Sources ZESDK)
add_source (ZEUTextEditControl.cpp				Sources)
add_source (ZEUTextEditControl.h				Sources ZESDK)

add_library (ZEUI ${Sources})

set_property(TARGET ZEUI PROPERTY FOLDER "Zinek Engine")

install (TARGETS ZEUI COMPONENT "Libraries" CONFIGURATIONS Release DESTINATION Lib)
install (FILES ${ZESDK}  COMPONENT "Includes" CONFIGURATIONS Release DESTINATION Include/ZEUI)
