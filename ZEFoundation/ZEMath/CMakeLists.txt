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

add_source (ZEAABBox.cpp			Sources)
add_source (ZEAABBox.h				Sources ZESDK)
add_source (ZEBSphere.cpp			Sources)
add_source (ZEBSphere.h				Sources ZESDK)
add_source (ZEMathDefinitions.cpp	Sources)
add_source (ZEMathDefinitions.h		Sources ZESDK)
add_source (ZELine.cpp				Sources)
add_source (ZELine.h				Sources ZESDK)
add_source (ZELineSegment.cpp		Sources)
add_source (ZELineSegment.h			Sources ZESDK)
add_source (ZEMatrix.cpp			Sources)
add_source (ZEMatrix.h				Sources ZESDK)
add_source (ZEOBBox.cpp				Sources)
add_source (ZEOBBox.h				Sources ZESDK)
add_source (ZEPlane.cpp				Sources)
add_source (ZEPlane.h				Sources ZESDK)
add_source (ZEQuaternion.cpp		Sources)
add_source (ZEQuaternion.h			Sources ZESDK)
add_source (ZERay.cpp				Sources)
add_source (ZERay.h					Sources ZESDK)
add_source (ZERectangle.cpp			Sources)
add_source (ZERectangle.h			Sources ZESDK)
add_source (ZERectangle3D.cpp		Sources)
add_source (ZERectangle3D.h			Sources ZESDK)
add_source (ZETriangle.cpp			Sources)
add_source (ZETriangle.h			Sources ZESDK)
add_source (ZETrigonometry.cpp		Sources)
add_source (ZETrigonometry.h		Sources ZESDK)
add_source (ZEVector.cpp			Sources)
add_source (ZEVector.h				Sources ZESDK)

add_library (ZEMath ${Sources})

set_property(TARGET ZEMath PROPERTY FOLDER "ZEFoundation")

install (TARGETS ZEMath COMPONENT "Libraries" CONFIGURATIONS Release DESTINATION Lib)
install (FILES ${ZESDK}  COMPONENT "Includes" CONFIGURATIONS Release DESTINATION Include/ZEMath)
