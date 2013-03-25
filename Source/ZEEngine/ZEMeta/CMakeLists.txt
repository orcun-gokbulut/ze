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

ze_add_source(ZEClass.cpp						Sources)
ze_add_source(ZEClass.h 						Sources Headers)
ze_add_source(ZEClassOverloads.cpp				Sources)
ze_add_source(ZEObject.cpp						Sources)
ze_add_source(ZEObject.h						Sources Headers)
ze_add_source(ZEAnimation.cpp					Sources)
ze_add_source(ZEAnimation.h						Sources Headers)
ze_add_source(ZEPostProcessor.cpp				Sources)
ze_add_source(ZEPostProcessor.h					Sources Headers)
ze_add_source(ZEMetaAttribute.cpp				Sources)
ze_add_source(ZEMetaAttribute.h					Sources Headers)
ze_add_source(ZEProperty.cpp					Sources)
ze_add_source(ZEProperty.h						Sources Headers)
ze_add_source(ZEPropertySignature.cpp			Sources)
ze_add_source(ZEPropertySignature.h				Sources Headers)
ze_add_source(ZEPropertySignatureGenerator.cpp	Sources)
ze_add_source(ZEPropertySignatureGenerator.h	Sources Headers)
ze_add_source(ZEMethod.cpp						Sources)
ze_add_source(ZEMethod.h						Sources Headers)
ze_add_source(ZEMethodSignature.cpp				Sources)
ze_add_source(ZEMethodSignature.h				Sources Headers)
ze_add_source(ZEMethodSignatureGenerator.cpp	Sources)
ze_add_source(ZEMethodSignatureGenerator.h		Sources Headers)
ze_add_source(ZEMethodSignatureGeneratorImp.h	Sources Headers)
ze_add_source(ZEContainer.cpp					Sources)
ze_add_source(ZEContainer.h						Sources Headers)
ze_add_source(ZEEnumurator.cpp					Sources)
ze_add_source(ZEEnumurator.h					Sources Headers)
ze_add_source(ZEEnum.cpp						Sources)
ze_add_source(ZEEnum.h							Sources Headers)
ze_add_source(ZEEvent.cpp						Sources)
ze_add_source(ZEEvent.h							Sources Headers)
ze_add_source(ZEEventImp.h						Sources Headers)
ze_add_source(ZEEventHandler.cpp				Sources)
ze_add_source(ZEEventHandler.h					Sources Headers)
ze_add_source(ZEEventHandlerImp.h				Sources Headers)
ze_add_source(ZEProvider.cpp					Sources)
ze_add_source(ZEProvider.h						Sources Headers)
ze_add_source(ZEMetaTestClass.cpp				Sources)
ze_add_source(ZEMetaTestClass.h					Sources Headers)
ze_add_source(ZEInteger.cpp						Sources)
ze_add_source(ZEInteger.h						Sources Headers)
ze_add_source(ZEReference.cpp					Sources)
ze_add_source(ZEReference.h						Sources Headers)
ze_add_source(ZEMetaRegister.cpp				Sources)
ze_add_source(ZEMetaRegister.h					Sources Headers)
ze_add_source(ZEMetaModuleRegister.cpp			Sources)
ze_add_source(ZEMetaModuleRegister.h			Sources Headers)


ze_add_library(ZEMeta 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEFoundation)
