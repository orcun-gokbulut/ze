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

include_directories(
	${ZEBUILD_INCLUDE_PATH}/GL
	${ZEBUILD_INCLUDE_PATH}/Cg)

link_directories(
	${ZEBUILD_LIB_PATH}/GL)

ze_add_source(ZEGL.h								Sources)
ze_add_source(ZEGLContextInfo.cpp					Sources)
ze_add_source(ZEGLContextInfo.h						Sources)
ze_add_source(ZEGLDepthBuffer.cpp					Sources)
ze_add_source(ZEGLDepthBuffer.h						Sources)
ze_add_source(ZEGLGraphicsDevice.cpp				Sources)
ze_add_source(ZEGLGraphicsDevice.h					Sources)
ze_add_source(ZEGLIndexBuffer.cpp					Sources)
ze_add_source(ZEGLIndexBuffer.h						Sources)
ze_add_source(ZEGLRenderTarget.cpp					Sources)
ze_add_source(ZEGLRenderTarget.h					Sources)
ze_add_source(ZEGLShader.cpp						Sources)
ze_add_source(ZEGLShader.h							Sources)
ze_add_source(ZEGLTexture2D.cpp						Sources)
ze_add_source(ZEGLTexture2D.h						Sources)
ze_add_source(ZEGLTexture3D.cpp						Sources)
ze_add_source(ZEGLTexture3D.h						Sources)
ze_add_source(ZEGLTextureCube.cpp					Sources)
ze_add_source(ZEGLTextureCube.h						Sources)
ze_add_source(ZEGLVertexBuffer.cpp					Sources)
ze_add_source(ZEGLVertexBuffer.h					Sources)

ze_add_library(ZEOpenGL 
	SOURCES ${Sources}
	LIBS glew32s OpenGL32 cg cgGL)
