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

ze_add_source(ZECanvas.cpp							Sources)
ze_add_source(ZECanvas.h 							Sources Headers)
ze_add_source(ZECamera.cpp							Sources)
ze_add_source(ZECamera.h							Sources Headers ZEMC)

ze_add_source(ZERNRenderer.cpp 						Sources)
ze_add_source(ZERNRenderer.h 						Sources Headers)
ze_add_source(ZERNCuller.cpp						Sources)
ze_add_source(ZERNCuller.h							Sources Headers)
ze_add_source(ZERNCommand.cpp 						Sources)
ze_add_source(ZERNCommand.h 						Sources Headers)
ze_add_source(ZERNView.cpp							Sources)
ze_add_source(ZERNView.h							Sources Headers)
ze_add_source(ZERNDrawParameters.cpp				Sources)
ze_add_source(ZERNDrawParameters.h					Sources Headers)
ze_add_source(ZEDrawStatistics.cpp					Sources)
ze_add_source(ZEDrawStatistics.h					Sources Headers)

ze_add_source(ZELight.cpp 							Sources)
ze_add_source(ZELight.h 							Sources Headers ZEMC)
ze_add_source(ZEPointLight.cpp 						Sources)
ze_add_source(ZEPointLight.h 						Sources Headers ZEMC)
ze_add_source(ZEProjectiveLight.cpp 				Sources)
ze_add_source(ZEProjectiveLight.h 					Sources Headers ZEMC)
ze_add_source(ZEDirectionalLight.cpp	 			Sources)
ze_add_source(ZEDirectionalLight.h					Sources Headers ZEMC)
ze_add_source(ZEOmniProjectiveLight.cpp 			Sources)
ze_add_source(ZEOmniProjectiveLight.h 				Sources Headers ZEMC)

ze_add_source(ZERNSampler.cpp 						Sources)
ze_add_source(ZERNSampler.h 						Sources Headers ZEMC)
ze_add_source(ZERNMaterial.cpp 						Sources)
ze_add_source(ZERNMaterial.h 						Sources Headers ZEMC)
ze_add_source(ZERNSimpleMaterial.cpp 				Sources)
ze_add_source(ZERNSimpleMaterial.h 					Sources Headers)
ze_add_source(ZEFixedMaterial.cpp 					Sources)
ze_add_source(ZEFixedMaterial.h 					Sources Headers ZEMC)
ze_add_source(ZEMaterialResource.cpp 				Sources)
ze_add_source(ZEMaterialResource.h 					Sources Headers)

ze_add_source(ZERNStage.h 							Sources Headers)
ze_add_source(ZERNStage.cpp 						Sources)
ze_add_source(ZERNStageManager.h 					Sources Headers)
ze_add_source(ZERNStageManager.cpp 					Sources)
ze_add_source(ZERNStageGBuffer.h 					Sources Headers)
ze_add_source(ZERNStageGBuffer.cpp 					Sources)
ze_add_source(ZERNStageAO.h 						Sources Headers)
ze_add_source(ZERNStageAO.cpp 						Sources)
ze_add_source(ZERNStageLighting.h 					Sources Headers)
ze_add_source(ZERNStageLighting.cpp					Sources)
ze_add_source(ZERNStageHDR.h 						Sources Headers)
ze_add_source(ZERNStageHDR.cpp 						Sources)
#ze_add_source(ZERNStageForward.h 					Sources Headers)
#ze_add_source(ZERNStageForward.cpp 					Sources)
ze_add_source(ZESortedTree.h 					Sources Headers)
ze_add_source(ZESortedTree.cpp 					Sources)

ze_add_library(TARGET ZERenderer 
	SOURCES ${Sources}
	HEADERS ${Headers}
	ZEMC ${ZEMC}
	LIBS ZEFoundation libFreeImage)
