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

ze_add_source(ZEFontResource.cpp				Sources)
ze_add_source(ZEFontResource.h					Sources Headers)
ze_add_source(ZEIUCheckBoxControl.cpp			Sources)
ze_add_source(ZEIUCheckBoxControl.h				Sources Headers)
ze_add_source(ZEUIBorderedFrameControl.cpp		Sources)
ze_add_source(ZEUIBorderedFrameControl.h		Sources Headers)
ze_add_source(ZEUIButtonControl.cpp				Sources)
ze_add_source(ZEUIButtonControl.h				Sources Headers)
ze_add_source(ZEUIControl.cpp					Sources)
ze_add_source(ZEUIControl.h						Sources Headers)
ze_add_source(ZEUICursorControl.cpp				Sources)
ze_add_source(ZEUICursorControl.h				Sources Headers)
ze_add_source(ZEUIDebugModule.cpp				Sources)
ze_add_source(ZEUIDebugModule.h					Sources Headers)
ze_add_source(ZEUIEvents.cpp					Sources)
ze_add_source(ZEUIEvents.h						Sources Headers)
ze_add_source(ZEUIFrameControl.cpp				Sources)
ze_add_source(ZEUIFrameControl.h				Sources Headers)
ze_add_source(ZEUIHorizontalSliderControl.cpp	Sources)
ze_add_source(ZEUIHorizontalSliderControl.h		Sources Headers)
ze_add_source(ZEUIManager.cpp					Sources)
ze_add_source(ZEUIManager.h						Sources Headers)
ze_add_source(ZEUIMultiLineTextEdit.cpp			Sources)
ze_add_source(ZEUIMultiLineTextEdit.h			Sources Headers)
ze_add_source(ZEUIRadioButtonControl.cpp		Sources)
ze_add_source(ZEUIRadioButtonControl.h			Sources Headers)
ze_add_source(ZEUIRectangle.cpp					Sources)
ze_add_source(ZEUIRectangle.h					Sources Headers)
ze_add_source(ZEUIRenderer.cpp					Sources)
ze_add_source(ZEUIRenderer.h					Sources Headers)
ze_add_source(ZEUISliderControl.cpp				Sources)
ze_add_source(ZEUISliderControl.h				Sources Headers)
ze_add_source(ZEUITextControl.cpp				Sources)
ze_add_source(ZEUITextControl.h					Sources Headers)
ze_add_source(ZEUIVerticalSliderControl.cpp		Sources)
ze_add_source(ZEUIVerticalSliderControl.h		Sources Headers)
ze_add_source(ZEUIWindowControl.cpp				Sources)
ze_add_source(ZEUIWindowControl.h				Sources Headers)
ze_add_source(ZEUTextEditControl.cpp			Sources)
ze_add_source(ZEUTextEditControl.h				Sources Headers)
ze_add_source(ZEUISceneStatisticsControl.cpp	Sources)
ze_add_source(ZEUISceneStatisticsControl.h		Sources Headers)
ze_add_source(ZEUIPortalMapStatisticsControl.cpp	Sources)
ze_add_source(ZEUIPortalMapStatisticsControl.h		Sources Headers)

ze_add_library(ZEUI
	LIBS ZEFoundation 
	SOURCES ${Sources}
	HEADERS ${Headers}
	INSTALL
	INSTALL_DESTINATION "ZEEngineAPI/ZEUI"
	INSTALL_COMPONENT ZESDK)
