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

ze_set_project_folder("ZEModules")

ze_add_module(ZEDirect3D9			Modules PLATFORMS Windows)
ze_add_module(ZEDirectInput			Modules OPTIONAL DEFAULT PLATFORMS Windows )
ze_add_module(ZEVirtualInput		Modules OPTIONAL DEFAULT)
ze_add_module(ZEWindowsInput		Modules OPTIONAL DEFAULT PLATFORMS Windows)
ze_add_module(ZEDirectSound			Modules OPTIONAL DEFAULT PLATFORMS Windows)
ze_add_module(ZEOpenAL				Modules OPTIONAL DEFAULT PLATFORMS Windows Linux MacOSX BSD)
ze_add_module(ZEPhysX				Modules OPTIONAL DEFAULT PLATFORMS Windows Linux MacOSX BSD)

ze_add_module(ZEVRPNInput			Modules OPTIONAL DEFAULT PLATFORMS Windows Linux MacOSX BSD)
ze_add_module(ZEXSens				Modules OPTIONAL PLATFORMS Windows)
ze_add_module(ZESensics				Modules OPTIONAL)
ze_add_module(ZEISenseInput			Modules OPTIONAL)
ze_add_module(ZEVectorNav			Modules OPTIONAL)

ze_add_source(ZEModules.h			Sources Headers)
ze_add_source(ZEModules.cpp			Sources)
ze_add_source(CMakeLists.txt		Sources)

ze_add_library(TARGET ZEModules 
	SOURCES ${Sources}
	LIBS ${Modules})

#ze_combine_libraries(TARGET ZEModules 
#	LIBS ZEDirect3D9
#		 ZEWindowsInput
#		 ZEVRPNInput
#		 ZEDirectSound
#		 ZEOpenAL
#		 ZEPhysX)
