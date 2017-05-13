//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRCollectorProductInfo.cpp
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

#include "ZECRCollectorProductInfo.h"

#include "ZEDS/ZEFormat.h"
#include "ZEML/ZEMLWriter.h"
#include "ZECRReportParameters.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory.h>

ZECRDataProviderType ZECRCollectorProductInfo::GetCollectorType()
{
	return ZECR_DPT_TEXT;
}

const char* ZECRCollectorProductInfo::GetExtension()
{
	return ".xml";
}

const char* ZECRCollectorProductInfo::GetName()
{
	return "Product Information";
}

bool ZECRCollectorProductInfo::Generate(ZEMLWriterNode* CollectorNode, const ZECRReportParameters* Parameters)
{
	if (!ZECRCollector::Generate(CollectorNode, Parameters))
		return false;

	ZEString Data;

	Data += "<ZECRCrashReport>\n";
	Data += " <ZECRCollectorProductInfo>\n";
	Data += "  <Application>\n";
	Data += ZEFormat::Format("   <Name>{0}</Name>\n", Parameters->ApplicationName);
	Data += "   <Version>\n";
	Data += ZEFormat::Format("    <{0}>{1}</{0}>\n", "Major", Parameters->ApplicationVersion.Major);
	Data += ZEFormat::Format("    <{0}>{1}</{0}>\n", "Minor", Parameters->ApplicationVersion.Minor);
	Data += ZEFormat::Format("    <{0}>{1}</{0}>\n", "Internal", Parameters->ApplicationVersion.Internal);
	Data += ZEFormat::Format("    <{0}>{1}</{0}>\n", "Revision", Parameters->ApplicationVersion.Revision);
	Data += ZEFormat::Format("    <{0}>{1}</{0}>\n", "Branch", Parameters->ApplicationVersion.Branch);
	Data += ZEFormat::Format("    <{0}>{1}</{0}>\n", "Platform", Parameters->ApplicationVersion.Platform);
	Data += ZEFormat::Format("    <{0}>{1}</{0}>\n", "Architecture", Parameters->ApplicationVersion.Architecture);
	Data += "   </Version>\n";
	Data += "  </Application>\n";
	Data += "  <License>\n";
	Data += ZEFormat::Format("   <{0}>{1}</{0}>\n", "Internal", Parameters->LicenseProductName);
	Data += ZEFormat::Format("   <{0}>{1}</{0}>\n", "Major", Parameters->LicenseLicenseeName);
	Data += ZEFormat::Format("   <{0}>{1}</{0}>\n", "Minor", Parameters->LicenseSerialKey);
	Data += "  </License>\n";
	Data += " </ZECRCollectorProductInfo>\n";
	Data += "</ZECRCrashReport>";

	if (!CollectorNode->WriteData("Data", Data.GetValue(), Data.GetSize()))
		return false;

	return true;
}

ZECRCollectorProductInfo::ZECRCollectorProductInfo()
{

}
