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

project(ZEMathTests)

append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS ZE_ZINEK_ENGINE)
	
add_source(ZEAABBoxTest.cpp				Sources)
add_source(ZEBSphereTest.cpp			Sources)
add_source(ZELineSegmentTest.cpp		Sources)
add_source(ZELineTest.cpp				Sources)
add_source(ZEMathTests.cpp				Sources)
add_source(ZEMathTests.h				Sources)
add_source(ZEMatrixTest.cpp				Sources)
add_source(ZEOBBoxTest.cpp				Sources)
add_source(ZEPlaneTest.cpp				Sources)
add_source(ZEQuaternionTest.cpp			Sources)
add_source(ZERayTest.cpp				Sources)
add_source(ZERectangle3DTest.cpp		Sources)
add_source(ZERectangleTest.cpp			Sources)
add_source(ZETriangleTest.cpp			Sources)
add_source(ZETrigonometryTest.cpp		Sources)
add_source(ZEVectorTest.cpp				Sources)

add_executable(ZEMathTests ${Sources})

set_property(TARGET ZEMathTests PROPERTY FOLDER "ZETests")
target_link_libraries (ZEMathTests ZETestsCommon ZEMath libunittest++.lib)

