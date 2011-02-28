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

add_subdirectory (ZECore)
add_subdirectory (ZEDebug)
add_subdirectory (ZEDS)
add_subdirectory (ZEGame)
add_subdirectory (ZEGraphics)
add_subdirectory (ZEInput)
add_subdirectory (ZEMap)
add_subdirectory (ZEMath)
add_subdirectory (ZEMeta)
add_subdirectory (ZEModel)
add_subdirectory (ZENetwork)
add_subdirectory (ZEParticle)
add_subdirectory (ZEPhysics)
add_subdirectory (ZEScript)
add_subdirectory (ZESound)
add_subdirectory (ZEUI)

add_source (ZEAPI.h 				Sources ZESDK)
add_source (ZECompileOptions.h 		Sources ZESDK)
add_source (ZEDefinitions.h 		Sources ZESDK)
add_source (ZEFastDelegate.h 		Sources ZESDK)
add_source (ZEFastDelegateBind.h	Sources ZESDK)
add_source (ZEMain.cpp 				Sources)
add_source (ZESDK.cpp 				Sources)
add_source (ZESDK.h 				Sources ZESDK)
add_source (ZETypes.h				Sources ZESDK)

add_executable (ZE WIN32 ${Sources})
target_link_libraries (ZE ZECore ZEDebug ZEDS ZEGame ZEGraphics ZEInput ZEMap ZEMath ZEMeta ZEModel ZEParticle ZEPhysics ZESound ZEUI)
target_link_libraries (ZE ZEDirect3D9 ZEDirectInput ZEDirectSound ZEOpenAL ZEPhysX ZEDummyInput ZEVirtualInput ZEPortalMap)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ZE PROPERTY FOLDER "Zinek Engine")

install (TARGETS ZE RUNTIME DESTINATION bin COMPONENT "Runtime")
install (FILES ${ZESDK}  COMPONENT "Includes" CONFIGURATIONS Release DESTINATION Include)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/Dll/ CONFIGURATIONS Release ARCHIVE DESTINATION bin COMPONENT "Runtime")
