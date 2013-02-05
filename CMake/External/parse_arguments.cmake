#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - parse_arguments.cmake
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

MACRO (PARSE_ARGUMENTS prefix arg_names option_names)
  SET (DEFAULT_ARGS)
  FOREACH (arg_name ${arg_names})    
    SET (${prefix}_${arg_name})
  ENDFOREACH (arg_name)
  FOREACH (option ${option_names})
    SET (${prefix}_${option} FALSE)
  ENDFOREACH (option)

  SET (current_arg_name DEFAULT_ARGS)
  SET (current_arg_list)
  FOREACH (arg ${ARGN})            
    SET (larg_names ${arg_names})    
    LIST (FIND larg_names "${arg}" is_arg_name)                   
    IF (is_arg_name GREATER -1)
      SET (${prefix}_${current_arg_name} ${current_arg_list})
      SET (current_arg_name ${arg})
      SET (current_arg_list)
    ELSE (is_arg_name GREATER -1)
      SET (loption_names ${option_names})    
      LIST (FIND loption_names "${arg}" is_option)            
      IF (is_option GREATER -1)
	     SET (${prefix}_${arg} TRUE)
      ELSE (is_option GREATER -1)
	     SET (current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option GREATER -1)
    ENDIF (is_arg_name GREATER -1)
  ENDFOREACH (arg)
  SET (${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO (PARSE_ARGUMENTS)
