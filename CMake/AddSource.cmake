#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - AddSource.cmake
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

MACRO(ADD_SOURCE)
	PARSE_ARGUMENTS(SOURCE "" "ZEPP" ${ARGN})
	LIST(GET SOURCE_DEFAULT_ARGS 0 SOURCE_FILE)
	
	SET (SOURCE_LISTS ${SOURCE_DEFAULT_ARGS})
	LIST (REMOVE_AT SOURCE_LISTS 0)
	LIST (GET SOURCE_LISTS 0 SOURCE_FILE_LIST)

	IF (SOURCE_ZEPP)
		ADD_CUSTOM_COMMAND (
			COMMAND "${CMAKE_SOURCE_DIR}/ZEPP/zepp.exe" 
			ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}
			OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}.zpp)
		list (APPEND ${SOURCE_FILE_LIST} ${SOURCE_FILE}.zpp)
		SOURCE_GROUP("ZPP" FILES ${SOURCE_FILE}.zpp)
		SET_PROPERTY(SOURCE ${SOURCE_FILE}.zpp PROPERTY LANGUAGE CXX )
		list (APPEND ZEPP ${SOURCE_FILE}.zpp)
	ENDIF (SOURCE_ZEPP)
	
	LIST(APPEND ${SOURCE_FILE_LIST} ${SOURCE_FILE})
	SOURCE_GROUP("" FILES ${SOURCE_FILE})
	LIST (REMOVE_AT SOURCE_LISTS 0)

	FOREACH (SDKList ${SOURCE_LISTS})
		LIST (APPEND ${SDKList} ${SOURCE_FILE})
	ENDFOREACH (SDKList)
	
	#MESSAGE("File : " ${SOURCE_FILE})
	#MESSAGE("- Arguments : " ${ARGN})
	#MESSAGE("- Default Arguments : " ${SOURCE_DEFAULT_ARGS})
	#MESSAGE("- Source List Name : " ${SOURCE_FILE_LIST})
	#MESSAGE("- Additional List Names : " ${SOURCE_LISTS})
	#MESSAGE("- ZEPP : " ${SOURCE_ZEPP})
	#MESSAGE("- ZEPP : " ${ZEPP})
ENDMACRO(ADD_SOURCE)
