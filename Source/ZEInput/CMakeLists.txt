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

#add_subdirectory (ZEDirectInput)
#add_subdirectory (ZEDummyInput)
#add_subdirectory (ZEVirtualInput)
add_subdirectory (ZEWindowsInput)
add_subdirectory (ZEFreespaceInput)
add_subdirectory (ZEVRPNInput)

ze_add_source(ZEInputAction.cpp							Sources)
ze_add_source(ZEInputAction.h							Sources Headers)
ze_add_source(ZEInputDefinitions.cpp					Sources)
ze_add_source(ZEInputDefinitions.h						Sources Headers)
ze_add_source(ZEInputDevice.cpp							Sources)
ze_add_source(ZEInputDevice.h							Sources Headers)
ze_add_source(ZEInputDeviceExtension.cpp				Sources)
ze_add_source(ZEInputDeviceExtension.h					Sources Headers)
ze_add_source(ZEInputDeviceExtensionDescription.cpp		Sources)
ze_add_source(ZEInputDeviceExtensionDescription.h		Sources Headers)
ze_add_source(ZEInputEvent.cpp							Sources)
ze_add_source(ZEInputEvent.h							Sources Headers)
ze_add_source(ZEInputMap.cpp							Sources)
ze_add_source(ZEInputMap.h								Sources Headers)
ze_add_source(ZEInputMapBinding.cpp						Sources)
ze_add_source(ZEInputMapBinding.h						Sources Headers)
ze_add_source(ZEInputModule.cpp							Sources)
ze_add_source(ZEInputModule.h							Sources Headers)
ze_add_source(ZEInputModuleDescription.cpp				Sources)
ze_add_source(ZEInputModuleDescription.h				Sources Headers)

ze_add_library(ZEInput 
	SOURCES ${Sources}
	HEADERS ${Headers}
	INSTALL
	INSTALL_DESTINATION "ZEInput"
	INSTALL_COMPONENT ZESDK)
