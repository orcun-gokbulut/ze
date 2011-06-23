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

add_source (ZEBitmap.cpp							Sources)
add_source (ZEBitmap.h								Sources ZESDK)
add_source (ZECamera.cpp							Sources)
add_source (ZECamera.h								Sources ZESDK ZEPP)
add_source (ZECanvas.cpp							Sources)
add_source (ZECanvas.h 								Sources ZESDK)
add_source (ZEDirectionalLight.cpp 					Sources)
add_source (ZEDirectionalLight.h 					Sources ZESDK ZEPP)
add_source (ZEFixedMaterial.cpp 					Sources)
add_source (ZEFixedMaterial.h 						Sources ZESDK ZEPP)
add_source (ZEFrameRenderer.cpp 					Sources)
add_source (ZEFrameRenderer.h 						Sources ZESDK)
add_source (ZEGraphicsDebugModule.cpp				Sources)
add_source (ZEGraphicsDebugModule.h					Sources)
add_source (ZEGraphicsModule.cpp 					Sources)
add_source (ZEGraphicsModule.h 						Sources ZESDK ZEPP)
add_source (ZEIndexedPolygon.h 						Sources ZESDK)
add_source (ZELight.cpp 							Sources)
add_source (ZELight.h 								Sources ZESDK ZEPP)
add_source (ZEMaterial.cpp 							Sources)
add_source (ZEMaterial.h 							Sources ZESDK ZEPP)
add_source (ZEMaterialComponents.h 					Sources ZESDK)
add_source (ZEMaterialFileFormat.h 					Sources)
add_source (ZEMaterialResource.cpp 					Sources)
add_source (ZEMaterialResource.h 					Sources ZESDK ZEPP)
add_source (ZEOmniProjectiveLight.cpp 				Sources)
add_source (ZEOmniProjectiveLight.h 				Sources ZESDK ZEPP)
add_source (ZEPointLight.cpp 						Sources)
add_source (ZEPointLight.h 							Sources ZESDK ZEPP)
add_source (ZEProjectiveLight.cpp 					Sources)
add_source (ZEProjectiveLight.h 					Sources ZESDK ZEPP)
add_source (ZERenderer.cpp 							Sources)
add_source (ZERenderer.h 							Sources ZESDK)
add_source (ZERenderOrder.cpp 						Sources)
add_source (ZERenderOrder.h 						Sources ZESDK)
add_source (ZEShadowRenderer.cpp 					Sources)
add_source (ZEShadowRenderer.h 						Sources ZESDK)
add_source (ZESimpleMaterial.cpp 					Sources)
add_source (ZESimpleMaterial.h 						Sources ZESDK ZEPP)
add_source (ZESkyBoxMaterial.cpp 					Sources)
add_source (ZESkyBoxMaterial.h 						Sources ZESDK ZEPP)
add_source (ZETexture.cpp 							Sources)
add_source (ZETexture.h 							Sources ZESDK)
add_source (ZETexture2D.cpp 						Sources)
add_source (ZETexture2D.h 							Sources ZESDK)
add_source (ZETexture2DResource.cpp 				Sources)
add_source (ZETexture2DResource.h 					Sources ZESDK)
add_source (ZETexture3D.cpp 						Sources)
add_source (ZETexture3D.h 							Sources ZESDK)
add_source (ZETexture3DResource.cpp 				Sources)
add_source (ZETexture3DResource.h 					Sources ZESDK)
add_source (ZETextureCube.cpp 						Sources)
add_source (ZETextureCube.h 						Sources ZESDK)
add_source (ZETextureCubeResource.cpp 				Sources)
add_source (ZETextureCubeResource.h 				Sources ZESDK)
add_source (ZETextureResource.cpp 					Sources)
add_source (ZETextureResource.h 					Sources ZESDK)
add_source (ZEUIMaterial.cpp 						Sources)
add_source (ZEUIMaterial.h 							Sources ZESDK)
add_source (ZEVertexBuffer.cpp 						Sources)
add_source (ZEVertexBuffer.h 						Sources ZESDK)
add_source (ZEVertexDeclaration.cpp 				Sources)
add_source (ZEVertexDeclaration.h 					Sources ZESDK)
add_source (ZEVertexTypes.cpp 						Sources)
add_source (ZEVertexTypes.h 						Sources ZESDK)
add_source (ZEViewPort.cpp 							Sources)
add_source (ZEViewPort.h 							Sources ZESDK)
add_source (ZEViewVolume.cpp 						Sources)
add_source (ZEViewVolume.h 							Sources ZESDK)
add_source (ZETextureLoader.h						Sources)
add_source (ZETextureLoader.cpp						Sources)				
add_source (ZETextureTools.h						Sources)
add_source (ZETextureTools.cpp						Sources)
add_source (ZETextureCacheChunkIdentifier.h			Sources)
add_source (ZETextureCacheChunkIdentifier.cpp		Sources)


add_library (ZEGraphics ${Sources})
target_link_libraries (ZEGraphics ATI_Compress_MT_VC8.lib)
target_link_libraries (ZEGraphics freeimage.lib)


set_property(TARGET ZEGraphics PROPERTY FOLDER "Zinek Engine")

install (TARGETS ZEGraphics COMPONENT "Libraries" CONFIGURATIONS Release DESTINATION Lib)
install (FILES ${ZESDK}  COMPONENT "Includes" CONFIGURATIONS Release DESTINATION Include/ZEGraphics)
