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


ze_add_source(ZEIPAddress.cpp							Sources)
ze_add_source(ZEIPAddress_Unix.cpp						Sources PLATFORMS Unix)
ze_add_source(ZEIPAddress_Windows.cpp					Sources PLATFORMS Windows)
ze_add_source(ZEIPAddress.h								Sources Headers)

ze_add_source(ZESocketDefinitions.cpp					Sources)
ze_add_source(ZESocketDefinitions.h						Sources Headers)

ze_add_source(ZESocketUDP.cpp							Sources)
ze_add_source(ZESocketUDP_Unix.cpp						Sources PLATFORMS Unix)
ze_add_source(ZESocketUDP_Windows.cpp					Sources PLATFORMS Windows)
ze_add_source(ZESocketUDP.h								Sources Headers)

ze_add_source(ZESocketTCPClient.cpp						Sources)
ze_add_source(ZESocketTCPClient_Unix.cpp				Sources PLATFORMS Unix)
ze_add_source(ZESocketTCPClient_Windows.cpp				Sources PLATFORMS Windows)
ze_add_source(ZESocketTCPClient.h						Sources Headers)

ze_add_source(ZESocketTCPServer.cpp						Sources)
ze_add_source(ZESocketTCPServer_Unix.cpp				Sources PLATFORMS Unix)
ze_add_source(ZESocketTCPServer_Windows.cpp				Sources PLATFORMS Windows)
ze_add_source(ZESocketTCPServer.h						Sources Headers)

ze_add_source(ZESocketTCPServerListener.cpp				Sources)
ze_add_source(ZESocketTCPServerListener_Unix.cpp		Sources PLATFORMS Unix)
ze_add_source(ZESocketTCPServerListener_Windows.cpp		Sources PLATFORMS Windows)
ze_add_source(ZESocketTCPServerListener.h				Sources Headers)

#ze_add_source(ZEIPAddressTests_Windows.cpp				Tests PLATFORMS Windows)
#ze_add_source(ZESocketTests_Windows.cpp				Tests PLATFORMS Windows)

ze_add_library(TARGET ZESocket
	SOURCES ${Sources}
	HEADERS ${Headers}
    LIBS ZEDS)

ze_link(TARGET ZESocket LIBS ws2_32.lib PLATFORMS Windows)

#ze_add_test(TARGET ZESocketTests
#	SOURCES ${Tests}
#	EXTRA_SOURCES
#	TEST_TARGET ZESocket)
