#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - ze_symbol_server.cmake
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

macro(ze_symbols_server_init)
	if (ZEBUILD_PLATFORM_WINDOWS)
		set(ZEBUILD_SYMBOL_SERVER_ENABLE FALSE CACHE BOOL "Generate symbol related targets.")
		set(ZEBUILD_SYMBOL_SERVER_ADDRESS "\\\\Server\\Symbols\\Zinek" CACHE STRING "Symbol server address.")
		set(ZEBUILD_SYMBOL_SERVER_COMMENT "" CACHE STRING "Comment text of the store operation.")
	
		ze_symbols_server_generate_targets()
	endif()
endmacro()

function(ze_symbols_server_generate_targets)
	file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Source" SOURCE_DIR_NATIVE)
	file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/Rundir" RUNDIR_DIR_NATIVE)
	
	add_custom_target(IndexSymbols
		COMMAND "${CMAKE_SOURCE_DIR}/Tool/Windows/SymbolServer/ssindex.cmd" 
			"/Debug" "/System=svn" "/Source=\"${SOURCE_DIR_NATIVE}\"" "/Symbols=\"${RUNDIR_DIR_NATIVE}\""
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		COMMENT "Indexing sources of pdb files in version control")
	set_property(TARGET IndexSymbols PROPERTY FOLDER "SymbolServer")
	
	add_custom_target(StoreSymbols
		COMMAND "${CMAKE_SOURCE_DIR}/Tool/Windows/SymbolServer/symstore.exe" 
			"add" "/o" "/r" "/f" "\"${RUNDIR_DIR_NATIVE}\\*.pdb\"" "/s" "\"${ZEBUILD_SYMBOL_SERVER_ADDRESS}\"" "/t" "\"Zinek Engine\"" "/v" "\"${ZEBUILD_VERSION}\""
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		COMMENT "Storing (Uploading) pdb files into Symbol Server. URL: ${ZEBUILD_SYMBOL_SERVER_ADDRESS}"
		DEPENDS IndexSymbols)
	set_property(TARGET StoreSymbols PROPERTY FOLDER "SymbolServer")
endfunction()

ze_symbols_server_init()
