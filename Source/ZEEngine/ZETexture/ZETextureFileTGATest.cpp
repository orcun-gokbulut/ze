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
#include "ZETextureData.h"
#include "ZETextureFileTGA.h"
#include "ZETextureTestUtils.h"
#include "ZETextureLoader.h"

static bool TestSuccess(ZEString FileName)
{
	ZEFile File;
	File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);

	ZETextureFileTGA Loader;
	ZETextureDataInfo Info;
	if (!Loader.LoadInfo(&Info, &File))
		return false;

	ZEPointer<ZETextureData> Data = Loader.Load(&File);
	if (Data == NULL)
		return false;

	return ZETextureTestUtils::Compare(Data, FileName + ".zeTexture");
}
 
static bool TestFail(ZEString FileName)
{
	ZELogType OldLogType = ZELog::GetInstance()->GetMinimumLogLevel();
	ZELog::GetInstance()->SetMinimumLogLevel(ZE_LOG_CRITICAL_ERROR);

	ZEFile File;
	File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE);

	ZETextureFileTGA Loader;
	ZEPointer<ZETextureData> Data = Loader.Load(&File);

	ZELog::GetInstance()->SetMinimumLogLevel(OldLogType);

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

	ZETest("Ilvalid Samples")
	{
		ZETestCheck(TestFail("Non Existent File"));
	}
}
