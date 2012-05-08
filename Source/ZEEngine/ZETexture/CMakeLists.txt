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

ze_add_source(ZEBitmap.cpp						Sources)
ze_add_source(ZEBitmap.h						Sources Headers)
ze_add_source(ZETextureResource.cpp				Sources)
ze_add_source(ZETextureResource.h				Sources Headers)
ze_add_source(ZETexture2DResource.cpp			Sources)
ze_add_source(ZETexture2DResource.h				Sources Headers)
ze_add_source(ZETexture3DResource.cpp			Sources)
ze_add_source(ZETexture3DResource.h				Sources Headers)
ze_add_source(ZETextureCubeResource.cpp			Sources)
ze_add_source(ZETextureCubeResource.h			Sources Headers)
ze_add_source(ZETextureCacheDataIdentifier.cpp	Sources)
ze_add_source(ZETextureCacheDataIdentifier.h	Sources Headers)
ze_add_source(ZETextureData.cpp					Sources)
ze_add_source(ZETextureData.h				    Sources Headers)
ze_add_source(ZETextureFile.cpp			  		Sources)
ze_add_source(ZETextureFile.h				    Sources Headers)
ze_add_source(ZETextureFileBMP.cpp				Sources)
ze_add_source(ZETextureFileBMP.h			  	Sources Headers)
ze_add_source(ZETextureFileBMPTest.cpp			Tests)
ze_add_source(ZETextureFileFormat.cpp			Sources)
ze_add_source(ZETexturePixelConverter.h			Sources Headers)
ze_add_source(ZETexturePixelConverter.cpp		Sources)
ze_add_source(ZETextureFileFormat.h				Sources Headers)
ze_add_source(ZETextureFileJPEG.cpp				Sources)
ze_add_source(ZETextureFileJPEG.h			  	Sources Headers)
ze_add_source(ZETextureFileTGA.cpp				Sources)
ze_add_source(ZETextureFileTGA.h			  	Sources Headers)
#ze_add_source(ZETextureFileTGATest.cpp			Tests)
ze_add_source(ZETextureFileTIFF.cpp				Sources)
ze_add_source(ZETextureFileTIFF.h			  	Sources Headers)
#ze_add_source(ZETextureFileTGATest.cpp			Tests)
ze_add_source(ZETextureLoader.cpp			  	Sources)
ze_add_source(ZETextureLoader.h					Sources Headers)
ze_add_source(ZETextureOptions.cpp				Sources)
ze_add_source(ZETextureOptions.h				Sources Headers)
ze_add_source(ZETextureQualityManager.cpp		Sources)
ze_add_source(ZETextureQualityManager.h			Sources Headers)
ze_add_source(ZETextureTools.cpp				Sources)
ze_add_source(ZETextureTools.h					Sources Headers)


ze_add_library(ZETexture 
	SOURCES ${Sources}
	HEADERS ${Headers}
	LIBS ZEDS libATICompress libfreeimage
	INSTALL
	INSTALL_DESTINATION "ZEEngineAPI/ZETexture"
	INSTALL_COMPONENT Header)

ze_add_test(ZETextureTests 
	SOURCES ${Tests}
	TEST_TARGET ZETexture)