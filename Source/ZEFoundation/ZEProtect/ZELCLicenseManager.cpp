//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCLicenseManager.cpp
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

#include "ZELCLicenseManager.h"

#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"

const ZEArray<ZELCLicense>& ZELCLicenseManager::GetLicenses()
{
	return Licenses;
}

void ZELCLicenseManager::RegisterLicense(const ZELCLicense& License)
{
	for (ZESize I = 0; I < Licenses.GetCount(); I++)
	{
		if (Licenses[I].GetGUID() == License.GetGUID())
			Licenses[I] = License;
	}

	Licenses.Add(License);
}

void ZELCLicenseManager::UnregisterLicense(const ZEGUID& LicenseGUID)
{
	for (ZESize I = 0; I < Licenses.GetCount(); I++)
	{
		if (Licenses[I].GetGUID() == LicenseGUID)
		{
			Licenses.Remove(I);
			return;
		}
	}
}

ZEArray<ZELCLicense> ZELCLicenseManager::LoadLicenseFile(const ZEString& FileName)
{
	ZEArray<ZELCLicense> Output;

	ZEMLReader Reader;
	if (!Reader.Open(FileName))
		return Output;	

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (RootNode.GetName() != "Licenses")
	{
		zeError("Not valid license file. File name: \"%s\".", FileName);
		return Output;	
	}
	
	
	ZESize LicenseCount = RootNode.GetNodeCount("License");
	Output.SetCount(LicenseCount);
	for (ZESize I = 0; I < LicenseCount; I++)
		Output[I].Load(&RootNode.GetNode("License", I));

	return Output;
}

bool ZELCLicenseManager::SaveLicenseFile(const ZEString& FileName, const ZEArray<ZELCLicense>& Licenses)
{
	ZEMLWriter Writer;
	if (!Writer.Open(FileName))
		return false;

	ZEMLWriterNode RootNode;
	Writer.OpenRootNode("Licenses", RootNode);
	RootNode.WriteUInt8("VersionMajor", 1);
	RootNode.WriteUInt8("VersionMinor", 0);

	for (ZESize I = 0; I < Licenses.GetCount(); I++)
	{
		ZEMLWriterNode NewNode;
		RootNode.OpenNode("License", NewNode);
		Licenses[I].Save(&NewNode);
		NewNode.CloseNode();
	}

	RootNode.CloseNode();
	Writer.Close();

	return true;
}

void ZELCLicenseManager::LoadLicenses()
{
	Licenses.Clear();

	ZEArray<ZELCLicense> SystemWideLicenses = LoadLicenseFile("S#:/License.zeLicense");
	for(ZESize I = 0; SystemWideLicenses.GetCount(); I++)
		SystemWideLicenses[I].SetSystemWide(true);
	Licenses.Combine(SystemWideLicenses);

	ZEArray<ZELCLicense> InstanceLicenses = LoadLicenseFile("S#:/License.zeLicense");
	for(ZESize I = 0; SystemWideLicenses.GetCount(); I++)
		SystemWideLicenses[I].SetSystemWide(false);
	Licenses.Combine(InstanceLicenses);
}

void ZELCLicenseManager::SaveLicenses()
{
	ZEArray<ZELCLicense> InstanceLicenses;
	ZEArray<ZELCLicense> SystemWideLicenses;

	for (ZESize I = 0; I < Licenses.GetCount(); I++)
	{
		if (Licenses[I].GetSystemWide())
			SystemWideLicenses.Add(Licenses[I]);
		else
			InstanceLicenses.Add(Licenses[I]);
	}

	SaveLicenseFile("S#:/License.zeLicense", SystemWideLicenses);
	SaveLicenseFile("US#:/License.zeLicense", InstanceLicenses);
}
