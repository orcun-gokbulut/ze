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


ze_add_source(ZEClass.cpp							Sources)
ze_add_source(ZEClass.h 							Sources Headers)
ze_add_source(ZEClassOverloads.cpp					Sources)
ze_add_source(ZEObject.cpp							Sources)
ze_add_source(ZEObject.h							Sources Headers)
ze_add_source(ZEEventDelegate.cpp					Sources)
ze_add_source(ZEEventDelegate.h						Sources Headers)
ze_add_source(ZEEventDelegateImp.h					Sources Headers)
ze_add_source(ZEEvent.cpp							Sources)
ze_add_source(ZEEvent.h								Sources Headers)
ze_add_source(ZEEventImp.h							Sources Headers)
ze_add_source(ZEEventSuppressor.cpp					Sources)
ze_add_source(ZEEventSuppressor.h					Sources Headers)
ze_add_source(ZEEnumerator.cpp						Sources)
ze_add_source(ZEEnumerator.h						Sources Headers)
ze_add_source(ZEMTCollection.cpp					Sources)
ze_add_source(ZEMTCollection.h						Sources Headers)
ze_add_source(ZEMTFundamental.cpp					Sources)
ze_add_source(ZEMTFundamental.h						Sources Headers)
ze_add_source(ZEMTDeclaration.cpp					Sources)
ze_add_source(ZEMTDeclaration.h 					Sources Headers)
ze_add_source(ZEMTAttribute.cpp						Sources)
ze_add_source(ZEMTAttribute.h						Sources Headers)
ze_add_source(ZEMTProperty.cpp						Sources)
ze_add_source(ZEMTProperty.h						Sources Headers)
ze_add_source(ZEMTPropertySignature.cpp				Sources)
ze_add_source(ZEMTPropertySignature.h				Sources Headers)
ze_add_source(ZEMTPropertySignatureGenerator.cpp	Sources)
ze_add_source(ZEMTPropertySignatureGenerator.h		Sources Headers)
ze_add_source(ZEMTMethod.cpp						Sources)
ze_add_source(ZEMTMethod.h							Sources Headers)
ze_add_source(ZEMTMethodSignature.cpp				Sources)
ze_add_source(ZEMTMethodSignature.h					Sources Headers)
ze_add_source(ZEMTMethodSignatureGenerator.cpp		Sources)
ze_add_source(ZEMTMethodSignatureGenerator.h		Sources Headers)
ze_add_source(ZEMTMethodSignatureGeneratorImp.h		Sources Headers)
ze_add_source(ZEMTProvider.cpp						Sources)
ze_add_source(ZEMTProvider.h						Sources Headers)
ze_add_source(ZEMTType.cpp							Sources)
ze_add_source(ZEMTType.h							Sources Headers)
ze_add_source(ZEMTTypeGenerator.cpp					Sources)
ze_add_source(ZEMTTypeGenerator.h					Sources Headers)
ze_add_source(ZEMTTypeGeneratorTests.cpp			Tests)
ze_add_source(ZEMTTypeGeneratorDynamic.cpp			Sources)
ze_add_source(ZEMTTypeGeneratorDynamic.h			Sources Headers)
ze_add_source(ZEMTTypeGeneratorDynamicTests.cpp		Tests)
ze_add_source(ZEMTRegistar.cpp						Sources)
ze_add_source(ZEMTRegistar.h						Sources Headers)
ze_add_source(ZEMTModuleRegistar.cpp				Sources)
ze_add_source(ZEMTModuleRegistar.h					Sources Headers)
ze_add_source(ZEMTTypeTestTable.h					Sources)
ze_add_source(ZEMTTestObject.h						Tests ZEMC)
ze_add_source(ZEMTTestObject.cpp					Tests)

ze_add_library(TARGET ZEMeta
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEDS)

ze_add_test(TARGET ZEMetaTests
	SOURCES ${Tests} ZEVariantTest.cpp
	ZEMC ${ZEMC}
	LIBS ZEFoundation
	TEST_TARGET ZEMeta)
