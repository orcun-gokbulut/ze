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

add_subdirectory (ZEPhysX)

add_source (ZEPhysicalCallbacks.cpp			Sources)
add_source (ZEPhysicalCallbacks.h			Sources ZESDK)
add_source (ZEPhysicalJoint.cpp				Sources)
add_source (ZEPhysicalJoint.h				Sources ZESDK)
add_source (ZEPhysicalMaterial.cpp			Sources)
add_source (ZEPhysicalMaterial.h			Sources ZESDK)
add_source (ZEPhysicalMesh.cpp				Sources)
add_source (ZEPhysicalMesh.h				Sources ZESDK)
add_source (ZEPhysicalObject.cpp			Sources)
add_source (ZEPhysicalObject.h				Sources ZESDK)
add_source (ZEPhysicalRigidBody.cpp			Sources)
add_source (ZEPhysicalRigidBody.h			Sources ZESDK)
add_source (ZEPhysicalShapes.cpp			Sources)
add_source (ZEPhysicalShapes.h				Sources ZESDK)
add_source (ZEPhysicalTrigger.cpp			Sources)
add_source (ZEPhysicalTrigger.h				Sources ZESDK)
add_source (ZEPhysicalWorld.cpp				Sources)
add_source (ZEPhysicalWorld.h				Sources ZESDK)
add_source (ZEPhysicsModule.cpp				Sources)
add_source (ZEPhysicsModule.h				Sources ZESDK)
add_source (ZEPhysicsDebugModule.cpp	    Sources)
add_source (ZEPhysicsDebugModule.h	        Sources)

add_library (ZEPhysics ${Sources})

install (TARGETS ZEPhysics COMPONENT "Libraries" CONFIGURATIONS Release DESTINATION Lib)
install (FILES ${ZESDK}  COMPONENT "Includes" CONFIGURATIONS Release DESTINATION Include/ZEPhysics)

set_property(TARGET ZEPhysics PROPERTY FOLDER "Zinek Engine")
