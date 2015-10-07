//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCUtils.cpp
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

#include "ZELCUtils.h"

#include "ZEDS\ZEFormat.h"
#include "ZERegEx\ZERegEx.h"

bool ZELCUtils::CheckSerialCode(const void* SerialKey)
{
	const ZEUInt32* SerialKeyBinary = (const ZEUInt32*)SerialKey;

	ZEUInt32 Check;
	Check  = SerialKeyBinary[0] + 0x46AD788E;
	Check ^= SerialKeyBinary[1] - 0x6B71E511;
	Check ^= SerialKeyBinary[2] - 0x915067FA;
	Check ^= SerialKeyBinary[3] + 0xEE7C85CA;
	return  SerialKeyBinary[4] == Check;
}

ZEString ZELCUtils::ConvertSerialKey(const void* SerialKeyBinary)
{
	const ZEUInt32* SerialKeyUInt32 = (const ZEUInt32*)SerialKeyBinary;

	return ZEFormat::Format("ZE-{0:X:08}-{1:X:08}-{2:X:08}-{3:X:08}-{4:X:08}",
		SerialKeyUInt32[0], SerialKeyUInt32[1], SerialKeyUInt32[2], SerialKeyUInt32[3], SerialKeyUInt32[4]);
}

bool ZELCUtils::ConvertSerialKey(void* SerialKey, const ZEString& SerialKeyText)
{
	ZEUInt32* SerialKeyUInt32 = (ZEUInt32*)SerialKey;

	ZERegEx RegEx("[\\n\\r\\s]*ZE[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*");

	ZERegExMatch Match;
	if (!RegEx.Match(SerialKeyText, Match))
		return false;

	SerialKeyUInt32[0] = Match.SubMatches[0].String.ToUInt32(16);
	SerialKeyUInt32[1] = Match.SubMatches[1].String.ToUInt32(16);
	SerialKeyUInt32[2] = Match.SubMatches[2].String.ToUInt32(16);
	SerialKeyUInt32[3] = Match.SubMatches[3].String.ToUInt32(16);
	SerialKeyUInt32[4] = Match.SubMatches[4].String.ToUInt32(16);

	return true;
}

ZEString ZELCUtils::ConvertActivationCode(const void* ActivationCodeBinary)
{
	const ZEUInt32* ActivationCodeUInt32 = (const ZEUInt32*)ActivationCodeBinary;

	return ZEFormat::Format(
		"ZE-"
		"{ 0:X:08}-{ 1:X:08}-{ 2:X:08}-{ 3:X:08}-{ 4:X:08}-{ 5:X:08}-{ 6:X:08}-{ 7:X:08}-"
		"{ 8:X:08}-{ 9:X:08}-{10:X:08}-{11:X:08}-{12:X:08}-{13:X:08}-{14:X:08}-{15:X:08}-"
		"{16:X:08}-{17:X:08}-{18:X:08}-{19:X:08}-{20:X:08}-{21:X:08}-{22:X:08}-{23:X:08}-"
		"{24:X:08}-{25:X:08}-{26:X:08}-{27:X:08}-{28:X:08}-{29:X:08}-{30:X:08}-{31:X:08}",
		ActivationCodeUInt32[ 0], ActivationCodeUInt32[ 1], ActivationCodeUInt32[ 2], ActivationCodeUInt32[ 3],
		ActivationCodeUInt32[ 4], ActivationCodeUInt32[ 5], ActivationCodeUInt32[ 6], ActivationCodeUInt32[ 7],
		ActivationCodeUInt32[ 8], ActivationCodeUInt32[ 9], ActivationCodeUInt32[10], ActivationCodeUInt32[11],
		ActivationCodeUInt32[12], ActivationCodeUInt32[13], ActivationCodeUInt32[14], ActivationCodeUInt32[15],
		ActivationCodeUInt32[16], ActivationCodeUInt32[17], ActivationCodeUInt32[18], ActivationCodeUInt32[19],
		ActivationCodeUInt32[20], ActivationCodeUInt32[21], ActivationCodeUInt32[22], ActivationCodeUInt32[23],
		ActivationCodeUInt32[24], ActivationCodeUInt32[25], ActivationCodeUInt32[26], ActivationCodeUInt32[27],
		ActivationCodeUInt32[28], ActivationCodeUInt32[29], ActivationCodeUInt32[30], ActivationCodeUInt32[31]);
}

bool ZELCUtils::ConvertActivationCode(void* ActivationCodeBinary, const ZEString& ActivationCodeText)
{
	ZEUInt32* ActivationCodeUInt32 = (ZEUInt32*)ActivationCodeBinary;

	ZERegEx RegEx("[\\n\\r\\s]*ZE[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-"
		"[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*-[\\n\\r\\s]*([a-fA-F0-9]{8})[\\n\\r\\s]*");
	
	ZERegExMatch Match;
	if (!RegEx.Match(ActivationCodeText, Match))
		return false;

	ActivationCodeUInt32[ 0] = Match.SubMatches[ 0].String.ToUInt32(16);
	ActivationCodeUInt32[ 1] = Match.SubMatches[ 1].String.ToUInt32(16);
	ActivationCodeUInt32[ 2] = Match.SubMatches[ 2].String.ToUInt32(16);
	ActivationCodeUInt32[ 3] = Match.SubMatches[ 3].String.ToUInt32(16);
	ActivationCodeUInt32[ 4] = Match.SubMatches[ 4].String.ToUInt32(16);
	ActivationCodeUInt32[ 5] = Match.SubMatches[ 5].String.ToUInt32(16);
	ActivationCodeUInt32[ 6] = Match.SubMatches[ 6].String.ToUInt32(16);
	ActivationCodeUInt32[ 7] = Match.SubMatches[ 7].String.ToUInt32(16);
	ActivationCodeUInt32[ 8] = Match.SubMatches[ 8].String.ToUInt32(16);
	ActivationCodeUInt32[ 9] = Match.SubMatches[ 9].String.ToUInt32(16);
	ActivationCodeUInt32[10] = Match.SubMatches[10].String.ToUInt32(16);
	ActivationCodeUInt32[11] = Match.SubMatches[11].String.ToUInt32(16);
	ActivationCodeUInt32[12] = Match.SubMatches[12].String.ToUInt32(16);
	ActivationCodeUInt32[13] = Match.SubMatches[13].String.ToUInt32(16);
	ActivationCodeUInt32[14] = Match.SubMatches[14].String.ToUInt32(16);
	ActivationCodeUInt32[15] = Match.SubMatches[15].String.ToUInt32(16);
	ActivationCodeUInt32[16] = Match.SubMatches[16].String.ToUInt32(16);
	ActivationCodeUInt32[17] = Match.SubMatches[17].String.ToUInt32(16);
	ActivationCodeUInt32[18] = Match.SubMatches[18].String.ToUInt32(16);
	ActivationCodeUInt32[19] = Match.SubMatches[19].String.ToUInt32(16);
	ActivationCodeUInt32[20] = Match.SubMatches[20].String.ToUInt32(16);
	ActivationCodeUInt32[21] = Match.SubMatches[21].String.ToUInt32(16);
	ActivationCodeUInt32[22] = Match.SubMatches[22].String.ToUInt32(16);
	ActivationCodeUInt32[23] = Match.SubMatches[23].String.ToUInt32(16);
	ActivationCodeUInt32[24] = Match.SubMatches[24].String.ToUInt32(16);
	ActivationCodeUInt32[25] = Match.SubMatches[25].String.ToUInt32(16);
	ActivationCodeUInt32[26] = Match.SubMatches[26].String.ToUInt32(16);
	ActivationCodeUInt32[27] = Match.SubMatches[27].String.ToUInt32(16);
	ActivationCodeUInt32[28] = Match.SubMatches[28].String.ToUInt32(16);
	ActivationCodeUInt32[29] = Match.SubMatches[29].String.ToUInt32(16);
	ActivationCodeUInt32[30] = Match.SubMatches[30].String.ToUInt32(16);
	ActivationCodeUInt32[31] = Match.SubMatches[31].String.ToUInt32(16);

	return true;
}
