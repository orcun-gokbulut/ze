//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileBMPTest.cpp
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

static bool TestSuccess(ZEString FileName)
{
	ZEPointer<ZEFile> File = ZEFile::Open(FileName);

	ZETextureFileBMP Loader;
	ZETextureDataInfo Info;
	if (!Loader.LoadInfo(&Info, File))
		return false;

	ZETextureData* Data = Loader.Load(File);

	if (Data == NULL)
		return false;

	ZEBitmap Original;
	if (!Original.Load(FileName))
	{
		//return false;
	}
	
	ZETextureLevel* Level = &Data->GetSurfaces()[0].GetLevels()[0];
	ZEBitmap Bitmap;
	Bitmap.Create(Level->GetWidth(), Level->GetHeight(), 4);
	Bitmap.CopyFrom(Level->GetData(), Level->GetPitch(), Level->GetWidth(), Level->GetHeight());
	Bitmap.Save(FileName + ".result.bmp", ZE_BFF_BMP);

	return true;//CompareImages(&Original, (ZEUInt32*)Level->GetData(), Data->GetWidth(), Data->GetHeight());
}

static bool TestFail(ZEString FileName)
{
	ZEPointer<ZEFile> File = ZEFile::Open(FileName);

	ZETextureFileBMP Loader;
	ZETextureData* Data = Loader.Load(File);

	return Data == NULL;
}

ZETestSuite(ZETextureFileBMPTest)
{
	ZETest("Valid Samples")
	{
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/24bpp-1x1.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/24bpp-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/24bpp-321x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/24bpp-322x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/24bpp-323x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/24bpp-topdown-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/32bpp-1x1.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/32bpp-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/32bpp-888-optimalpalette-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/32bpp-optimalpalette-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/32bpp-topdown-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/555-1x1.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/555-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/555-321x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/565-1x1.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/565-320x240-topdown.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/565-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/565-321x240-topdown.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/565-321x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/565-322x240-topdown.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/565-322x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-1x1.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-1x64000.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-321x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-322x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-323x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-colorsimportant-two.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-colorsused-zero.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/8bpp-topdown-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/rle8-64000x1.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/rle8-absolute-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/rle8-blank-160x120.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/rle8-delta-320x240.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/rle8-encoded-320x240.bmp"));
	}

	ZETest("Semi Valid")
	{
		// Semi Valid
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/8bpp-pixels-not-in-palette.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/pels-per-meter-x-large.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/pels-per-meter-x-negative.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/pels-per-meter-x-zero.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/pels-per-meter-y-large.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/pels-per-meter-y-negative.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/pels-per-meter-y-zero.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/pixeldata-rle8-toomuch.bmp"));
		//ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/planes-large.bmp"));
		//ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/planes-zero.bmp"));
		//ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/reserved1-bad.bmp"));
		//ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/reserved2-bad.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/rle8-height-negative.bmp"));
		ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/rle8-no-end-of-bitmap-marker.bmp"));
		//ZETestCheck(TestSuccess("TestResources/ZETextureFileBMP/Valid/Questionable/8bpp-no-palette.bmp"));

	}

	ZETest("Corrupted or Malicius")
	{
		ZELogType OldLogType = ZELog::GetInstance()->GetMinimumLogLevel();
		ZELog::GetInstance()->SetMinimumLogLevel(ZE_LOG_CRITICAL_ERROR);
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/1bpp-no-palette.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/1bpp-pixeldata-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/24bpp-pixeldata-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/32bpp-pixeldata-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/4bpp-no-palette.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/4bpp-pixeldata-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/555-pixeldata-cropped.bmp"));
		//ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/8bpp-colorsimportant-large.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/8bpp-colorsimportant-negative.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/8bpp-colorsused-large.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/8bpp-colorsused-negative.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/8bpp-pixeldata-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/bitdepth-large.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/bitdepth-odd.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/bitdepth-zero.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/colormasks-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/colormasks-missing.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/compression-bad-rle4-for-8bpp.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/compression-bad-rle8-for-4bpp.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/compression-unknown.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/emptyfile.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/fileinfoheader-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/height-zero.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/infoheader-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/infoheader-missing.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/infoheadersize-large.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/infoheadersize-small.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/infoheadersize-zero.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/magicnumber-bad.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/magicnumber-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/offbits-large.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/offbits-negative.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/offbits-zero.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/palette-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/palette-missing.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/palette-too-big.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/pixeldata-missing.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle4-absolute-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle4-delta-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle4-no-end-of-line-marker.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle4-runlength-cropped.bmp"));
		/*ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle8-absolute-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle8-delta-cropped.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle8-deltaleavesimage.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle8-no-end-of-line-marker.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/rle8-runlength-cropped.bmp"));*/
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/width-negative.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/width-times-height-overflow.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Invalid/width-zero.bmp"));
		ZELog::GetInstance()->SetMinimumLogLevel(OldLogType);
	}

	ZETest("Unsupported Sample Types")
	{
		ZELogType OldLogType = ZELog::GetInstance()->GetMinimumLogLevel();
		ZELog::GetInstance()->SetMinimumLogLevel(ZE_LOG_CRITICAL_ERROR);
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Valid/Unsupported/filesize-bad.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Valid/Unsupported/filesize-zero.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Valid/Unsupported/pixeldata-toomuch.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/32bpp-101110-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-1x1.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-320x240-color.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-320x240-overlappingcolor.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-321x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-322x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-323x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-324x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-325x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-326x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-327x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-328x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-329x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-330x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-331x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-332x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-333x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-334x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-335x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/1bpp-topdown-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/32bpp-0x0.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/32bpp-0x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/32bpp-320x0.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-1x1.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-321x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-322x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-323x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-324x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-325x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-326x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-327x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/4bpp-topdown-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/rle4-absolute-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/rle4-alternate-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/rle4-delta-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/rle4-encoded-320x240.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/rle4-height-negative.bmp"));
		ZETestCheck(TestFail("TestResources/ZETextureFileBMP/Unsupported/rle4-no-end-of-bitmap-marker.bmp"));
		ZELog::GetInstance()->SetMinimumLogLevel(OldLogType);
	}
}
