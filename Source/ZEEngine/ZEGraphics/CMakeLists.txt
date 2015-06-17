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

ze_add_source(ZEGraphicsModule.cpp 			Sources)
ze_add_source(ZEGraphicsModule.h 			Sources Headers)
ze_add_source(ZEGraphicsDevice.cpp			Sources)
ze_add_source(ZEGraphicsDevice.h			Sources Headers)
ze_add_source(ZEGraphicsDeviceState.cpp		Sources)
ze_add_source(ZEGraphicsDeviceState.h		Sources Headers)
ze_add_source(ZEGRStatistics.cpp			Source)
ze_add_source(ZEGRStatistics.h				Source Headers)

ze_add_source(ZEGraphicsDefinitions.cpp		Sources)
ze_add_source(ZEGraphicsDefinitions.h		Sources Headers)

ze_add_source(ZEGRState.cpp					Sources)
ze_add_source(ZEGRState.h					Sources Headers)
ze_add_source(ZEBlendState.cpp				Sources)
ze_add_source(ZEBlendState.h				Sources Headers)
ze_add_source(ZEDepthStencilState.cpp		Sources)
ze_add_source(ZEDepthStencilState.h			Sources Headers)
ze_add_source(ZESamplerState.cpp 			Sources)
ze_add_source(ZESamplerState.h 				Sources Headers)
ze_add_source(ZERasterizerState.cpp 		Sources)
ze_add_source(ZERasterizerState.h 			Sources Headers)
ze_add_source(ZEVertexLayout.cpp			Sources)
ze_add_source(ZEVertexLayout.h				Sources Headers)

ze_add_source(ZEGraphicsResource.cpp		Sources)
ze_add_source(ZEGraphicsResource.h			Sources Headers)
ze_add_source(ZEIndexBuffer.cpp 			Sources)
ze_add_source(ZEIndexBuffer.h 				Sources Headers)
ze_add_source(ZEVertexBuffer.cpp 			Sources)
ze_add_source(ZEVertexBuffer.h 				Sources Headers)
ze_add_source(ZEConstantBuffer.cpp			Sources)
ze_add_source(ZEConstantBuffer.h			Sources Headers)
ze_add_source(ZETexture.cpp 				Sources)
ze_add_source(ZETexture.h 					Sources Headers)
ze_add_source(ZETexture2D.cpp				Sources)
ze_add_source(ZETexture2D.h 				Sources Headers)
ze_add_source(ZETexture3D.cpp 				Sources)
ze_add_source(ZETexture3D.h 				Sources Headers)
ze_add_source(ZETextureCube.cpp 			Sources)
ze_add_source(ZETextureCube.h 				Sources Headers)

ze_add_source(ZEDepthStencilBuffer.cpp		Sources)
ze_add_source(ZEDepthStencilBuffer.h		Sources Headers)
ze_add_source(ZERenderTarget.cpp 			Sources)
ze_add_source(ZERenderTarget.h 				Sources Headers)

ze_add_source(ZEScissorRectangle.cpp		Sources)
ze_add_source(ZEScissorRectangle.h			Sources Headers)
ze_add_source(ZEViewPort.cpp				Sources)
ze_add_source(ZEViewPort.h					Sources Headers)

ze_add_source(ZEGraphicsEvent.cpp			Sources)
ze_add_source(ZEGraphicsEvent.h				Sources Headers)

ze_add_source(ZEShader.cpp 					Sources)
ze_add_source(ZEShader.h 					Sources Headers)
ze_add_source(ZEShaderMeta.cpp				Sources)
ze_add_source(ZEShaderMeta.h				Sources Headers)
ze_add_source(ZEShaderCompiler.cpp			Sources)
ze_add_source(ZEShaderCompiler.h			Sources Headers)
ze_add_source(ZEShaderCompileOptions.cpp	Sources)
ze_add_source(ZEShaderCompileOptions.h		Sources Headers)

ze_add_source(ZEGraphicsMonitor.h			Sources Headers)
ze_add_source(ZEGraphicsMonitor.cpp			Sources)
ze_add_source(ZEGraphicsWindow.h			Sources Headers)
ze_add_source(ZEGraphicsWindow.cpp			Sources)
ze_add_source(ZEGraphicsWindow_Unix.cpp		Sources PLATFORMS Unix)
ze_add_source(ZEGraphicsWindow_Windows.cpp	Sources PLATFORMS Windows)

ze_add_library(TARGET ZEGraphics 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEFoundation libFreeImage)
