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

cmake_minimum_required(VERSION 2.8)

project(ZinekEngine)

ze_set_project_folder("ZEEngine")

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ZE_ZINEK_ENGINE)

add_subdirectory(ZEAI)
add_subdirectory(ZECore)
add_subdirectory(ZEDebug)
add_subdirectory(ZEGame)
add_subdirectory(ZEGraphics)
add_subdirectory(ZERenderer)
add_subdirectory(ZETexture)
add_subdirectory(ZEInput)
add_subdirectory(ZEInterior)
add_subdirectory(ZEModel)
add_subdirectory(ZEParticle)
add_subdirectory(ZEPhysics)
add_subdirectory(ZESound)
add_subdirectory(ZEAtmosphere)
add_subdirectory(ZEUI)
#add_subdirectory(ZETerrain)
add_subdirectory(ZEResource)

ze_add_source(ZEAPI.h 						Sources Headers)
ze_add_source(ZECompileOptions.h 			Sources Headers)
ze_add_source(ZEDefinitions.h 				Sources Headers)
ze_add_source(ZESDK.h 						Sources Headers)
ze_add_source(ZESDK.cpp 					Sources)

ze_add_library(TARGET ZEEngine 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZECore ZEGraphics ZEInput ZEInterior ZEParticle ZEMeta ZEPhysics ZESound ZEGame ZEAI ZETexture ZERenderer ZEModel ZEModules ZEAtmosphere ZEResource ZEUI)

ze_add_executable(TARGET ZE 
	SOURCES ${Sources} ZEMain.cpp ZECore/ZEWindowResources.rc
	LIBS ZEEngine
	INSTALL)

ze_meta_register(LIBS ZECore ZEInput ZEInterior ZEParticle ZEMeta ZEPhysics ZESound ZEGame ZEAI ZETerrain ZETexture ZERenderer ZEModel ZEModules ZEAtmosphere ZEResource ZEUI)

#ze_combine_libraries(ZEEngine LIBS ZEEngine ZEAI ZECore ZEGame ZEGraphics ZETexture ZEInput ZEMap ZEMeta ZEModel ZEParticle ZEPhysics ZESound ZEUI ZETerrain)
