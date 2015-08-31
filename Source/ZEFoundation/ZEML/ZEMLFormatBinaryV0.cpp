//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLFormatBinaryV0.cpp
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

#include "ZEMLFormatBinaryV0.h"
#include "ZEFile\ZEFile.h"
#include "ZEEndian.h"
#include "ZEPointer\ZEPointer.h"

#define ZEML_ITEM_FILE_IDENTIFIER	'Z'
#define ZEML_MAX_NAME_SIZE			256

enum ZEMLElementTypeV0
{
	ZEML_ETV0_UNDEFINED		= 0,
	ZEML_ETV0_FLOAT			= 1,
	ZEML_ETV0_DOUBLE		= 2,
	ZEML_ETV0_INT8			= 3,
	ZEML_ETV0_INT16			= 4,
	ZEML_ETV0_INT32			= 5,
	ZEML_ETV0_INT64			= 6,
	ZEML_ETV0_UINT8			= 7,
	ZEML_ETV0_UINT16		= 8,
	ZEML_ETV0_UINT32		= 9,
	ZEML_ETV0_UINT64		= 10,
	ZEML_ETV0_BOOLEAN		= 11,
	ZEML_ETV0_STRING		= 12,
	ZEML_ETV0_QUATERNION	= 13,
	ZEML_ETV0_VECTOR2		= 14,
	ZEML_ETV0_VECTOR3		= 15,
	ZEML_ETV0_VECTOR4		= 16,
	ZEML_ETV0_MATRIX3X3		= 17,
	ZEML_ETV0_MATRIX4X4		= 18,
	ZEML_ETV0_INLINE_DATA	= 19,
	ZEML_ETV0_OFFSET_DATA	= 20,
	ZEML_ETV0_NODE			= 21
};

struct ZEMLFormatBinartyV0Description : public ZEMLFormatDescription
{
	public:
		virtual const char*			GetName() const;
		virtual ZEUInt				GetMinorVersion() const;
		virtual ZEUInt				GetMajorVersion() const;
		virtual ZEMLFormatType		GetType() const;
		virtual ZEMFormatSupport	GetSupport() const;
		virtual bool				Determine(ZEFile* File);
		virtual ZEMLFormat*			CreateInstance();
};


const char* ZEMLFormatBinartyV0Description::GetName() const
{
	return "ZEML Binary V1.0";
}

ZEUInt ZEMLFormatBinartyV0Description::GetMajorVersion() const
{
	return 0;
}

ZEUInt ZEMLFormatBinartyV0Description::GetMinorVersion() const
{
	return 0;
}

ZEMLFormatType ZEMLFormatBinartyV0Description::GetType() const
{
	return ZEML_FT_BINARY;
}

ZEMFormatSupport ZEMLFormatBinartyV0Description::GetSupport() const
{
	return ZEML_FS_READ;
}

bool ZEMLFormatBinartyV0Description::Determine(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Cannot load ZEML file. File is NULL.");
		return false;
	}

	if (!File->IsOpen())
	{
		zeError("Cannot load ZEML file. File is not open.");
		return false;
	}

	File->Seek(0, ZE_SF_BEGINING);

	char Identifier[2];
	if (File->Read(Identifier, 2, 1) != 1)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	if (Identifier[0] != 'Z' || Identifier[1] != 21)
		return false;

	zeWarning("Deprecated ZEML file format detected. Please convert this file to newer ZEML file version. File Name: \"%s\".", File->GetPath().ToCString());

	return true;
}

ZEMLFormat* ZEMLFormatBinartyV0Description::CreateInstance()
{
	return new ZEMLFormatBinaryV0();
}

ZEMLFormatDescription* ZEMLFormatBinaryV0::Description()
{
	static ZEMLFormatBinartyV0Description Description;
	return &Description;
}

ZEMLFormatDescription* ZEMLFormatBinaryV0::GetDescription() const
{
	return Description();
}

bool ZEMLFormatBinaryV0::ReadHeader(ZEFile* File)
{
	File->Seek(0, ZE_SF_BEGINING);
	return true;
}

bool ZEMLFormatBinaryV0::ReadGoToNode(ZEFile* File, const ZEMLFormatElement& Element)
{
	File->Seek(Element.Offset, ZE_SF_BEGINING);
	return true;
}

bool ZEMLFormatBinaryV0::ReadElement(ZEFile* File, ZEMLFormatElement& Element)
{
	char Identifier;	
	if (File->Read(&Identifier, sizeof(char), 1) != 1)
		return false;

	if (Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		return false;

	ZEUInt8 ElementTypeNative = 0;
	if (File->Read(&ElementTypeNative, sizeof(ZEUInt8), 1) != 1)
		return false;
	ZEMLElementTypeV0 ElementType = (ZEMLElementTypeV0)ElementTypeNative;

	ZEUInt8 NameSize = 0;
	if (File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
		return false;

	char NameBuffer[ZEML_MAX_NAME_SIZE];
	if (File->Read(NameBuffer, NameSize, 1) != 1)
		return false;
	NameBuffer[NameSize] = '\0';
	Element.Name = NameBuffer;
	Element.NameHash = Element.Name.Hash();
	
	if (ElementType == ZEML_ETV0_NODE)
	{
		Element.ElementType = ZEML_ET_NODE;

		ZEUInt64 ElementCount = 0;
		if (File->Read(&ElementCount, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Count = ZEEndian::Little(ElementCount);

		ZEUInt64 ElementSize = 0;
		if (File->Read(&ElementSize, sizeof(ZEUInt64), 1) != 1)
			return false;

		Element.Size = ZEEndian::Little(ElementSize);
		Element.Offset = File->Tell();

		File->Seek(Element.Size, ZE_SF_CURRENT);
	}
	else if (ElementType == ZEML_ETV0_INLINE_DATA)
	{
		Element.ElementType = ZEML_ET_DATA;

		ZEUInt64 ElementSize = 0;
		if (File->Read(&ElementSize, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Size = ZEEndian::Little(ElementSize);

		Element.Offset = File->Tell();

		File->Seek(Element.Size, ZE_SF_CURRENT);
	}
	else if (ElementType == ZEML_ETV0_OFFSET_DATA)
	{
		Element.ElementType = ZEML_ET_DATA;

		ZEUInt64 ElementSize = 0;
		if (File->Read(&ElementSize, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Size = ZEEndian::Little(ElementSize);

		ZEUInt64 ElementOffset = 0;
		if (File->Read(&ElementOffset, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Offset = ZEEndian::Little(ElementOffset);
	}
	else
	{
		Element.ElementType = ZEML_ET_PROPERTY;
		ZEUInt64 ElementSize = 0;
		if (File->Read(&ElementSize, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Size = ZEEndian::Little(ElementSize);

		switch (ElementType)
		{
			case ZEML_ETV0_INT8:
			{
				Element.ValueType = ZEML_VT_INT8;
				ZEInt8 Temp;
				if (File->Read(&Temp, sizeof(ZEInt8), 1) != 1)
					return false;
				Element.Value.SetInt8(Temp);
				break;
			}

			case ZEML_ETV0_INT16:
			{
				Element.ValueType = ZEML_VT_INT16;
				ZEInt16 Temp;
				if (File->Read(&Temp, sizeof(ZEInt16), 1) != 1)
					return false;
				Element.Value.SetInt16(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_ETV0_INT32:
			{
				Element.ValueType = ZEML_VT_INT32;
				ZEInt32 Temp;
				if (File->Read(&Temp, sizeof(ZEInt32), 1) != 1)
					return false;
				Element.Value.SetInt32(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_ETV0_INT64:
			{
				Element.ValueType = ZEML_VT_INT64;
				ZEInt64 Temp;
				if (File->Read(&Temp, sizeof(ZEInt64), 1) != 1)
					return false;
				Element.Value.SetInt64(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_ETV0_UINT8:
			{
				Element.ValueType = ZEML_VT_UINT8;
				ZEUInt8 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt8), 1) != 1)
					return false;
				Element.Value.SetUInt8(Temp);
				break;
			}

			case ZEML_ETV0_UINT16:
			{
				Element.ValueType = ZEML_VT_UINT16;
				ZEUInt16 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt16), 1) != 1)
					return false;
				Element.Value.SetUInt16(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_ETV0_UINT32:
			{
				Element.ValueType = ZEML_VT_UINT32;
				ZEUInt32 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt32), 1) != 1)
					return false;
				Element.Value.SetUInt32(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_ETV0_UINT64:
			{
				Element.ValueType = ZEML_VT_UINT64;
				ZEUInt64 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt64), 1) != 1)
					return false;
				Element.Value.SetUInt64(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_ETV0_BOOLEAN:
			{
				Element.ValueType = ZEML_VT_BOOLEAN;
				bool Temp;
				if (File->Read(&Temp, sizeof(bool), 1) != 1)
					return false;
				Element.Value.SetBoolean(Temp);
				break;
			}

			case ZEML_ETV0_FLOAT:
			{
				Element.ValueType = ZEML_VT_FLOAT;
				float Temp;
				if (File->Read(&Temp, sizeof(float), 1) != 1)
					return false;
				Element.Value.SetFloat(Temp);
				break;
			}

			case ZEML_ETV0_DOUBLE:
			{
				Element.ValueType = ZEML_VT_DOUBLE;
				double Temp;
				if (File->Read(&Temp, sizeof(double), 1) != 1)
					return false;
				Element.Value.SetDouble(Temp);
				break;
			}

			case ZEML_ETV0_STRING:
			{
				Element.ValueType = ZEML_VT_STRING;
				if (Element.Size == 0)
				{
					Element.Value.SetString("");
				}
				else
				{
					ZEPointer<char> Temp = new char[Element.Size];
					if (File->Read(Temp, Element.Size, 1) != 1)
						return false;
					Element.Value.SetString(Temp);
				}
				break;
			}

			case ZEML_ETV0_QUATERNION:
			{
				Element.ValueType = ZEML_VT_QUATERNION;
				ZEQuaternion Temp;
				if (File->Read(&Temp, sizeof(ZEQuaternion), 1) != 1)
					return false;
				Element.Value.SetQuaternion(Temp);
				break;
			}

			case ZEML_ETV0_VECTOR2:
			{
				Element.ValueType = ZEML_VT_VECTOR2;
				ZEVector2 Temp;
				if (File->Read(&Temp, sizeof(ZEVector2), 1) != 1)
					return false;
				Element.Value.SetVector2(Temp);
				break;
			}

			case ZEML_ETV0_VECTOR3:
			{
				Element.ValueType = ZEML_VT_VECTOR3;
				ZEVector3 Temp;
				if (File->Read(&Temp, sizeof(ZEVector3), 1) != 1)
					return false;
				Element.Value.SetVector3(Temp);
				break;
			}

			case ZEML_ETV0_VECTOR4:
			{
				Element.ValueType = ZEML_VT_VECTOR4;
				ZEVector4 Temp;
				if (File->Read(&Temp, sizeof(ZEVector4), 1) != 1)
					return false;
				Element.Value.SetVector4(Temp);
				break;
			}

			case ZEML_ETV0_MATRIX3X3:
			{
				Element.ValueType = ZEML_VT_MATRIX3X3;
				ZEMatrix3x3 Temp;
				if (File->Read(&Temp, sizeof(ZEMatrix3x3), 1) != 1)
					return false;
				Element.Value.SetMatrix3x3(Temp);
				break;
			}

			case ZEML_ETV0_MATRIX4X4:
			{
				Element.ValueType = ZEML_VT_MATRIX4X4;
				ZEMatrix4x4 Temp;
				if (File->Read(&Temp, sizeof(ZEMatrix4x4), 1) != 1)
					return false;
				Element.Value.SetMatrix4x4(Temp);
				break;
			}

			default:
				return false;
		}
	}

	return true;
}

bool ZEMLFormatBinaryV0::ReadData(ZEFile* File, const ZEMLFormatElement& Element, void* Buffer, ZESize Offset, ZESize Size)
{
	File->Seek(Element.Offset + Offset, ZE_SF_BEGINING);
	return File->Read(Buffer, Size, 1) == 1;
}

bool ZEMLFormatBinaryV0::WriteHeader(ZEFile* File)
{
	return false;
}

bool ZEMLFormatBinaryV0::WriteHeaderClose(ZEFile* File)
{
	return false;
}

bool ZEMLFormatBinaryV0::WriteElement(ZEFile* File, ZEMLFormatElement& Element)
{
	return false;
}

bool ZEMLFormatBinaryV0::WriteElementClose(ZEFile* File, ZEMLFormatElement& Element)
{
	return false;
}
