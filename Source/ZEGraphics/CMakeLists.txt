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

add_source(ZEBitmap.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEBitmap.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECamera.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECamera.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECanvas.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZECanvas.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEDirectionalLight.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEDirectionalLight.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEFixedMaterial.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEFixedMaterial.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEFrameRenderer.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEFrameRenderer.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGraphicsModule.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEGraphicsModule.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEIndexedPolygon.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZELight.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZELight.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEMaterial.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEMaterial.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEMaterialComponents.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEMaterialFileFormat.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEMaterialResource.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEMaterialResource.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEOmniProjectiveLight.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEOmniProjectiveLight.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPointLight.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEPointLight.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEProjectiveLight.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEProjectiveLight.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZERenderer.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZERenderer.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZERenderOrder.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZERenderOrder.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEShadowRenderer.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEShadowRenderer.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESimpleMaterial.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESimpleMaterial.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESkyBoxMaterial.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZESkyBoxMaterial.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture2D.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture2D.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture2DResource.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture2DResource.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture3D.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture3D.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture3DResource.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETexture3DResource.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETextureCube.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETextureCube.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETextureCubeResource.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETextureCubeResource.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETextureResource.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZETextureResource.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEUIMaterial.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEUIMaterial.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEVertexBuffer.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEVertexBuffer.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEVertexDeclaration.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEVertexDeclaration.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEVertexTypes.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEVertexTypes.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEViewPort.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEViewPort.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEViewVolume.cpp ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZEViewVolume.h ZEGraphics_SOURCE_FILES INCLUDED_SDKS zesdk)

add_definitions(-DFREEIMAGE_LIB)
add_library(ZEGraphics ${ZEGraphics_SOURCE_FILES})
target_link_libraries(ZEGraphics freeimage.lib)
