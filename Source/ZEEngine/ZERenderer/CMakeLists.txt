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

ze_add_source(ZECamera.cpp						Sources)
ze_add_source(ZECamera.h						Sources Header)
ze_add_source(ZECanvas.cpp						Sources)
ze_add_source(ZECanvas.h 						Sources Headers)
ze_add_source(ZERenderer.cpp 					Sources)
ze_add_source(ZERenderer.h 						Sources Headers)
ze_add_source(ZERenderCommand.cpp 				Sources)
ze_add_source(ZERenderCommand.h 				Sources Headers)
ze_add_source(ZELight.cpp 						Sources)
ze_add_source(ZELight.h 						Sources Header)
ze_add_source(ZELightPoint.cpp 					Sources)
ze_add_source(ZELightPoint.h 					Sources Header)
ze_add_source(ZELightDirectional.cpp	 		Sources)
ze_add_source(ZELightDirectional.h				Sources Header)
ze_add_source(ZELightProjective.cpp 			Sources)
ze_add_source(ZELightProjective.h 				Sources Header)
ze_add_source(ZELightOmniProjective.cpp 		Sources)
ze_add_source(ZELightOmniProjective.h 			Sources Header)
ze_add_source(ZERenderStage.cpp					Sources)
ze_add_source(ZERenderStage.h					Sources Headers)
ze_add_source(ZERenderStageGeometry.cpp			Sources)
ze_add_source(ZERenderStageGeometry.h			Sources Headers)
ze_add_source(ZERenderStageLighting.cpp			Sources)
ze_add_source(ZERenderStageLighting.h			Sources Headers)
ze_add_source(ZERenderStageForward.cpp			Sources)
ze_add_source(ZERenderStageForward.h			Sources Headers)
ze_add_source(ZERenderStageShadow.cpp			Sources)
ze_add_source(ZERenderStageShadow.h				Sources Headers)
ze_add_source(ZERenderStageTransparent.cpp		Sources)
ze_add_source(ZERenderStageTransparent.h		Sources Headers)
ze_add_source(ZERenderStagePostProcess.cpp		Sources)
ze_add_source(ZERenderStagePostProcess.h		Sources Headers)
ze_add_source(ZEMaterial.cpp 					Sources)
ze_add_source(ZEMaterial.h 						Sources Header ZEPP)
ze_add_source(ZEMaterialDefault.cpp 			Sources)
ze_add_source(ZEMaterialDefault.h 				Sources Header ZEPP)
ze_add_source(ZEMaterialSimple.cpp 				Sources)
ze_add_source(ZEMaterialSimple.h 				Sources Header)
ze_add_source(ZEMaterialUserInterface.cpp 		Sources)
ze_add_source(ZEMaterialUserInterface.h 		Sources Headers)
ze_add_source(ZEMaterialLight.cpp 				Sources)
ze_add_source(ZEMaterialLight.h 				Sources Header)
ze_add_source(ZEMaterialDirectionalLight.cpp	Sources)
ze_add_source(ZEMaterialDirectionalLight.h 		Sources Header)
ze_add_source(ZEMaterialPointLight.cpp 			Sources)
ze_add_source(ZEMaterialPointLight.h 			Sources Header)
ze_add_source(ZEMaterialProjectiveLight.cpp 	Sources)
ze_add_source(ZEMaterialProjectiveLight.h 		Sources Header)
ze_add_source(ZEMaterialOmniProjectiveLight.cpp	Sources)
ze_add_source(ZEMaterialOmniProjectiveLight.h 	Sources Header)
#ze_add_source(ZESkyBoxMaterial.cpp 			Sources)
#ze_add_source(ZESkyBoxMaterial.h 				Sources Header)
#ze_add_source(ZESkyDomeMaterial.cpp 			Sources)
#ze_add_source(ZESkyDomeMaterial.h 				Sources Header)
#ze_add_source(ZETerrainMaterial.cpp			Sources)
#ze_add_source(ZETerrainMaterial.h 				Sources Headers)
#ze_add_source(ZECloudMaterial.cpp				Sources)
#ze_add_source(ZECloudMaterial.h 				Sources Headers)
#ze_add_source(ZEDecalMaterial.cpp				Sources)
#ze_add_source(ZEDecalMaterial.h 				Sources Headers)
#ze_add_source(ZEMoonMaterial.cpp 				Sources)
#ze_add_source(ZEMoonMaterial.h 				Sources Header)

ze_add_library(ZERenderer
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEGraphics ZEFoundation ZEMeta)
