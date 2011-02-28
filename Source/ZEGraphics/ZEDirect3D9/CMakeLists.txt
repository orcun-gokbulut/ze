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

#add_definitions (-DZE_DEBUG_D3D9_PERFHUD -DZE_DEBUG_D3D9_SHADERS)

add_subdirectory (ZED3D9PostProcessor)

add_source (ZED3D9Common.cpp				Sources)
add_source (ZED3D9Common.h					Sources)
add_source (ZED3D9CommonTools.cpp			Sources)
add_source (ZED3D9CommonTools.h				Sources)
add_source (ZED3D9ComponentBase.cpp			Sources)
add_source (ZED3D9ComponentBase.h			Sources)
add_source (ZED3D9FixedMaterial.cpp			Sources)
add_source (ZED3D9FixedMaterial.h			Sources)
add_source (ZED3D9FrameRenderer.cpp			Sources)
add_source (ZED3D9FrameRenderer.h			Sources)
add_source (ZED3D9HDRProcessor.cpp			Sources)
add_source (ZED3D9HDRProcessor.h			Sources)
add_source (ZED3D9Module.cpp				Sources)
add_source (ZED3D9Module.h					Sources)
add_source (ZED3D9ModuleDescription.cpp		Sources)
add_source (ZED3D9ModuleDescription.h		Sources)
add_source (ZED3D9Profiler.cpp				Sources)
add_source (ZED3D9Profiler.h				Sources)
add_source (ZED3D9Shader.cpp				Sources)
add_source (ZED3D9Shader.h					Sources)
add_source (ZED3D9ShaderManager.cpp			Sources)
add_source (ZED3D9ShaderManager.h			Sources)
add_source (ZED3D9ShadowRenderer.cpp		Sources)
add_source (ZED3D9ShadowRenderer.h			Sources)
add_source (ZED3D9SimpleMaterial.cpp		Sources)
add_source (ZED3D9SimpleMaterial.h			Sources)
add_source (ZED3D9SkyBoxMaterial.cpp		Sources)
add_source (ZED3D9SkyBoxMaterial.h			Sources)
add_source (ZED3D9SSAOProcessor.cpp			Sources)
add_source (ZED3D9SSAOProcessor.h			Sources)
add_source (ZED3D9Texture2D.cpp				Sources)
add_source (ZED3D9Texture2D.h				Sources)
add_source (ZED3D9Texture3D.cpp				Sources)
add_source (ZED3D9Texture3D.h				Sources)
add_source (ZED3D9TextureCube.cpp			Sources)
add_source (ZED3D9TextureCube.h				Sources)
add_source (ZED3D9UIMaterial.cpp			Sources)
add_source (ZED3D9UIMaterial.h				Sources)
add_source (ZED3D9VertexBuffer.cpp			Sources)
add_source (ZED3D9VertexBuffer.h			Sources)
add_source (ZED3D9VertexDeclaration.cpp		Sources)
add_source (ZED3D9VertexDeclaration.h		Sources)
add_source (ZED3D9ViewPort.cpp				Sources)
add_source (ZED3D9ViewPort.h				Sources)
 
add_library (ZEDirect3D9 ${Sources})
target_link_libraries (ZEDirect3D9 d3d9.lib d3dx9.lib)
