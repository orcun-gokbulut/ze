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

project(ZEAI)

ze_add_source(ZEActor.h							Sources Headers ZEMC)
ze_add_source(ZEActor.cpp						Sources)
ze_add_source(ZESteering.h						Sources Headers)
ze_add_source(ZESteering.cpp					Sources)
ze_add_source(ZESteeringAlign.h		            Sources Headers)
ze_add_source(ZESteeringAlign.cpp		        Sources)
ze_add_source(ZESteeringArrive.h	        	Sources Headers)
ze_add_source(ZESteeringArrive.cpp		        Sources)
ze_add_source(ZESteeringCollisionAvoidance.h	Sources Headers)
ze_add_source(ZESteeringCollisionAvoidance.cpp	Sources)
ze_add_source(ZESteeringFace.h	        	    Sources Headers)
ze_add_source(ZESteeringFace.cpp	        	Sources)
ze_add_source(ZESteeringFaceVelocity.h	        Sources Headers)
ze_add_source(ZESteeringFaceVelocity.cpp	    Sources)
ze_add_source(ZESteeringFlee.h	        	    Sources Headers)
ze_add_source(ZESteeringFlee.cpp	        	Sources)
ze_add_source(ZESteeringPathFollowing.h	        Sources Headers)
ze_add_source(ZESteeringPathFollowing.cpp	    Sources)
ze_add_source(ZESteeringSeek.h	        	    Sources Headers)
ze_add_source(ZESteeringSeek.cpp	        	Sources)
ze_add_source(ZESteeringSeparate.h	        	Sources Headers)
ze_add_source(ZESteeringSeparate.cpp	        Sources)
ze_add_source(ZESteeringStopper.h	        	Sources Headers)
ze_add_source(ZESteeringStopper.cpp	        	Sources)
ze_add_source(ZESteeringUserTankControl.h	    Sources Headers)
ze_add_source(ZESteeringUserTankControl.cpp	    Sources)
ze_add_source(ZESteeringVelocityMatching.h	    Sources Headers)
ze_add_source(ZESteeringVelocityMatching.cpp	Sources)
ze_add_source(ZESteeringWander.h	        	Sources Headers)
ze_add_source(ZESteeringWander.cpp	        	Sources)
ze_add_source(ZEAStar.h							Sources Headers)
ze_add_source(ZEAStar.cpp						Sources)
ze_add_source(ZEAStarTests.cpp					Tests)
ze_add_source(ZENavigationMesh.h				Sources Headers)
ze_add_source(ZENavigationMesh.cpp				Sources)
ze_add_source(ZENavigationMeshGenerator.h		Sources Headers)
ze_add_source(ZENavigationMeshGenerator.cpp		Sources)

ze_add_library(ZEAI
	SOURCES ${Sources} 
	HEADERS ${Headers}
	ZEMC ${ZEMC}
	LIBS ZEFoundation)
	
ze_add_test(ZEAITests 
	SOURCES ${Tests}
	ZEMC ${ZEMC}
	TEST_TARGET ZEAI)
