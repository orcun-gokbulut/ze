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
	ZE_DEBUG_D3D10_DEBUG_SHADERS	ZE_DEBUG_D3D10_ENABLE_PERFHUD)

set(ZEBUILD_D3D10_ENABLE_PERFHUD FALSE CACHE BOOL "AT RELEASE CONFIGURATION; Enable PerfHud device in release mode.")
if(ZEBUILD_D3D10_ENABLE_PERFHUD)
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_RELEASE ZE_DEBUG_D3D10_ENABLE_PERFHUD)
endif()

set(ZEBUILD_D3D10_DEBUG_SHADERS_RELEASE TRUE CACHE BOOL "AT RELEASE CONFIGURATION; Compile shaders with debug mode.")
if(ZEBUILD_D3D9_DEBUG_SHADERS_RELEASE)
	ze_append_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_RELEASE ZE_DEBUG_D3D10_DEBUG_SHADERS)
endif()

ze_add_source(ZED11ComponentBase.cpp			Sources)
ze_add_source(ZED11ComponentBase.h				Sources)
ze_add_source(ZED11DepthStencilBuffer.cpp		Sources)
ze_add_source(ZED11DepthStencilBuffer.h			Sources)
ze_add_source(ZED11Tracer.cpp					Sources)
ze_add_source(ZED11Tracer.h						Sources)
ze_add_source(ZED11Context.cpp					Sources)
ze_add_source(ZED11Context.h					Sources)
ze_add_source(ZED11Module.cpp					Sources)
ze_add_source(ZED11Module.h						Sources)
ze_add_source(ZED11Adapter.cpp					Sources)
ze_add_source(ZED11Adapter.h					Sources)
ze_add_source(ZED11IndexBuffer.cpp				Sources)
ze_add_source(ZED11IndexBuffer.h				Sources)
ze_add_source(ZED11RenderTarget.cpp				Sources)
ze_add_source(ZED11RenderTarget.h				Sources)
ze_add_source(ZED11Shader.cpp					Sources)
ze_add_source(ZED11Shader.h						Sources)
ze_add_source(ZED11ShaderCompiler.cpp			Sources)
ze_add_source(ZED11ShaderCompiler.h				Sources)
ze_add_source(ZED11ConstantBuffer.cpp			Sources)
ze_add_source(ZED11ConstantBuffer.h				Sources)
ze_add_source(ZED11StatePool.cpp				Sources)
ze_add_source(ZED11State.h						Sources)
ze_add_source(ZED11State.cpp					Sources)
ze_add_source(ZED11StatePool.h					Sources)
ze_add_source(ZED11StatePool.cpp				Sources)
ze_add_source(ZED11RenderStateData.h			Sources)
ze_add_source(ZED11RenderStateData.cpp			Sources)
ze_add_source(ZED11Output.h						Sources)
ze_add_source(ZED11Output.cpp					Sources)
ze_add_source(ZED11Texture2D.cpp				Sources)
ze_add_source(ZED11Texture2D.h					Sources)
ze_add_source(ZED11Texture3D.cpp				Sources)
ze_add_source(ZED11Texture3D.h					Sources)
ze_add_source(ZED11TextureCube.cpp				Sources)
ze_add_source(ZED11TextureCube.h				Sources)
ze_add_source(ZED11VertexBuffer.cpp				Sources)
ze_add_source(ZED11VertexBuffer.h				Sources)

ze_add_library(TARGET ZEDirect3D11
	SOURCES ${Sources}
	LIBS d3d11 uuid dxgi D3dcompiler)
