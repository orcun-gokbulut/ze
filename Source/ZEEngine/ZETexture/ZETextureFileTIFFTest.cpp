//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileTIFFTest.cpp
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
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZETest/ZETest.h"
#include "ZEDS/ZEPointer.h"
#include "ZEDS/ZEString.h"
#include "ZEFile/ZEFile.h"
#include "ZETextureData.h"
#include "ZETextureFileTIFF.h"
#include "ZETextureTestUtils.h"
#include "ZETextureLoader.h"

static bool TestSuccess(ZEString FileName)
{
	ZEFile File;
	File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);

	ZETextureFileTIFF Loader;
	ZETextureDataInfo Info;
	if (!Loader.LoadInfo(&Info, &File))
		return false;

	ZETextureData* Data = Loader.Load(&File);

	if (Data == NULL)
		return false;

	return ZETextureTestUtils::Compare(Data, FileName + ".zeTexture");
}

static bool TestFail(ZEString FileName)
{
	ZEFile File;
	File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);

	ZEFile File;
	File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);

	ZETextureFileTIFF Loader;
	ZETextureData* Data = Loader.Load(&File);

	ZELog::GetInstance()->SetMinimumLogLevel(OldLogType);

	return Data == NULL;
}

ZETestSuite(ZETextureFileTIFFTest)
{
	ZETest("Valid Samples")
	{
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/INDEXED_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/INDEXED_LZW.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/L16_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/L8_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/L8_NONE_TILE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/L8_RLE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/LA16_BE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/LA16_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/LA8_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGB16_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGB8_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGB8_LZW.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGB8_LZW_PLANAR.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGB8_LZW_TILE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGB8_NONE_TILE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGBA16_BE_DEFLATE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGBA16_BE_LZW.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGBA16_BE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGBA16_LE_DEFLATE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGBA16_LE_LZW.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGBA16_LE_NONE.tif"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTIFF/Supported/RGBA8_LE_NONE.tif"));

	}

	ZETest("Ilvalid Samples")
	{
		ZETestCheck(TestFail("Non Existent File"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/HALFTONE_1.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/HALFTONE_2.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/HALFTONE_3.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/HALFTONE_4.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/L1.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/L1_2.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/L4.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/YCbCr_JPEG.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/YCbCr_JPEG_2.tif"));
		ZETestCheck(TestFail("TestResources/ZETextureFileTIFF/Unsupported/YCbCr_LZW.tif"));
	}
}
