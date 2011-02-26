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


add_source(ZEPortalMap.cpp ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMap.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapDoor.cpp ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapDoor.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapFileFormat.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapOctree.cpp ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapOctree.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapPortal.cpp ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapPortal.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapPortalDoor.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapPortalOctree.cpp ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapPortalOctree.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapResource.cpp ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPortalMapResource.h ZEPortalMap_SOURCE_FILES INCLUDED_SDKS zesdk)

add_library(ZEPortalMap ${ZEPortalMap_SOURCE_FILES})

install(TARGETS ZEPortalMap COMPONENT "Libraries" CONFIGURATIONS Release DESTINATION Lib)
install(FILES ${zesdk_INCLUDES}  COMPONENT "Includes" CONFIGURATIONS Release DESTINATION Includes/ZEMap/ZEPortalMap)
