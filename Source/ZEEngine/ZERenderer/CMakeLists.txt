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

ze_add_source(ZECamera.cpp					Sources)
ze_add_source(ZECamera.h					Sources Header)
ze_add_source(ZECanvas.cpp					Sources)
ze_add_source(ZECanvas.h 					Sources Headers)
ze_add_source(ZELight.cpp 					Sources)
ze_add_source(ZELight.h 					Sources Header)
ze_add_source(ZEPointLight.cpp 				Sources)
ze_add_source(ZEPointLight.h 				Sources Header)
ze_add_source(ZEDirectionalLight.cpp	 	Sources)
ze_add_source(ZEDirectionalLight.h			Sources Header)
ze_add_source(ZEProjectiveLight.cpp 		Sources)
ze_add_source(ZEProjectiveLight.h 			Sources Header)
ze_add_source(ZEOmniProjectiveLight.cpp 	Sources)
ze_add_source(ZEOmniProjectiveLight.h 		Sources Header)
ze_add_source(ZEMaterial.cpp 				Sources)
ze_add_source(ZEMaterial.h 					Sources Header ZEPP)
ze_add_source(ZEUIMaterial.cpp 				Sources)
ze_add_source(ZEUIMaterial.h 				Sources Headers)
ze_add_source(ZESimpleMaterial.cpp 			Sources)
ze_add_source(ZESimpleMaterial.h 			Sources Header)
ze_add_source(ZEFixedMaterial.cpp 			Sources)
ze_add_source(ZEFixedMaterial.h 			Sources Header ZEPP)
#ze_add_source(ZESkyBoxMaterial.cpp 		Sources)
#ze_add_source(ZESkyBoxMaterial.h 			Sources Header)
#ze_add_source(ZESkyDomeMaterial.cpp 		Sources)
#ze_add_source(ZESkyDomeMaterial.h 			Sources Header)
#ze_add_source(ZETerrainMaterial.cpp		Sources)
#ze_add_source(ZETerrainMaterial.h 			Sources Headers)
#ze_add_source(ZECloudMaterial.cpp			Sources)
#ze_add_source(ZECloudMaterial.h 			Sources Headers)
#ze_add_source(ZEDecalMaterial.cpp			Sources)
#ze_add_source(ZEDecalMaterial.h 			Sources Headers)
#ze_add_source(ZEMoonMaterial.cpp 			Sources)
#ze_add_source(ZEMoonMaterial.h 			Sources Header)
ze_add_source(ZEMaterialResource.cpp 		Sources)
ze_add_source(ZEMaterialResource.h 			Sources Header)
ze_add_source(ZERenderCommand.cpp 			Sources)
ze_add_source(ZERenderCommand.h 			Sources Headers)
ze_add_source(ZERenderer.cpp 				Sources)
ze_add_source(ZERenderer.h 					Sources Headers)
ze_add_source(ZERenderStage.cpp				Sources)
ze_add_source(ZERenderStage.h				Sources Headers)
ze_add_source(ZERenderStageGeometry.cpp		Sources)
ze_add_source(ZERenderStageGeometry.h		Sources Headers)
ze_add_source(ZERenderStageLighting.cpp		Sources)
ze_add_source(ZERenderStageLighting.h		Sources Headers)
ze_add_source(ZERenderStageForward.cpp		Sources)
ze_add_source(ZERenderStageForward.h		Sources Headers)
ze_add_source(ZERenderStageShadow.cpp		Sources)
ze_add_source(ZERenderStageShadow.h			Sources Headers)
ze_add_source(ZERenderStageTransparent.cpp	Sources)
ze_add_source(ZERenderStageTransparent.h	Sources Headers)
ze_add_source(ZERenderStagePostProcess.cpp	Sources)
ze_add_source(ZERenderStagePostProcess.h	Sources Headers)


ze_add_library(ZERenderer
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEGraphics ZEFoundation ZEMeta ZEGame)
