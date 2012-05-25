//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileTGATest.cpp
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
#include "ZETextureFileTGA.h"

static bool CompareImages(ZEBitmap* Original, ZEUInt32* Data, ZESize Width, ZESize Height)
{
	if (Original->GetWidth() != Width || Original->GetHeight() != Height)
		return false;

	for (ZESize y = 0; y < Height; y++)
		for (ZESize x = 0; x < Width; x++)
			if (((ZEUInt32*)Original->GetPixels())[y * Width + x] != Data[y * Width + x])
				return false;

	return true;
}

static bool TestSuccess(ZEString FileName)
{
	ZEPointer<ZEFile> File = ZEFile::Open(FileName);

	ZETextureFileTGA Loader;
	ZETextureDataInfo Info;
	if (!Loader.LoadInfo(&Info, File))
		return false;

	ZETextureData* Data = Loader.Load(File);

	if (Data == NULL)
		return false;

	ZEBitmap Original;
	if (!Original.Load(FileName))
		return false;
	ZETextureLevel* Level = &Data->GetSurfaces()[0].GetLevels()[0];

	ZEBitmap Bitmap;
	Bitmap.Create(Level->GetWidth(), Level->GetHeight(), 4);
	Bitmap.CopyFrom(Level->GetData(), Level->GetPitch(), Level->GetWidth(), Level->GetHeight());
	Bitmap.Save(FileName + ".result.bmp", ZE_BFF_BMP);

	return true;// CompareImages(&Original, (ZEUInt32*)Level->GetData(), Data->GetWidth(), Data->GetHeight());
}

static bool TestFail(ZEString FileName)
{
	ZEPointer<ZEFile> File = ZEFile::Open(FileName);

	ZETextureFileTGA Loader;
	ZETextureData* Data = Loader.Load(File);

	return Data == NULL;
}

ZETestSuite(ZETextureFileTGATest)
{
	ZETest("Valid Samples")
	{
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/CBW8.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/CCM8.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/CTC16.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/CTC24.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/CTC32.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/FLAG_B16.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/FLAG_B24.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/FLAG_B32.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/FLAG_T16.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/FLAG_T32.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/MARBLES.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/UBW8.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/UCM8.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/UTC16.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/UTC24.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/UTC32.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/XING_B16.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/XING_B24.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/XING_B32.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/XING_T16.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/XING_T24.TGA"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileTGA/XING_T32.TGA"));
	}
}
