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

add_subdirectory(ZEDirect3D9)
add_subdirectory(ZEPostProcessor)

ze_add_source(ZEBitmap.cpp							Sources)
ze_add_source(ZEBitmap.h							Sources Headers)
ze_add_source(ZECamera.cpp							Sources)
ze_add_source(ZECamera.h							Sources Header ZEPP)
ze_add_source(ZECanvas.cpp							Sources)
ze_add_source(ZECanvas.h 							Sources Headers)
ze_add_source(ZEDirectionalLight.cpp	 			Sources)
ze_add_source(ZEDirectionalLight.h					Sources Header ZEPP)
ze_add_source(ZEFixedMaterial.cpp 					Sources)
ze_add_source(ZEFixedMaterial.h 					Sources Header ZEPP)
ze_add_source(ZEFrameRenderer.cpp 					Sources)
ze_add_source(ZEFrameRenderer.h 					Sources Headers)
ze_add_source(ZEGraphicsDebugModule.cpp				Sources)
ze_add_source(ZEGraphicsDebugModule.h				Sources)
ze_add_source(ZEGraphicsModule.cpp 					Sources)
ze_add_source(ZEGraphicsModule.h 					Sources Header ZEPP)
ze_add_source(ZEGraphicsModuleDescription.cpp		Sources)
ze_add_source(ZEGraphicsModuleDescription.h			Sources Header ZEPP)
ze_add_source(ZEIndexedPolygon.h 					Sources Headers)
ze_add_source(ZELight.cpp 							Sources)
ze_add_source(ZELight.h 							Sources Header ZEPP)
ze_add_source(ZEMaterial.cpp 						Sources)
ze_add_source(ZEMaterial.h 							Sources Header ZEPP)
ze_add_source(ZEMaterialComponents.h				Sources Headers)
ze_add_source(ZEMaterialFileFormat.h 				Sources)
ze_add_source(ZEMaterialResource.cpp 				Sources)
ze_add_source(ZEMaterialResource.h 					Sources Header ZEPP)
ze_add_source(ZEOmniProjectiveLight.cpp 			Sources)
ze_add_source(ZEOmniProjectiveLight.h 				Sources Header ZEPP)
ze_add_source(ZEPointLight.cpp 						Sources)
ze_add_source(ZEPointLight.h 						Sources Header ZEPP)
ze_add_source(ZEProjectiveLight.cpp 				Sources)
ze_add_source(ZEProjectiveLight.h 					Sources Header ZEPP)
ze_add_source(ZERenderer.cpp 						Sources)
ze_add_source(ZERenderer.h 							Sources Headers)
ze_add_source(ZERenderOrder.cpp 					Sources)
ze_add_source(ZERenderOrder.h 						Sources Headers)
ze_add_source(ZEShadowRenderer.cpp 					Sources)
ze_add_source(ZEShadowRenderer.h 					Sources Headers)
ze_add_source(ZESimpleMaterial.cpp 					Sources)
ze_add_source(ZESimpleMaterial.h 					Sources Header ZEPP)
ze_add_source(ZESkyBoxMaterial.cpp 					Sources)
ze_add_source(ZESkyBoxMaterial.h 					Sources Header ZEPP)
ze_add_source(ZETexture.cpp 						Sources)
ze_add_source(ZETexture.h 							Sources Headers)
ze_add_source(ZETexture2D.cpp						Sources)
ze_add_source(ZETexture2D.h 						Sources Headers)
ze_add_source(ZETexture2DResource.cpp		 		Sources)
ze_add_source(ZETexture2DResource.h					Sources Headers)
ze_add_source(ZETexture3D.cpp 						Sources)
ze_add_source(ZETexture3D.h 						Sources Headers)
ze_add_source(ZETexture3DResource.cpp				Sources)
ze_add_source(ZETexture3DResource.h					Sources Headers)
ze_add_source(ZETextureCube.cpp 					Sources)
ze_add_source(ZETextureCube.h 						Sources Headers)
ze_add_source(ZETextureCubeResource.cpp				Sources)
ze_add_source(ZETextureCubeResource.h 				Sources Headers)
ze_add_source(ZETextureResource.cpp					Sources)
ze_add_source(ZETextureResource.h 					Sources Headers)
ze_add_source(ZEUIMaterial.cpp 						Sources)
ze_add_source(ZEUIMaterial.h 						Sources Headers)
ze_add_source(ZEVertexBuffer.cpp 					Sources)
ze_add_source(ZEVertexBuffer.h 						Sources Headers)
ze_add_source(ZEVertexDeclaration.cpp				Sources)
ze_add_source(ZEVertexDeclaration.h					Sources Headers)
ze_add_source(ZEVertexTypes.cpp 					Sources)
ze_add_source(ZEVertexTypes.h 						Sources Headers)
ze_add_source(ZEViewPort.cpp 						Sources)
ze_add_source(ZEViewPort.h 							Sources Headers)
ze_add_source(ZEViewVolume.cpp 						Sources)
ze_add_source(ZEViewVolume.h 						Sources Headers)
ze_add_source(ZETextureLoader.h						Sources)
ze_add_source(ZETextureLoader.cpp					Sources)				
ze_add_source(ZETextureTools.h						Sources)
ze_add_source(ZETextureTools.cpp					Sources)
ze_add_source(ZETextureCacheChunkIdentifier.h		Sources)
ze_add_source(ZETextureCacheChunkIdentifier.cpp		Sources)

ze_add_library(ZEGraphics 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ATI_Compress_MT_VC8.lib freeimage.lib
	INSTALL
	INSTALL_DESTINATION "ZEGraphics"
	INSTALL_COMPONENT Header)
