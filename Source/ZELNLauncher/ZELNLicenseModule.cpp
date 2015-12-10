//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLicenseModule.cpp
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

#include "ZELNLicenseModule.h"

#include "ZELNLicenseWidget.h"
#include "ZELNLauncher.h"
#include "ZEProtect\ZELCLicenseManager.h"

ZELN_MODULE_DECRIPTION(ZELNLicenseModule, "License");

void ZELNLicenseModule::LoadLicense()
{
	ZELCLicenseManager LicenseManager;
	LicenseManager.LoadLicenses();
	const ZELCLicense* ResultLicense = LicenseManager.RequestLicense(ZELNLauncher::GetInstance()->GetApplicationName(), ZELNLauncher::GetInstance()->GetApplicationVersionMajor());
	if (ResultLicense != NULL)
	{
		License = *ResultLicense;
		LicenseValid = true;
	}
	else
	{
		ZEArray<ZELCLicense> Licenses = LicenseManager.GetLicenses(ZELNLauncher::GetInstance()->GetApplicationName(), ZELNLauncher::GetInstance()->GetApplicationVersionMajor());
		Licenses.Sort2<ZELCLicenseManager::CompareLicenseOrder>();
		if (Licenses.GetCount() != 0 && Licenses[0].GetEnabled())
		{
			License = Licenses[0];
		}
		else
		{
			License = ZELCLicense();
			License.SetApplicationName(ZELNLauncher::GetInstance()->GetApplicationName());
		}
		LicenseValid = false;
	}

	Widget->SetLicense(License);

	ZELNLauncher::GetInstance()->Update();
}

bool ZELNLicenseModule::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;
	
	Widget = new ZELNLicenseWidget();
	Widget->Module = this;
	LoadLicense();

	return true;
}

QWidget* ZELNLicenseModule::GetWidget()
{
	return Widget;
}

bool ZELNLicenseModule::GetAllowLaunch()
{
	return LicenseValid;
}

Ui_ZELNLicenseWidget* ZELNLicenseModule::GetForm()
{
	return Widget->Form;
}