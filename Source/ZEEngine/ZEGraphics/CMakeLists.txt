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
ze_add_source(ZEGRGraphicsModule.h 			Sources Headers ZEMC)
ze_add_source(ZEGRContext.cpp				Sources)
ze_add_source(ZEGRContext.h					Sources Headers ZEMC)
ze_add_source(ZEGRRenderState.cpp			Sources)
ze_add_source(ZEGRRenderState.h				Sources Headers ZEMC)

ze_add_source(ZEGRDefinitions.cpp			Sources)
ze_add_source(ZEGRDefinitions.h				Sources Headers ZEMC)

ze_add_source(ZEGRState.cpp					Sources)
ze_add_source(ZEGRState.h					Sources Headers ZEMC)
ze_add_source(ZEGRBlendState.cpp			Sources)
ze_add_source(ZEGRBlendState.h				Sources Headers ZEMC)
ze_add_source(ZEGRDepthStencilState.cpp		Sources)
ze_add_source(ZEGRDepthStencilState.h		Sources Headers ZEMC)
ze_add_source(ZEGRRasterizerState.cpp 		Sources)
ze_add_source(ZEGRRasterizerState.h 		Sources Headers ZEMC)
ze_add_source(ZEGRVertexLayout.cpp			Sources)
ze_add_source(ZEGRVertexLayout.h			Sources Headers ZEMC)
ze_add_source(ZEGRVertexElementType.cpp		Sources)
ze_add_source(ZEGRVertexElementType.h		Sources Headers ZEMC)

ze_add_source(ZEGRResource.cpp				Sources)
ze_add_source(ZEGRResource.h				Sources Headers ZEMC)

ze_add_source(ZEGRSampler.cpp 				Sources)
ze_add_source(ZEGRSampler.h 				Sources Headers ZEMC)

ze_add_source(ZEGRBuffer.cpp				Sources)
ze_add_source(ZEGRBuffer.h					Sources Headers ZEMC)

ze_add_source(ZEGRTexture.cpp 				Sources)
ze_add_source(ZEGRTexture.h 				Sources Headers ZEMC)

ze_add_Source(ZEGRFormat.cpp				Sources)
ze_add_Source(ZEGRFormat.h					Sources Headers ZEMC)

ze_add_source(ZEGRDepthStencilBuffer.cpp	Sources)
ze_add_source(ZEGRDepthStencilBuffer.h		Sources Headers ZEMC)
ze_add_source(ZEGRRenderTarget.cpp 			Sources)
ze_add_source(ZEGRRenderTarget.h 			Sources Headers ZEMC)

ze_add_source(ZEGRScissorRect.cpp			Sources)
ze_add_source(ZEGRScissorRect.h				Sources Headers ZEMC)
ze_add_source(ZEGRViewport.cpp				Sources)
ze_add_source(ZEGRViewport.h				Sources Headers ZEMC)

ze_add_source(ZEGRShader.cpp 				Sources)
ze_add_source(ZEGRShader.h 					Sources Headers ZEMC)
ze_add_source(ZEGRShaderMeta.cpp			Sources)
ze_add_source(ZEGRShaderMeta.h				Sources Headers ZEMC)
ze_add_source(ZEGRShaderCompiler.cpp		Sources)
ze_add_source(ZEGRShaderCompiler.h			Sources Headers ZEMC)
ze_add_source(ZEGRShaderCompileOptions.cpp	Sources)
ze_add_source(ZEGRShaderCompileOptions.h	Sources Headers ZEMC)

ze_add_source(ZEGRAdapter.h					Sources Headers ZEMC)
ze_add_source(ZEGRAdapter.cpp				Sources)
ze_add_source(ZEGROutput.h					Sources Headers ZEMC)
ze_add_source(ZEGROutput.cpp				Sources)

ze_add_source(ZEGRWindow.h					Sources Headers ZEMC)
ze_add_source(ZEGRWindow.cpp				Sources)
ze_add_source(ZEGRWindow_Unix.cpp			Sources PLATFORMS Unix)
ze_add_source(ZEGRWindow_Windows.cpp		Sources PLATFORMS Windows)

ze_add_source(ZEGRCounter.cpp				Sources)
ze_add_source(ZEGRCounter.h					Sources Headers)
ze_add_source(ZEGRTracer.cpp				Sources)
ze_add_source(ZEGRTracer.h					Sources Headers)
ze_add_source(ZEGRTracerEvent.cpp			Sources)
ze_add_source(ZEGRTracerEvent.h				Sources Headers)

ze_add_source(ZEGRCanvas.cpp 				Sources)
ze_add_source(ZEGRCanvas.h 					Sources Headers)

ze_add_library(TARGET ZEGraphics 
	SOURCES ${Sources}
	HEADERS ${Headers}
	ZEMC ${ZEMC}
	LIBS ZEFoundation libFreeImage libDirectXTex)
