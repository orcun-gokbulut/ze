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

ze_add_source(ZEAABBox.cpp						Sources)
ze_add_source(ZEAABBox.h						Sources Headers)
ze_add_source(ZEAngle.cpp						Sources)
ze_add_source(ZEAngle.h							Sources Headers)
ze_add_source(ZEAngleTests.cpp					Tests)
ze_add_source(ZEAABBoxTests.cpp					Tests)
ze_add_source(ZEBSphere.cpp						Sources)
ze_add_source(ZEBSphere.h						Sources Headers)
ze_add_source(ZEBSphereTests.cpp				Tests)
ze_add_source(ZELine.cpp						Sources)
ze_add_source(ZELine.h							Sources Headers)
ze_add_source(ZELineTests.cpp					Tests)
ze_add_source(ZELineSegment.cpp					Sources)
ze_add_source(ZELineSegment.h					Sources Headers)
ze_add_source(ZELineSegmentTests.cpp			Tests)
ze_add_source(ZEMath.h							Sources Headers)
ze_add_Source(ZEMath.cpp						Sources)
ze_add_source(ZEMatrix.cpp						Sources)
ze_add_source(ZEMatrix3x3Tests.cpp				Tests)
ze_add_source(ZEMatrix4x4Tests.cpp				Tests)
ze_add_source(ZEMatrix.h						Sources Headers)
ze_add_source(ZEMatrixdGeneric.cpp				Sources)
ze_add_source(ZEMatrixd.cpp						Sources)
ze_add_source(ZEMatrixd.h						Sources Headers)
ze_add_source(ZEOBBox.cpp						Sources)
ze_add_source(ZEOBBox.h							Sources Headers)
ze_add_source(ZEOBBoxTests.cpp					Tests)
ze_add_source(ZEPlane.cpp						Sources)
ze_add_source(ZEPlane.h							Sources Headers)
ze_add_source(ZEPlaneTests.cpp					Tests)
ze_add_source(ZEQuaternion.cpp					Sources)
ze_add_source(ZEQuaternion.h					Sources Headers)
ze_add_source(ZEQuaternionTests.cpp				Tests)
ze_add_source(ZERay.cpp							Sources)
ze_add_source(ZERay.h							Sources Headers)
ze_add_source(ZERayTests.cpp					Tests)
ze_add_source(ZERectangle.cpp					Sources)
ze_add_source(ZERectangle.h						Sources Headers)
ze_add_source(ZERectangleTests.cpp				Tests)
ze_add_source(ZERectangle3D.cpp					Sources)
ze_add_source(ZERectangle3D.h					Sources Headers)
ze_add_source(ZERectangle3DTests.cpp			Tests)
ze_add_source(ZETriangle.cpp					Sources)
ze_add_source(ZETriangle.h						Sources Headers)
ze_add_source(ZETriangleTests.cpp				Tests)
ze_add_source(ZEVector.cpp						Sources)
ze_add_source(ZEVector.h						Sources Headers)
ze_add_source(ZEVectord.cpp						Sources)
ze_add_source(ZEVectord.h						Sources Headers)
ze_add_source(ZEVectorSwizzle.cpp				Sources)
ze_add_source(ZEVectorSwizzle.h					Sources Headers)
ze_add_source(ZEVectordGeneric.cpp				Sources)
ze_add_source(ZEVector2Tests.cpp				Tests)
ze_add_source(ZEVector3Tests.cpp				Tests)
ze_add_source(ZEVector4Tests.cpp				Tests)
ze_add_source(ZEPolygon.h						Sources Headers)
ze_add_source(ZEPolygon.cpp						Sources)
ze_add_source(ZEFrustum.h						Sources Headers)
ze_add_source(ZEFrustum.cpp						Sources)
ze_add_source(ZEViewVolume.h					Sources Headers)
ze_add_source(ZEViewVolume.cpp					Sources)


if (ZEBUILD_PLATFORM_ARCHITECTURE_SSE4)
	ze_add_source(ZEMatrixSSE4.cpp			Sources)
	ze_add_source(ZEVectorSSE4.cpp			Sources)
    add_definitions(-D__SSE4__)
    add_definitions(-D__SSE__)
    add_definitions(-D__MMX__)
elseif (ZEBUILD_PLATFORM_ARCHITECTURE_SSE3)
	ze_add_source(ZEMatrixSSE3.cpp			Sources)
	ze_add_source(ZEVectorSSE3.cpp			Sources)
    add_definitions(-D__SSE3__)
    add_definitions(-D__SSE__)
    add_definitions(-D__MMX__)
else()
	ze_add_source(ZEMatrixGeneric.cpp		Sources)
	ze_add_source(ZEVectorGeneric.cpp		Sources)
endif()

set(BuildIn ${BuildIn}
	ZEMath/ZEVector.h
	ZEMath/ZEVectord.h
	ZEMath/ZEQuaternion.h
	ZEMath/ZEMatrix.h	
	ZEMath/ZEMatrixd.h
	PARENT_SCOPE)

ze_add_library(TARGET ZEMath 
	SOURCES ${Sources} 
	HEADERS ${Headers})
	
ze_add_test(TARGET ZEMathTests
	SOURCES ${Tests}
	EXTRA_SOURCES ZEMathIOStreamMapping.cpp ZEMathIOStreamMapping.h
	TEST_TARGET ZEMath)
