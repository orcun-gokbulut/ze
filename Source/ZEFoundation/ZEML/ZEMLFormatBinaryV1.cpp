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
#include "ZEDS\ZEArray.h"

#define ZEML_ITEM_FILE_IDENTIFIER	'Z'
#define ZEML_MAX_NAME_SIZE			256

struct ZEMLFormatBinaryV1Description : public ZEMLFormatDescription
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

const char* ZEMLFormatBinaryV1Description::GetName() const
{
	return "ZEML Binary  Format v1.0";
}

ZEUInt ZEMLFormatBinaryV1Description::GetMajorVersion() const
{
	return 1;
}

ZEUInt ZEMLFormatBinaryV1Description::GetMinorVersion() const
{
	return 0;
}

ZEMLFormatType ZEMLFormatBinaryV1Description::GetType() const
{
	return ZEML_FT_BINARY;
}

ZEMFormatSupport ZEMLFormatBinaryV1Description::GetSupport() const
{
	return ZEML_FS_WRITE | ZEML_FS_READ;
}


bool ZEMLFormatBinaryV1Description::Determine(ZEFile* File)
{
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

ZEMLFormat* ZEMLFormatBinaryV1Description::CreateInstance()
{
	return new ZEMLFormatBinaryV1();
}

ZEMLFormatDescription* ZEMLFormatBinaryV1::Description()
{
	static ZEMLFormatBinaryV1Description Description;
	return &Description;
}

ZEMLFormatDescription* ZEMLFormatBinaryV1::GetDescription() const
{
	return Description();
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
					ZEArray<char> Temp;
					Temp.SetCount(StringSize + 1);
					if (File->Read(Temp.GetCArray(), StringSize, 1) != 1)
						return false;
					Temp[StringSize] = '\0';
					Element.Value.SetString(Temp.GetConstCArray());
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

			case ZEML_VT_VECTOR2D:
			{
				ZEVector2d Temp;
				if (File->Read(&Temp, sizeof(ZEVector2d), 1) != 1)
					return false;
				Element.Value.SetVector2d(Temp);
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

			case ZEML_VT_VECTOR3D:
			{
				ZEVector3d Temp;
				if (File->Read(&Temp, sizeof(ZEVector3d), 1) != 1)
					return false;
				Element.Value.SetVector3d(Temp);
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

			case ZEML_VT_VECTOR4D:
			{
				ZEVector4d Temp;
				if (File->Read(&Temp, sizeof(ZEVector4d), 1) != 1)
					return false;
				Element.Value.SetVector4d(Temp);
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

			case ZEML_VT_MATRIX3X3D:
			{
				ZEMatrix3x3d Temp;
				if (File->Read(&Temp, sizeof(ZEMatrix3x3d), 1) != 1)
					return false;
				Element.Value.SetMatrix3x3d(Temp);
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

			case ZEML_VT_MATRIX4X4D:
			{
				ZEMatrix4x4d Temp;
				if (File->Read(&Temp, sizeof(ZEMatrix4x4d), 1) != 1)
					return false;
				Element.Value.SetMatrix4x4d(Temp);
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
	const char Identifer[6] =
	{
		'Z', 'E', 'M', 'L',
		1, // Major Version
		0 // Minor Version
	};
	File->Write(Identifer, 6, 1);

	ZEUInt64 StartOffset = ZEEndian::Little(File->Tell() + sizeof(ZEUInt64));
	if (!File->Write(&StartOffset, sizeof(ZEUInt64), 1) != 1)
		return false;

	return true;
}

bool ZEMLFormatBinaryV1::WriteHeaderClose(ZEFile* File)
{
	return true;
}

bool ZEMLFormatBinaryV1::WriteElement(ZEFile* File, ZEMLFormatElement& Element)
{
	char Header[2];
	Header[0] = 'Z';
	Header[1] = Element.ElementType;
	if (File->Write(Header, 2 * sizeof(char), 1) != 1)
		return false;

	ZESize NameSizeFull = Element.Name.GetLength() + 1;
	if (NameSizeFull >= ZEML_MAX_NAME_SIZE)
		return false;

	ZEUInt8 NameSize = (ZEUInt8)NameSizeFull;
	if (File->Write(&NameSize, sizeof(ZEUInt8), 1) != 1)
		return false;

	if (File->Write(Element.Name.ToCString(), NameSize, 1) != 1)
		return false;

	if (Element.ElementType == ZEML_ET_NODE)
	{
		ZEUInt64 NodeSize = 0;
		if (File->Write(&NodeSize, sizeof(ZEUInt64), 1) != 1)
			return false;

		ZEUInt64 ElementCount = 0;
		if (File->Write(&ElementCount, sizeof(ZEUInt64), 1) != 1)
			return false;

		Element.Offset = File->Tell();

		return true;
	}
	else if (Element.ElementType == ZEML_ET_PROPERTY)
	{
		ZEUInt8 ValueType = Element.ValueType;
		if (File->Write(&ValueType, sizeof(char), 1) != 1)
			return false;

		switch (Element.ValueType)
		{
			default:
			case ZEML_VT_UNDEFINED:
				return false;

			case ZEML_VT_FLOAT:
			{
				float ValueTemp = Element.Value.GetFloat();
				if (File->Write(&ValueTemp, sizeof(float), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_DOUBLE:
			{
				double ValueTemp = Element.Value.GetDouble();
				if (File->Write(&ValueTemp, sizeof(double), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_INT8:
			{
				ZEInt8 ValueTemp = Element.Value.GetInt8();
				if (File->Write(&ValueTemp, sizeof(ZEInt8), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_INT16:
			{
				ZEInt16 ValueTemp = ZEEndian::Little(Element.Value.GetInt16());
				if (File->Write(&ValueTemp, sizeof(ZEInt16), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_INT32:
			{
				ZEInt32 ValueTemp = ZEEndian::Little(Element.Value.GetInt32());
				if (File->Write(&ValueTemp, sizeof(ZEInt32), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_INT64:
			{
				ZEInt64 ValueTemp = ZEEndian::Little(Element.Value.GetInt64());
				if (File->Write(&ValueTemp, sizeof(ZEInt64), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_UINT8:
			{
				ZEUInt8 ValueTemp = Element.Value.GetUInt8();
				if (File->Write(&ValueTemp, sizeof(ZEUInt8), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_UINT16:
			{
				ZEUInt16 ValueTemp = ZEEndian::Little(Element.Value.GetUInt16());
				if (File->Write(&ValueTemp, sizeof(ZEUInt16), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_UINT32:
			{
				ZEUInt32 ValueTemp = ZEEndian::Little(Element.Value.GetUInt32());
				if (File->Write(&ValueTemp, sizeof(ZEUInt32), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_UINT64:
			{
				ZEUInt64 ValueTemp = ZEEndian::Little(Element.Value.GetUInt64());
				if (File->Write(&ValueTemp, sizeof(ZEUInt64), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_BOOLEAN:
			{
				bool ValueTemp = Element.Value.GetBoolean();
				if (File->Write(&ValueTemp, sizeof(bool), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_STRING:
			{
				ZEUInt32 StringSize = ZEEndian::Little((ZEUInt32)Element.Value.GetString().GetSize());
				if (File->Write(&StringSize, sizeof(ZEUInt32), 1) != 1)
					return false;
				if (StringSize == 0)
					break;
				if (File->Write(Element.Value.GetString().ToCString(), Element.Value.GetString().GetSize() * sizeof(char), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_QUATERNION:
			{
				ZEQuaternion ValueTemp = Element.Value.GetQuaternion();
				if (File->Write(&ValueTemp, sizeof(ZEQuaternion), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_VECTOR2:
			{
				ZEVector2 ValueTemp = Element.Value.GetVector2();
				if (File->Write(&ValueTemp, sizeof(ZEVector2), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_VECTOR2D:
			{
				ZEVector2d ValueTemp = Element.Value.GetVector2d();
				if (File->Write(&ValueTemp, sizeof(ZEVector2d), 1) != 1)
					return false;
				break;
			}


			case ZEML_VT_VECTOR3:
			{
				ZEVector3 ValueTemp = Element.Value.GetVector3();
				if (File->Write(&ValueTemp, sizeof(ZEVector3), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_VECTOR3D:
			{
				ZEVector3d ValueTemp = Element.Value.GetVector3d();
				if (File->Write(&ValueTemp, sizeof(ZEVector3d), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_VECTOR4:
			{
				ZEVector4 ValueTemp = Element.Value.GetVector4();
				if (File->Write(&ValueTemp, sizeof(ZEVector4), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_VECTOR4D:
			{
				ZEVector4d ValueTemp = Element.Value.GetVector4d();
				if (File->Write(&ValueTemp, sizeof(ZEVector4d), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_MATRIX3X3:
			{
				ZEMatrix3x3 ValueTemp = Element.Value.GetMatrix3x3();
				if (File->Write(&ValueTemp, sizeof(ZEMatrix3x3), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_MATRIX3X3D:
			{
				ZEMatrix3x3d ValueTemp = Element.Value.GetMatrix3x3d();
				if (File->Write(&ValueTemp, sizeof(ZEMatrix3x3d), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_MATRIX4X4:
			{
				ZEMatrix4x4 ValueTemp = Element.Value.GetMatrix4x4();
				if (File->Write(&ValueTemp, sizeof(ZEMatrix4x4), 1) != 1)
					return false;
				break;
			}

			case ZEML_VT_MATRIX4X4D:
			{
				ZEMatrix4x4d ValueTemp = Element.Value.GetMatrix4x4d();
				if (File->Write(&ValueTemp, sizeof(ZEMatrix4x4d), 1) != 1)
					return false;
				break;
			}
		}
	}
	else if (Element.ElementType == ZEML_ET_DATA)
	{
		void* DataPointer = (void*)Element.Value.GetUInt64();
		ZEUInt64 DataSize = ZEEndian::Little(Element.Size);
		if (File->Write(&DataSize, sizeof(ZEUInt64), 1) != 1)
			return false;

		if (File->Write(DataPointer, DataSize, 1) != 1)
			return false;
	}
	else
	{
		return false;
	}

	return true;
}

bool ZEMLFormatBinaryV1::WriteElementClose(ZEFile* File, ZEMLFormatElement& Element)
{
	if (Element.ElementType == ZEML_ET_NODE)
	{
		ZEUInt64 CurrentFilePos = File->Tell();
		File->Seek(Element.Offset - 2 * sizeof(ZEUInt64), ZE_SF_BEGINING);

		ZEUInt64 NodeSize = ZEEndian::Little(CurrentFilePos - Element.Offset);
		if (File->Write(&NodeSize, sizeof(ZEUInt64), 1) != 1)
			return false;

		ZEUInt64 NodeCount = ZEEndian::Little(Element.Count);
		if (File->Write(&NodeCount, sizeof(ZEUInt64), 1) != 1)
			return false;

		File->Seek(CurrentFilePos, ZE_SF_BEGINING);
	}

	return true;
}
