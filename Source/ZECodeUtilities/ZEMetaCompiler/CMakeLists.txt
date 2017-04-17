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

ze_add_source(ZEMCDriver.cpp 							Sources)
ze_add_source(ZEMCDriver.h 								Sources)
ze_add_source(ZEMCOptions.cpp 							Sources)
ze_add_source(ZEMCOptions.h 							Sources)
ze_add_source(ZEMCContext.cpp							Sources)
ze_add_source(ZEMCContext.h								Sources)
ze_add_source(ZEMCGenerator.cpp							Sources)
ze_add_source(ZEMCGenerator.h							Sources)
ze_add_source(ZEMCGeneratorDeclaration.cpp				Sources)
ze_add_source(ZEMCGeneratorEnumerator.cpp				Sources)
ze_add_source(ZEMCGeneratorClass.cpp					Sources)
ze_add_source(ZEMCGeneratorClassProperty.cpp			Sources)
ze_add_source(ZEMCGeneratorClassMethod.cpp				Sources)
ze_add_source(ZEMCGeneratorClassScripting.cpp			Sources)
ze_add_source(ZEMCGeneratorClassMemoryManagement.cpp	Sources)
ze_add_source(ZEMCParser.cpp 							Sources)
ze_add_source(ZEMCParserAttribute.cpp					Sources)
ze_add_source(ZEMCParserType.cpp						Sources)
ze_add_source(ZEMCParserClass.cpp						Sources)
ze_add_source(ZEMCParserProperty.cpp					Sources)
ze_add_source(ZEMCParserMethod.cpp						Sources)
ze_add_source(ZEMCParser.h								Sources)
ze_add_source(ZEMCRegisterer.h							Sources)
ze_add_source(ZEMCRegisterer.cpp						Sources)

add_definitions(-D ZE_FOUNDATION_STATIC)
ze_add_executable(TARGET ZEMetaCompiler 
	CONSOLE 
	SOURCES ${Sources} ${DependencySources}
	LIBS ZEFoundationStatic libClang)

source_group("Dependencies" FILES ${DependencySources})

set_property(TARGET ZEMetaCompiler PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Tool")
