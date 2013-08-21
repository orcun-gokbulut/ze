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

ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG
	ZE_DEBUG_D3D9_DEBUG_SHADERS	ZE_DEBUG_D3D9_ENABLE_PERFHUD)

set(ZEBUILD_D3D9_ENABLE_PERFHUD	                FALSE CACHE BOOL "AT RELEASE CONFIGURATION; Enable PerfHud device in release mode.")
if(ZEBUILD_D3D9_ENABLE_PERFHUD)
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_RELEASE ZE_DEBUG_D3D9_ENABLE_PERFHUD)
endif()

set(ZEBUILD_D3D9_DEBUG_SHADERS_RELEASE   TRUE CACHE BOOL "AT RELEASE CONFIGURATION; Compile shaders with debug mode.")
if(ZEBUILD_D3D9_DEBUG_SHADERS_RELEASE)
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_RELEASE ZE_DEBUG_D3D9_DEBUG_SHADERS)
endif()

add_subdirectory (ZED3D9PostProcessor)

ze_add_source(ZED3D9PixelWorldPositionProcessor.cpp		Sources)
ze_add_source(ZED3D9PixelWorldPositionProcessor.h		Sources)
ze_add_source(ZED3D9BlurMaskProcessor.cpp				Sources)
ze_add_source(ZED3D9BlurMaskProcessor.h					Sources)
ze_add_source(ZED3D9BlurProcessor.cpp					Sources)
ze_add_source(ZED3D9BlurProcessor.h						Sources)
ze_add_source(ZED3D9ChannelDisorientationProcessor.cpp	Sources)
ze_add_source(ZED3D9ChannelDisorientationProcessor.h	Sources)
ze_add_source(ZED3D9CloudMaterial.cpp					Sources)
ze_add_source(ZED3D9CloudMaterial.h						Sources)
ze_add_source(ZED3D9ColorTransformProcessor.cpp			Sources)
ze_add_source(ZED3D9ColorTransformProcessor.h			Sources)
ze_add_source(ZED3D9CommonTools.cpp						Sources)
ze_add_source(ZED3D9CommonTools.h						Sources)
ze_add_source(ZED3D9ComponentBase.cpp					Sources)
ze_add_source(ZED3D9ComponentBase.h						Sources)
ze_add_source(ZED3D9DOFProcessor.cpp					Sources)
ze_add_source(ZED3D9DOFProcessor.h						Sources ZEPP)
ze_add_source(ZED3D9EDProcessor.cpp						Sources)
ze_add_source(ZED3D9EDProcessor.h						Sources)
ze_add_source(ZED3D9FixedMaterial.cpp					Sources)
ze_add_source(ZED3D9FixedMaterial.h						Sources)
ze_add_source(ZED3D9FogProcessor.cpp					Sources)
ze_add_source(ZED3D9FogProcessor.h						Sources ZEPP)
ze_add_source(ZED3D9FrameRenderer.cpp					Sources)
ze_add_source(ZED3D9FrameRenderer.h						Sources)
ze_add_source(ZED3D9GrainProcessor.cpp					Sources)
ze_add_source(ZED3D9GrainProcessor.h					Sources)
ze_add_source(ZED3D9HDRProcessor.cpp					Sources)
ze_add_source(ZED3D9HDRProcessor.h						Sources ZEPP)
ze_add_source(ZED3D9IndexBuffer.cpp						Sources)
ze_add_source(ZED3D9IndexBuffer.h						Sources)
ze_add_source(ZED3D9MLAAProcessorAreaImage.cpp			Sources)
ze_add_source(ZED3D9MLAAProcessorAreaImage.h			Sources)
ze_add_source(ZED3D9MLAAProcessor.cpp					Sources)
ze_add_source(ZED3D9MLAAProcessor.h						Sources ZEPP)
ze_add_source(ZED3D9Module.cpp							Sources)
ze_add_source(ZED3D9Module.h							Sources)
ze_add_source(ZED3D9MoonMaterial.cpp					Sources)
ze_add_source(ZED3D9MoonMaterial.h						Sources)
ze_add_source(ZED3D9Profiler.cpp						Sources)
ze_add_source(ZED3D9Profiler.h							Sources)
ze_add_source(ZED3D9Shader.cpp							Sources)
ze_add_source(ZED3D9Shader.h							Sources)
ze_add_source(ZED3D9ShaderManager.cpp					Sources)
ze_add_source(ZED3D9ShaderManager.h						Sources)
ze_add_source(ZED3D9ShadowRenderer.cpp					Sources)
ze_add_source(ZED3D9ShadowRenderer.h					Sources)
ze_add_source(ZED3D9SimpleMaterial.cpp					Sources)
ze_add_source(ZED3D9SimpleMaterial.h					Sources)
ze_add_source(ZED3D9SkyBoxMaterial.cpp					Sources)
ze_add_source(ZED3D9SkyBoxMaterial.h					Sources)
ze_add_source(ZED3D9SkyDomeMaterial.cpp					Sources)
ze_add_source(ZED3D9SkyDomeMaterial.h					Sources)
ze_add_source(ZED3D9SSAOProcessor.cpp					Sources)
ze_add_source(ZED3D9SSAOProcessor.h						Sources ZEPP)
ze_add_source(ZED3D9HBAOProcessor.cpp					Sources)
ze_add_source(ZED3D9HBAOProcessor.h						Sources ZEPP)
ze_add_source(ZED3D9SSAAProcessor.cpp					Sources)
ze_add_source(ZED3D9SSAAProcessor.h						Sources)
ze_add_source(ZED3D9TerrainMaterial.cpp					Sources)
ze_add_source(ZED3D9TerrainMaterial.h					Sources)
ze_add_source(ZED3D9Texture2D.cpp						Sources)
ze_add_source(ZED3D9Texture2D.h							Sources)
ze_add_source(ZED3D9Texture3D.cpp						Sources)
ze_add_source(ZED3D9Texture3D.h							Sources)
ze_add_source(ZED3D9TextureCube.cpp						Sources)
ze_add_source(ZED3D9TextureCube.h						Sources)
ze_add_source(ZED3D9TextureMaskProcessor.cpp			Sources)
ze_add_source(ZED3D9TextureMaskProcessor.h				Sources)
ze_add_source(ZED3D9TextureResizeFilters.cpp			Sources)
ze_add_source(ZED3D9TextureResizeFilters.h				Sources)
ze_add_source(ZED3D9TextureResizer.cpp					Sources)
ze_add_source(ZED3D9TextureResizer.h					Sources)
ze_add_source(ZED3D9UIMaterial.cpp						Sources)
ze_add_source(ZED3D9UIMaterial.h						Sources)
ze_add_source(ZED3D9UnsharpenFilterProcessor.cpp		Sources)
ze_add_source(ZED3D9UnsharpenFilterProcessor.h			Sources)
ze_add_source(ZED3D9VertexBuffer.cpp					Sources)
ze_add_source(ZED3D9VertexBuffer.h						Sources)
ze_add_source(ZED3D9VertexDeclaration.cpp				Sources)
ze_add_source(ZED3D9VertexDeclaration.h					Sources)
ze_add_source(ZED3D9ViewPort.cpp						Sources)
ze_add_source(ZED3D9ViewPort.h							Sources)
ze_add_source(ZED3D9ZoomBlurProcessor.cpp				Sources)
ze_add_source(ZED3D9ZoomBlurProcessor.h					Sources)
ze_add_source(ZED3D9Plugin.h							Sources)
ze_add_source(ZED3D9Plugin.cpp							Sources)
ze_add_source(ZED3D9SeaMaterial.h						Sources)
ze_add_source(ZED3D9SeaMaterial.cpp						Sources)
	
ze_add_source(ZED3D9AerialPerspectiveProcessor.cpp		Sources)
ze_add_source(ZED3D9AerialPerspectiveProcessor.h		Sources ZEPP)

ze_add_source(Shaders/AerialPerspectiveProcessor.hlsl			Sources ZEFC)
ze_add_source(Shaders/BlurMaskProcessor.hlsl					Sources ZEFC)
ze_add_source(Shaders/BlurProcessor.hlsl						Sources ZEFC)
ze_add_source(Shaders/ChannelDisorientationProcessor.hlsl		Sources ZEFC)
ze_add_source(Shaders/CloudMaterial.hlsl						Sources ZEFC)
ze_add_source(Shaders/ColorTransformProcessor.hlsl				Sources ZEFC)
ze_add_source(Shaders/DOFProcessor.hlsl							Sources ZEFC)
ze_add_source(Shaders/DownSample2x.hlsl							Sources ZEFC)
ze_add_source(Shaders/EDProcessor.hlsl							Sources ZEFC)
ze_add_source(Shaders/FixedMaterial.hlsl						Sources ZEFC)
ze_add_source(Shaders/FixedMaterialComponents.hlsl				Sources ZEFC)
ze_add_source(Shaders/FogProcessor.hlsl							Sources ZEFC)
ze_add_source(Shaders/GBuffer.hlsl								Sources ZEFC)
ze_add_source(Shaders/GrainProcessor.hlsl						Sources ZEFC)
ze_add_source(Shaders/HBAOProcessor.hlsl						Sources ZEFC)
ze_add_source(Shaders/HDRProcessor.hlsl							Sources ZEFC)
ze_add_source(Shaders/LBuffer.hlsl								Sources ZEFC)
ze_add_source(Shaders/Lights.hlsl								Sources ZEFC)
ze_add_source(Shaders/MLAAProcessor.hlsl						Sources ZEFC)
ze_add_source(Shaders/MoonMaterial.hlsl							Sources ZEFC)
ze_add_source(Shaders/PixelWorldPositionProcessor.hlsl			Sources ZEFC)
ze_add_source(Shaders/SeaMaterial.hlsl							Sources ZEFC)
ze_add_source(Shaders/Shadow.hlsl								Sources ZEFC)
ze_add_source(Shaders/ShadowMaterial.hlsl						Sources ZEFC)
ze_add_source(Shaders/SimpleMaterial.hlsl						Sources ZEFC)
ze_add_source(Shaders/SkinTransform.hlsl						Sources ZEFC)
ze_add_source(Shaders/SkyBoxMaterial.hlsl						Sources ZEFC)
ze_add_source(Shaders/SkyDomeMaterial.hlsl						Sources ZEFC)
ze_add_source(Shaders/SSAAProcessor.hlsl						Sources ZEFC)
ze_add_source(Shaders/SSAOProcessor.hlsl						Sources ZEFC)
ze_add_source(Shaders/TerrainMaterial.hlsl						Sources ZEFC)
ze_add_source(Shaders/TextureMaskProcessor.hlsl					Sources ZEFC)
ze_add_source(Shaders/TextureResizeProcessor.hlsl				Sources ZEFC)
ze_add_source(Shaders/UI.hlsl									Sources ZEFC)
ze_add_source(Shaders/UnsharpenFilterProcessor.hlsl				Sources ZEFC)
ze_add_source(Shaders/ZoomBlurProcessor.hlsl					Sources ZEFC)

ze_add_library(TARGET ZEDirect3D9 
	SOURCES ${Sources}
	LIBS libDirect3D9)
