//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileJPEGTest.cpp
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
#include "ZEBitmap.h"
#include "ZETextureData.h"
#include "ZETextureFileBMP.h"
#include "ZETextureFileJPEG.h"
#include "ZETextureLoader.h"
#include "ZETest/ZETestCheck.h"

#include <stdlib.h>

bool JPEGCompareImages(ZETextureData* TextureA, ZETextureData* TextureB)
{
	const ZETextureDataInfo* InfoA = &TextureA->GetInfo();
	const ZETextureDataInfo* InfoB = &TextureB->GetInfo();

	if (InfoA->LevelCount != InfoB->LevelCount || 
		InfoA->SurfaceCount != InfoB->SurfaceCount ||
		InfoA->Width != InfoB->Width || 
		InfoA->Height != InfoB->Height || 
		InfoA->Type != InfoB->Type || 
		InfoA->PixelFormat != InfoB->PixelFormat)
	{
		return false;
	}

	void* DataA = TextureA->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
	ZESize SizeA = TextureA->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetSize();
	void* DataB = TextureB->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetData();
	ZESize SizeB = TextureB->GetSurfaces().GetItem(0).GetLevels().GetItem(0).GetSize();
	
	if ( (SizeA != SizeB) || (memcmp(DataA, DataB, SizeA) != 0) )
		return false;

	return true;
}

bool JPEGTestSuccess(ZEString& ImageFile, ZEString& ReferanceFile)
{
	ZEFile OrgFile;
	ZETextureDataInfo Info;
	ZETextureFileJpeg Loader;
	ZETextureData OriginalData;

	ZEFile ImgFile;
	ImgFile.Open(ImageFile, ZE_FOM_READ, ZE_FCM_NONE);

	if (!Loader.LoadInfo(&Info, &ImgFile)) return false;

	ZETextureData* Data = Loader.Load(&ImgFile);
	if (Data == NULL) return false;

	OrgFile.Open(ReferanceFile, ZE_FOM_READ, ZE_FCM_NONE);
	if (!ZETextureLoader::Read(&OrgFile, &OriginalData)) return false;
	
	return JPEGCompareImages(Data, &OriginalData);
}

bool JPEGTestFail(ZEString FileName)
{
	ZEFile File;
	File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);

	ZETextureFileJpeg Loader;
	ZETextureData* Data = Loader.Load(&File);

	return Data == NULL;
}

ZETestSuite(ZETextureFileJPEGTest)
{
	ZETest("Valid")
	{
		ZETestCase("Supported")
		{
			ZELogType OldLogType = ZELog::GetInstance()->GetMinimumLogLevel();
			ZELog::GetInstance()->SetMinimumLogLevel(ZE_LOG_CRITICAL_ERROR);

			ZEString SupportedInputList[20];
			SupportedInputList[0]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_313x417.jpg";
			SupportedInputList[1]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_313x417_lowQ.jpg";
			SupportedInputList[2]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_600x800.jpg";
			SupportedInputList[3]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_800x533.jpg";
			SupportedInputList[4]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H2V2_800x533.jpeg";
			SupportedInputList[5]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_H1V1_H1V1_256x256.jpg";
			SupportedInputList[6]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_H1V1_H1V1_313x313.jpg";
			SupportedInputList[7]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_H1V1_H1V1_520x520.jpg";
			SupportedInputList[8]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_H1V1_H1V1_1024x768.jpg";
			SupportedInputList[9]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H2V1_H1V1_H1V1_1280x1000.JPG";
			SupportedInputList[10]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H2V1_H1V1_H1V1_2048x1536.jpg";
			SupportedInputList[11]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H2V2_H1V1_H1V1_520x520_lowQ.jpg";
			SupportedInputList[12]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H2V2_H1V1_H1V1_1024x768_lowQ.jpg";
			SupportedInputList[13]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H2V2_H1V1_H1V1_1438x900_lowQ.jpg";
			SupportedInputList[14]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H2V2_H1V1_H1V1_2048x1536.jpg";
			SupportedInputList[15]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_H1V1_H1V1_1024x1024_ICC_Optimized.jpg";
			SupportedInputList[16]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_H1V1_H1V1_1024x1024_nonICC_Optimized.jpg";
			SupportedInputList[17]		= "TestResources/ZETextureFileJPEG/Valid/Supported/H1V1_H1V1_H1V1_1024x1024_ICC.jpg";

			ZEString SupportedReferanceList[20];
			SupportedReferanceList[0]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_313x417.ZETexture";
			SupportedReferanceList[1]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_313x417_lowQ.ZETexture";
			SupportedReferanceList[2]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_600x800.ZETexture";
			SupportedReferanceList[3]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_800x533.ZETexture";
			SupportedReferanceList[4]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H2V2_800x533.ZETexture";
			SupportedReferanceList[5]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_H1V1_H1V1_256x256.ZETexture";
			SupportedReferanceList[6]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_H1V1_H1V1_313x313.ZETexture";
			SupportedReferanceList[7]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_H1V1_H1V1_520x520.ZETexture";
			SupportedReferanceList[8]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_H1V1_H1V1_1024x768.ZETexture";
			SupportedReferanceList[9]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H2V1_H1V1_H1V1_1280x1000.ZETexture";
			SupportedReferanceList[10]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H2V1_H1V1_H1V1_2048x1536.ZETexture";
			SupportedReferanceList[11]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H2V2_H1V1_H1V1_520x520_lowQ.ZETexture";
			SupportedReferanceList[12]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H2V2_H1V1_H1V1_1024x768_lowQ.ZETexture";
			SupportedReferanceList[13]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H2V2_H1V1_H1V1_1438x900_lowQ.ZETexture";
			SupportedReferanceList[14]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H2V2_H1V1_H1V1_2048x1536.ZETexture";
			SupportedReferanceList[15]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_H1V1_H1V1_1024x1024_ICC_Optimized.ZETexture";
			SupportedReferanceList[16]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_H1V1_H1V1_1024x1024_nonICC_Optimized.ZETexture";
			SupportedReferanceList[17]	= "TestResources/ZETextureFileJPEG/Valid/Supported/Referance_H1V1_H1V1_H1V1_1024x1024_ICC.ZETexture";

			// Gray Scale H1V1
			ZETestCheck(JPEGTestSuccess(SupportedInputList[0],  SupportedReferanceList[0]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[1],  SupportedReferanceList[1]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[2],  SupportedReferanceList[2]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[3],  SupportedReferanceList[3]));
			
			// Gray Scale H2V2 
			ZETestCheck(JPEGTestSuccess(SupportedInputList[4],  SupportedReferanceList[4]));

			// YCbCr H1V1 H1V1 H1V1
			ZETestCheck(JPEGTestSuccess(SupportedInputList[5],  SupportedReferanceList[5]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[6],  SupportedReferanceList[6]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[7],  SupportedReferanceList[7]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[8],  SupportedReferanceList[8]));

			// YCbCr H2V1 H1V1 H1V1
			ZETestCheck(JPEGTestSuccess(SupportedInputList[9],  SupportedReferanceList[9]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[10], SupportedReferanceList[10]));

			// YCbCr H2V2 H1V1 H1V1
			ZETestCheck(JPEGTestSuccess(SupportedInputList[11], SupportedReferanceList[11]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[12], SupportedReferanceList[12]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[13], SupportedReferanceList[13]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[14], SupportedReferanceList[14]));

			// Optimized & ICC
			ZETestCheck(JPEGTestSuccess(SupportedInputList[15], SupportedReferanceList[15]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[16], SupportedReferanceList[16]));
			ZETestCheck(JPEGTestSuccess(SupportedInputList[17], SupportedReferanceList[17]));

			ZELog::GetInstance()->SetMinimumLogLevel(OldLogType);
		}

		ZETestCase("Unsupported")
		{
			ZELogType OldLogType = ZELog::GetInstance()->GetMinimumLogLevel();
			ZELog::GetInstance()->SetMinimumLogLevel(ZE_LOG_CRITICAL_ERROR);

			ZEString UnsupportedInputList[20];
			UnsupportedInputList[0]  = "TestResources/ZETextureFileJPEG/Valid/Unsupported/Progressive1.jpg";
			UnsupportedInputList[1]  = "TestResources/ZETextureFileJPEG/Valid/Unsupported/Progressive2.jpg";
			
			// Progressive
			ZETestCheck(JPEGTestFail(UnsupportedInputList[0]));
			ZETestCheck(JPEGTestFail(UnsupportedInputList[1]));
			
			// H2V2 Grayscale
			// ZETestCheck(JPEGTestFail(UnsupportedInputList[2]));
			
			// Hierarchical
			// ZETestCheck(TestFail("TestResources/ZETextureFileJPEG/Valid/Unsupported/filesize-bad.bmp"));
			
			ZELog::GetInstance()->SetMinimumLogLevel(OldLogType);
		}
	}
}
