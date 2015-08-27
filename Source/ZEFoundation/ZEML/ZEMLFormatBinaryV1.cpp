//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLFormatBinaryV1.cpp
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

#include "ZEMLFormatBinaryV1.h"
#include "ZEFile\ZEFile.h"
#include "ZEEndian.h"
#include "ZEPointer\ZEPointer.h"

#define ZEML_ITEM_FILE_IDENTIFIER	'Z'
#define ZEML_MAX_NAME_SIZE			256

const char* ZEMLFormatBinaryV1::GetName() const
{
	return "ZEML Binary V1.0";
}

ZEUInt ZEMLFormatBinaryV1::GetMajorVersion() const
{
	return 1;
}

ZEUInt ZEMLFormatBinaryV1::GetMinorVersion() const
{
	return 0;
}

ZEMLFormatType ZEMLFormatBinaryV1::GetType() const
{
	return ZEML_FT_BINARY;
}

ZEMFormatSupport ZEMLFormatBinaryV1::GetSupport() const
{
	return ZEML_FS_WRITE | ZEML_FS_READ;
}

bool ZEMLFormatBinaryV1::ReadHeader(ZEFile* File)
{
	File->Seek(6, ZE_SF_BEGINING);

	ZEUInt64 StartOffset;
	if (File->Read(&StartOffset, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	File->Seek(ZEEndian::Little(StartOffset), ZE_SF_BEGINING);
	
	return true;
}

bool ZEMLFormatBinaryV1::ReadGoToNode(ZEFile* File, const ZEMLFormatElement& Element)
{
	File->Seek(Element.Offset, ZE_SF_BEGINING);
	return true;
}

bool ZEMLFormatBinaryV1::ReadElement(ZEFile* File, ZEMLFormatElement& Element)
{
	char Identifier;	
	if (File->Read(&Identifier, sizeof(char), 1) != 1)
		return false;

	if (Identifier != ZEML_ITEM_FILE_IDENTIFIER)
		return false;

	ZEUInt8 ElementType = 0;
	if (File->Read(&ElementType, sizeof(ZEUInt8), 1) != 1)
		return false;
	Element.ElementType = (ZEMLElementType)ElementType;

	ZEUInt8 NameSize = 0;
	if (File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
		return false;

	char NameBuffer[ZEML_MAX_NAME_SIZE];
	if (File->Read(NameBuffer, NameSize, 1) != 1)
		return false;

	NameBuffer[NameSize] = '\0';
	Element.Name = NameBuffer;
	Element.NameHash = Element.Name.Hash();

	if (Element.ElementType == ZEML_ET_NODE)
	{
		ZEUInt64 ElementSize = 0;
		if (File->Read(&ElementSize, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Size = ZEEndian::Little(ElementSize);

		ZEUInt64 ElementCount = 0;
		if (File->Read(&ElementCount, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Count = ZEEndian::Little(ElementCount);
		Element.Offset = File->Tell();

		File->Seek(Element.Size, ZE_SF_CURRENT);
	}
	else if (Element.ElementType == ZEML_ET_DATA)
	{
		ZEUInt64 ElementSize;
		if (File->Read(&ElementSize, sizeof(ZEUInt64), 1) != 1)
			return false;
		Element.Size = ZEEndian::Little(ElementSize);
		Element.Offset = File->Tell();

		File->Seek(Element.Size, ZE_SF_CURRENT);
	}
	else if (Element.ElementType == ZEML_ET_PROPERTY)
	{
		ZEUInt8 ValueType = 0;
		if (File->Read(&ValueType, sizeof(ZEUInt8), 1) != 1)
			return false;
		Element.ValueType = (ZEMLValueType)ValueType;

		switch (Element.ValueType)
		{
			case ZEML_VT_INT8:
			{
				ZEInt8 Temp;
				if (File->Read(&Temp, sizeof(ZEInt8), 1) != 1)
					return false;
				Element.Value.SetInt8(Temp);
				break;
			}

			case ZEML_VT_INT16:
			{
				ZEInt16 Temp;
				if (File->Read(&Temp, sizeof(ZEInt16), 1) != 1)
					return false;
				Element.Value.SetInt16(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_VT_INT32:
			{
				ZEInt32 Temp;
				if (File->Read(&Temp, sizeof(ZEInt32), 1) != 1)
					return false;
				Element.Value.SetInt32(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_VT_INT64:
			{
				ZEInt64 Temp;
				if (File->Read(&Temp, sizeof(ZEInt64), 1) != 1)
					return false;
				Element.Value.SetInt64(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_VT_UINT8:
			{
				ZEUInt8 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt8), 1) != 1)
					return false;
				Element.Value.SetUInt8(Temp);
				break;
			}

			case ZEML_VT_UINT16:
			{
				ZEUInt16 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt16), 1) != 1)
					return false;
				Element.Value.SetUInt16(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_VT_UINT32:
			{
				ZEUInt32 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt32), 1) != 1)
					return false;
				Element.Value.SetUInt32(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_VT_UINT64:
			{
				ZEUInt64 Temp;
				if (File->Read(&Temp, sizeof(ZEUInt64), 1) != 1)
					return false;
				Element.Value.SetUInt64(ZEEndian::Little(Temp));
				break;
			}

			case ZEML_VT_BOOLEAN:
			{
				bool Temp;
				if (File->Read(&Temp, sizeof(bool), 1) != 1)
					return false;
				Element.Value.SetBoolean(Temp);
				break;
			}

			case ZEML_VT_FLOAT:
			{
				float Temp;
				if (File->Read(&Temp, sizeof(float), 1) != 1)
					return false;
				Element.Value.SetFloat(Temp);
				break;
			}

			case ZEML_VT_DOUBLE:
			{
				double Temp;
				if (File->Read(&Temp, sizeof(double), 1) != 1)
					return false;
				Element.Value.SetDouble(Temp);
				break;
			}

			case ZEML_VT_STRING:
			{
				ZEUInt32 StringSize;
				if (File->Read(&StringSize, sizeof(ZEUInt32), 1) != 1)
					return false;
				StringSize = ZEEndian::Little(StringSize);

				if (StringSize == 0)
				{
					Element.Value.SetString("");
				}
				else
				{
					ZEPointer<char> Temp = new char[StringSize + 1];
					if (File->Read(Temp, StringSize, 1) != 1)
						return false;
					Temp[StringSize] = '\0';
					Element.Value.SetString(Temp);
				}
				break;
			}

			case ZEML_VT_QUATERNION:
			{
				ZEQuaternion Temp;
				if (File->Read(&Temp, sizeof(ZEQuaternion), 1) != 1)
					return false;
				Element.Value.SetQuaternion(Temp);
			}
			break;

			case ZEML_VT_VECTOR2:
			{
				ZEVector2 Temp;
				if (File->Read(&Temp, sizeof(ZEVector2), 1) != 1)
					return false;
				Element.Value.SetVector2(Temp);
				break;
			}

			case ZEML_VT_VECTOR3:
			{
				ZEVector3 Temp;
				if (File->Read(&Temp, sizeof(ZEVector3), 1) != 1)
					return false;
				Element.Value.SetVector3(Temp);
				break;
			}

			case ZEML_VT_VECTOR4:
			{
				ZEVector4 Temp;
				if (File->Read(&Temp, sizeof(ZEVector4), 1) != 1)
					return false;
				Element.Value.SetVector4(Temp);
				break;
			}

			case ZEML_VT_MATRIX3X3:
			{
				ZEMatrix3x3 Temp;
				if (File->Read(&Temp, sizeof(ZEMatrix3x3), 1) != 1)
					return false;
				Element.Value.SetMatrix3x3(Temp);
				break;
			}

			case ZEML_VT_MATRIX4X4:
			{
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
	else
	{
		return false;
	}

	return true;
}

bool ZEMLFormatBinaryV1::ReadData(ZEFile* File, const ZEMLFormatElement& Element, void* Buffer, ZESize Offset, ZESize Size)
{
	File->Seek(Element.Offset + Offset, ZE_SF_BEGINING);
	return File->Read(Buffer, Size, 1) == 1;
}

bool ZEMLFormatBinaryV1::WriteHeader(ZEFile* File)
{
	return false;
}

bool ZEMLFormatBinaryV1::WriteHeaderClose(ZEFile* File)
{
	return false;
}

bool ZEMLFormatBinaryV1::WriteElement(ZEFile* File, ZEMLFormatElement& Element)
{
	return false;
}

bool ZEMLFormatBinaryV1::WriteElementClose(ZEFile* File, ZEMLFormatElement& Element)
{
	return false;
}

bool ZEMLFormatBinaryV1::Determine(ZEFile* File)
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

	char Identifier[4];
	if (File->Read(Identifier, 4, 1) != 1)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	if (Identifier[0] != 'Z' || Identifier[1] != 'E' ||	Identifier[2] != 'M' ||	Identifier[3] != 'L')
		return false;

	ZEUInt8 Version[2];
	if (File->Read(&Version, 2 * sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	if (Version[0] != 1)
		return false;

	return true;
}

ZEMLFormat* ZEMLFormatBinaryV1::CreateInstance()
{
	return new ZEMLFormatBinaryV1();
}
