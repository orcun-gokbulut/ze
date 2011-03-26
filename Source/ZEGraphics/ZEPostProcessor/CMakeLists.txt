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

add_source (ZEPostProcessor.cpp			Sources)
add_source (ZEPostProcessor.h			Sources ZESDK)
add_source (ZEPostProcessorNode.cpp		Sources)
add_source (ZEPostProcessorNode.h		Sources ZESDK)
add_source (ZEPPBlurNode.cpp			Sources)
add_source (ZEPPBlurNode.h				Sources ZESDK)
add_source (ZEPPColorInputNode.cpp		Sources)
add_source (ZEPPColorInputNode.h		Sources ZESDK)
add_source (ZEPPDepthInputNode.cpp		Sources)
add_source (ZEPPDepthInputNode.h		Sources ZESDK)
add_source (ZEPPFilter1DNode.cpp		Sources)
add_source (ZEPPFilter1DNode.h			Sources ZESDK)
add_source (ZEPPFilter2DNode.cpp		Sources)
add_source (ZEPPFilter2DNode.h			Sources ZESDK)
add_source (ZEPPScreenOutputNode.cpp	Sources)
add_source (ZEPPScreenOutputNode.h		Sources ZESDK)
add_source (ZEPPTextureInputNode.cpp	Sources)
add_source (ZEPPTextureInputNode.h		Sources ZESDK)
add_source (ZEPPTextureOutputNode.cpp	Sources)
add_source (ZEPPTextureOutputNode.h		Sources ZESDK)
add_source (ZEPPVelocityInputNode.cpp	Sources)
add_source (ZEPPVelocityInputNode.h		Sources ZESDK)

add_library (ZEPostProcessor ${Sources})

set_property(TARGET ZEPostProcessor PROPERTY FOLDER "Zinek Engine")
