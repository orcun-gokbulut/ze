//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputDeviceState.h
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

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"

#include "ZEInputDefinitions.h"
#include "ZEInputDeviceDescription.h"
#include "ZEExport.ZEEngine.h"

class ZE_EXPORT_ZEENGINE ZEInputDeviceState
{
	public:
		struct  
		{
			ZEArray<bool>						OldValues;
			ZEArray<bool>						CurrentValues;

		} Buttons;

		struct  
		{
			ZEArray<float>						OldValues;
			ZEArray<float>						CurrentValues;
		} Axises;

		struct  
		{
			ZEArray<ZEInt>						OldValues;
			ZEArray<ZEInt>						CurrentValues;
		} POVs;

		struct  
		{
			ZEArray<ZEUInt>						OldValues;
			ZEArray<ZEUInt>						CurrentValues;
		} Switches;

		struct  
		{
			ZEArray<ZEVector4>					OldValues;
			ZEArray<ZEVector4>					CurrentValues;
		} Vectors;

		struct  
		{
			ZEArray<ZEQuaternion>				OldValues;
			ZEArray<ZEQuaternion>				CurrentValues;
		} Quaternions;

		struct 
		{
			ZEVector2							OldValue;
			ZEVector2							CurrentValue;
		} Cursor;

		ZEUInt									Character;
		ZEINCharacterModifiers					CharacterModifiers;
		
		void									Setup(const ZEInputDeviceDescription& Description);
		void									Reset();
		void									Advance();
		void									AdvanceAndReset();
		void									Clear();
};
