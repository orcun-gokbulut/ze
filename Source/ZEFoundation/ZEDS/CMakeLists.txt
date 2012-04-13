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

ze_add_source(ZEAllocator.cpp			Sources)
ze_add_source(ZEAllocator.h				Sources Headers)
ze_add_source(ZEArray.cpp				Sources)
ze_add_source(ZEArray.h					Sources Headers)
ze_add_source(ZEArrayTests.cpp			Tests)
ze_add_source(ZEDelegate.cpp			Sources)
ze_add_source(ZEDelegate.h				Sources Headers)
ze_add_source(ZEDelegateImp.h			Sources Headers)
ze_add_source(ZEDelegateTests.cpp		Tests)
ze_add_source(ZEFastDelegate.h			Sources Headers)
ze_add_source(ZEFastDelegateBind.h		Sources Headers)
ze_add_source(ZEFlags.cpp				Sources)
ze_add_source(ZEFlags.h					Sources Headers)
ze_add_source(ZEFlagsTests.cpp			Tests)
ze_add_source(ZEHeapBase.h				Sources Headers)
ze_add_source(ZEHeapBase.cpp			Sources)
ze_add_source(ZEMinHeap.h				Sources Headers)
ze_add_source(ZEMinHeap.cpp				Sources)
ze_add_source(ZEMinHeapTests.cpp		Tests)
ze_add_source(ZEMaxHeap.h				Sources Headers)
ze_add_source(ZEMaxHeap.cpp				Sources)
ze_add_source(ZEMaxHeapTests.cpp		Tests)
ze_add_source(ZEList.h					Sources Headers)
ze_add_source(ZEList.cpp				Sources)
ze_add_source(ZETree.h					Sources Headers)
ze_add_source(ZETree.cpp				Sources Headers)
ze_add_source(ZEGraph.h					Sources Headers)
ze_add_source(ZEGraph.cpp				Sources Headers)
ze_add_source(ZEPointer.cpp				Sources)
ze_add_source(ZEPointer.h				Sources Headers)
ze_add_source(ZEPointerTests.cpp		Tests)
ze_add_source(ZESharedPointer.cpp		Sources)
ze_add_source(ZESharedPointer.h			Sources Headers)
ze_add_source(ZESharedPointerTests.cpp	Tests)
ze_add_source(ZEWeakPointer.cpp			Sources)
ze_add_source(ZEWeakPointer.h			Sources Headers)
ze_add_source(ZEWeakPointerTests.cpp	Tests)
ze_add_source(ZEString.cpp				Sources)
ze_add_source(ZEString.h				Sources Headers)
ze_add_source(ZEStringTests.cpp			Tests)
ze_add_source(ZETypedVariant.cpp		Sources)
ze_add_source(ZETypedVariant.h			Sources Headers)
ze_add_source(ZEVariant.cpp				Sources)
ze_add_source(ZEVariant.h				Sources Headers)
ze_add_source(ZEVariantTests.cpp		Tests)

ze_add_source(ZEPointerTestClass.h		Tests)
ze_add_source(ZEPointerTestClass.cpp	Tests)

ze_add_library(ZEDS 
	SOURCES ${Sources} 
	HEADERS ${Headers}
	LIBS ZEFoundation
	INSTALL
	INSTALL_DESTINATION ZEFoundation/ZEDS
	INSTALL_COMPONENT ZESDK)
	
ze_add_test(ZEDSTests 
	SOURCES ${Tests}
	TEST_TARGET ZEDS)
