//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCLicense.h
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

#pragma once

#include "ZEDS/ZEString.h"
#include "ZEGUID.h"

class ZEMLReaderNode;
class ZEMLWriterNode;

class ZELCLicense
{
	private:
		ZEGUID					GUID;
		ZEString				ApplicationName;
		ZEString				LicenseeName;
		ZEString				SerialKey;
		ZEString				ActivationCode;
		ZEInt					Priority;
		ZEUInt					LicenseVersion;
		bool					SystemWide;
		bool					Enabled;

	public:
		void					SetGUID(const ZEGUID& GUID);
		const ZEGUID&			GetGUID() const;

		void					SetApplicationName(const ZEString& ApplicationName);
		const ZEString&			GetApplicationName() const;

		ZEUInt8					GetApplicationVersionMajor() const;
		ZEUInt8					GetApplicationVersionMinor() const;
		ZEUInt8					GetApplicationEdition() const;
		ZEUInt8					GetFlags() const;

		void					SetLicenseeName(const ZEString& Name);
		const ZEString&			GetLicenseeName() const;
	 
		void					SetSerialKey(const ZEString& SerialKey);
		const ZEString&			GetSerialKey() const;

		void					SetActivationCode(const ZEString& Code);
		const ZEString&			GetActivationCode() const;

		void					SetPriority(ZEInt Priority);
		ZEInt					GetPriority() const;

		void					SetEnabled(bool Enabled);
		bool					GetEnabled() const;

		void					SetSystemWide(bool SystemWide);
		bool					GetSystemWide() const;

		void					SetLicenseVersion(ZEUInt Version);
		ZEUInt					GetLicenseVersion() const;

		ZEString				GeneratePreActivationCode() const;

		bool					CheckSerialKeyValid() const;
		bool					CheckActivationCodeValid() const;
		bool					CheckValid() const;

		void					Load(ZEMLReaderNode* Reader);
		void					Save(ZEMLWriterNode* Writer) const;

								ZELCLicense();
};
