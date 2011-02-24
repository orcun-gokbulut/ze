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

#add_definitions(-DZE_DEBUG_D3D9_PERFHUD -DZE_DEBUG_D3D9_SHADERS)

add_subdirectory(ZED3D9PostProcessor)

add_source(ZED3D9Common.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Common.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9CommonTools.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9CommonTools.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ComponentBase.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ComponentBase.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9FixedMaterial.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9FixedMaterial.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9FrameRenderer.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9FrameRenderer.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9HDRProcessor.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9HDRProcessor.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Module.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Module.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ModuleDescription.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ModuleDescription.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Profiler.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Profiler.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Shader.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Shader.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ShaderManager.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ShaderManager.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ShadowRenderer.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ShadowRenderer.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9SimpleMaterial.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9SimpleMaterial.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9SkyBoxMaterial.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9SkyBoxMaterial.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9SSAOProcessor.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9SSAOProcessor.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Texture2D.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Texture2D.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Texture3D.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9Texture3D.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9TextureCube.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9TextureCube.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9UIMaterial.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9UIMaterial.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9VertexBuffer.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9VertexBuffer.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9VertexDeclaration.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9VertexDeclaration.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ViewPort.cpp ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)
add_source(ZED3D9ViewPort.h ZEDirect3D9_SOURCE_FILES INCLUDED_SDKS zesdk)

add_library(ZEDirect3D9 ${ZEDirect3D9_SOURCE_FILES})
target_link_libraries(ZEDirect3D9 d3d9.lib d3dx9.lib)
