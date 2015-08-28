//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLProperty.h
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
#ifndef	__ZEML_PROPERTY_H__
#define __ZEML_PROPERTY_H__

#include "ZEMLElement.h"
#include "ZEDS/ZEValue.h"

class ZEMLProperty : public ZEMLElement
{
	friend class ZEMLNode;
	private:
		ZEValue						Value;

	public:
		virtual ZEMLElementType		GetType();

		virtual ZEMLElement*		Clone();

		ZEMLValueType				GetValueType();

		bool						SetValue(const ZEValue& Value);
		const ZEValue&				GetValue() const;

		void						SetFloat(float Value);
		void						SetDouble(double Value);
		void						SetInt8(ZEInt8 Value);
		void						SetInt16(ZEInt16 Value);
		void						SetInt32(ZEInt32 Value);
		void						SetInt64(ZEInt64 Value);
		void						SetUInt8(ZEUInt8 Value);
		void						SetUInt16(ZEUInt16 Value);
		void						SetUInt32(ZEUInt32 Value);
		void						SetUInt64(ZEUInt64 Value);
		void						SetBool(bool Value);
		void						SetString(const char* Value);
		void						SetQuaternion(const ZEQuaternion& Value);
		void						SetVector2(const ZEVector2& Value);
		void						SetVector3(const ZEVector3& Value);
		void						SetVector4(const ZEVector4& Value);
		void						SetMatrix3x3(const ZEMatrix3x3& Value);
		void						SetMatrix4x4(const ZEMatrix4x4& Value);

									ZEMLProperty();
									ZEMLProperty(const char* Name);
									ZEMLProperty(const char* Name, const ZEValue& Value);
};

#endif
