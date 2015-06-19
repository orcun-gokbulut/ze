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

ze_add_source(ZEGRGraphicsModule.cpp 		Sources)
ze_add_source(ZEGRGraphicsModule.h 			Sources Headers)
ze_add_source(ZEGRDevice.cpp				Sources)
ze_add_source(ZEGRDevice.h					Sources Headers)
ze_add_source(ZEGRRenderState.cpp			Sources)
ze_add_source(ZEGRRenderState.h				Sources Headers)

ze_add_source(ZEGRDefinitions.cpp			Sources)
ze_add_source(ZEGRDefinitions.h				Sources Headers)

ze_add_source(ZEGRState.cpp					Sources)
ze_add_source(ZEGRState.h					Sources Headers)
ze_add_source(ZEGRBlendState.cpp			Sources)
ze_add_source(ZEGRBlendState.h				Sources Headers)
ze_add_source(ZEGRDepthStencilState.cpp		Sources)
ze_add_source(ZEGRDepthStencilState.h		Sources Headers)
ze_add_source(ZEGRSamplerState.cpp 			Sources)
ze_add_source(ZEGRSamplerState.h 			Sources Headers)
ze_add_source(ZEGRRasterizerState.cpp 		Sources)
ze_add_source(ZEGRRasterizerState.h 		Sources Headers)
ze_add_source(ZEGRVertexLayout.cpp			Sources)
ze_add_source(ZEGRVertexLayout.h			Sources Headers)

ze_add_source(ZEGRResource.cpp				Sources)
ze_add_source(ZEGRResource.h				Sources Headers)
ze_add_source(ZEGRHolder.cpp				Sources)
ze_add_source(ZEGRHolder.h					Sources Headers)

ze_add_source(ZEGRIndexBuffer.cpp 			Sources)
ze_add_source(ZEGRIndexBuffer.h 			Sources Headers)
ze_add_source(ZEGRVertexBuffer.cpp 			Sources)
ze_add_source(ZEGRVertexBuffer.h 			Sources Headers)
ze_add_source(ZEGRConstantBuffer.cpp		Sources)
ze_add_source(ZEGRConstantBuffer.h			Sources Headers)
ze_add_source(ZEGRTexture.cpp 				Sources)
ze_add_source(ZEGRTexture.h 				Sources Headers)
ze_add_source(ZEGRTexture2D.cpp				Sources)
ze_add_source(ZEGRTexture2D.h 				Sources Headers)
ze_add_source(ZEGRTexture3D.cpp 			Sources)
ze_add_source(ZEGRTexture3D.h 				Sources Headers)
ze_add_source(ZEGRTextureCube.cpp 			Sources)
ze_add_source(ZEGRTextureCube.h 			Sources Headers)

ze_add_source(ZEGRDepthStencilBuffer.cpp	Sources)
ze_add_source(ZEGRDepthStencilBuffer.h		Sources Headers)
ze_add_source(ZEGRRenderTarget.cpp 			Sources)
ze_add_source(ZEGRRenderTarget.h 			Sources Headers)

ze_add_source(ZEGRScissorRectangle.cpp		Sources)
ze_add_source(ZEGRScissorRectangle.h		Sources Headers)
ze_add_source(ZEGRViewPort.cpp				Sources)
ze_add_source(ZEGRViewPort.h				Sources Headers)

ze_add_source(ZEGRShader.cpp 				Sources)
ze_add_source(ZEGRShader.h 					Sources Headers)
ze_add_source(ZEGRShaderMeta.cpp			Sources)
ze_add_source(ZEGRShaderMeta.h				Sources Headers)
ze_add_source(ZEGRShaderCompiler.cpp		Sources)
ze_add_source(ZEGRShaderCompiler.h			Sources Headers)
ze_add_source(ZEGRShaderCompileOptions.cpp	Sources)
ze_add_source(ZEGRShaderCompileOptions.h	Sources Headers)

ze_add_source(ZEGRMonitor.h					Sources Headers)
ze_add_source(ZEGRMonitor.cpp				Sources)
ze_add_source(ZEGRWindow.h					Sources Headers)
ze_add_source(ZEGRWindow.cpp				Sources)
ze_add_source(ZEGRWindow_Unix.cpp			Sources PLATFORMS Unix)
ze_add_source(ZEGRWindow_Windows.cpp		Sources PLATFORMS Windows)

ze_add_source(ZEGRCounter.cpp				Sources)
ze_add_source(ZEGRCounter.h					Sources Headers)
ze_add_source(ZEGRTracer.cpp				Sources)
ze_add_source(ZEGRTracer.h					Sources Headers)
ze_add_source(ZEGRTracerEvent.cpp			Sources)
ze_add_source(ZEGRTracerEvent.h				Sources Headers)

ze_add_library(TARGET ZEGraphics 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEFoundation libFreeImage)
