//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLFormatXMLV1.cpp
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

#include "ZEMLFormatXMLV1.h"

#include "ZEEndian.h"
#include "ZEBase64.h"
#include "ZEDS\ZEFormat.h"
#include "ZEFile\ZEFile.h"
#include "ZEPointer\ZEPointer.h"

#include <tinyxml.h>
#include "ZERegEx\ZERegEx.h"

#define ZEML_ITEM_FILE_IDENTIFIER	'Z'
#define ZEML_MAX_NAME_SIZE			256

struct ZEMLFormatXMLV1Description : public ZEMLFormatDescription
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

const char* ZEMLFormatXMLV1Description::GetName() const
{
	return "ZEML Binary V1.0";
}

ZEUInt ZEMLFormatXMLV1Description::GetMajorVersion() const
{
	return 1;
}

ZEUInt ZEMLFormatXMLV1Description::GetMinorVersion() const
{
	return 0;
}

ZEMLFormatType ZEMLFormatXMLV1Description::GetType() const
{
	return ZEML_FT_BINARY;
}

ZEMFormatSupport ZEMLFormatXMLV1Description::GetSupport() const
{
	return ZEML_FS_WRITE | ZEML_FS_READ;
}

bool ZEMLFormatXMLV1Description::Determine(ZEFile* File)
{
	char StartBuffer[1024];
	memset(StartBuffer, 0, sizeof(StartBuffer));

	File->Read(StartBuffer, 1, 100);
	StartBuffer[1023] = '\0';

	ZERegEx RegEx("<\?ZEML\?>");
	if (RegEx.Match(StartBuffer))
		return true;

	if (File->GetSize() >= 64 * 1024)
		zeWarning("Loading XML based ZEML format which is slow and has high memory allocation. Please convert it to a faster binary ZEML format.");

	return true;
}

ZEMLFormat* ZEMLFormatXMLV1Description::CreateInstance()
{
	return new ZEMLFormatXMLV1();
}

ZEMLFormatDescription* ZEMLFormatXMLV1::Description()
{
	static ZEMLFormatXMLV1Description Description;
	return &Description;
}

void ZEMLFormatXMLV1::PrintC14NSpace(ZEFile* File)
{
	for (ZESize I = 0; I < C14NDepth; I++)
		File->Write(" ", 1, 1);
}

ZEMLFormatDescription* ZEMLFormatXMLV1::GetDescription()
{
	return Description();
}

bool ZEMLFormatXMLV1::ReadHeader(ZEFile* File)
{
	File->Seek(0, ZE_SF_BEGINING);

	ZESize FileSize = File->GetSize();
	ZEPointer<char> Buffer = new char[FileSize + 1];
	if (File->Read(Buffer, FileSize, 1) != 0)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	Buffer[FileSize] = '\0';
	if (!Document->Parse(Buffer))
	{
		zeError("Cannot parse ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}
	
	return true;
}

bool ZEMLFormatXMLV1::ReadGoToNode(ZEFile* File, const ZEMLFormatElement& Element)
{
	return false;
}

bool ZEMLFormatXMLV1::ReadElement(ZEFile* File, ZEMLFormatElement& Element)
{
	return false;
}

bool ZEMLFormatXMLV1::ReadData(ZEFile* File, const ZEMLFormatElement& Element, void* Buffer, ZESize Offset, ZESize Size)
{
	return false;
}

bool ZEMLFormatXMLV1::WriteHeader(ZEFile* File)
{
	const char Header[] =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<?ZEML?>\n"
		"<ZEML MajorVersion=\"1\" MinorVersion=\"0\">\n";

	if (File->Write(Header, sizeof(Header) - 1, 1) != 1)
		return false;

	C14NDepth = 1;

	return true;
}

bool ZEMLFormatXMLV1::WriteHeaderClose(ZEFile* File)
{
	const char Footer[] = "</ZEML>";
	if (File->Write(Footer, sizeof(Footer) - 1, 1) != 1)
		return false;

	return true;
}

static const char* Convert(ZEMLValueType Type)
{
	switch (Type)
	{
		default:
		case ZEML_VT_UNDEFINED:
			return NULL;

		case ZEML_VT_FLOAT:
			return "Float";

		case ZEML_VT_DOUBLE:
			return "Double";

		case ZEML_VT_INT8:
			return "Int8";

		case ZEML_VT_INT16:
			return "Int16";

		case ZEML_VT_INT32:
			return "Int32";

		case ZEML_VT_INT64:
			return "Int64";

		case ZEML_VT_UINT8:
			return "UInt8";

		case ZEML_VT_UINT16:
			return "UInt16";

		case ZEML_VT_UINT32:
			return "UInt32";

		case ZEML_VT_UINT64:
			return "UInt64";

		case ZEML_VT_BOOLEAN:
			return "Bool";

		case ZEML_VT_STRING:
			return "String";

		case ZEML_VT_QUATERNION:
			return "Quaternion";

		case ZEML_VT_VECTOR2:
			return "Vector2";

		case ZEML_VT_VECTOR3:
			return "Vector3";

		case ZEML_VT_VECTOR4:
			return "Vector4";

		case ZEML_VT_MATRIX3X3:
			return "Matrix3x3";

		case ZEML_VT_MATRIX4X4:
			return "Matrix4x4";
	}
}

bool ZEMLFormatXMLV1::WriteElement(ZEFile* File, ZEMLFormatElement& Element)
{
	PrintC14NSpace(File);
	switch(Element.ElementType)
	{
		case ZEML_ET_NODE:
		{	ZEString Output = ZEFormat::Format("<Node Name=\"{0}\">\n", Element.Name);
			if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
				return false;

			C14NDepth++;
			break;
		}

		case ZEML_ET_PROPERTY:
		{
			switch(Element.ValueType)
			{
				default:
				{
					const char* TypeText = Convert(Element.ValueType);
					if (TypeText == NULL)
						return false;

					ZEString Output	= ZEFormat::Format("<Property Name=\"{0}\" Type=\"{1}\">{2}</Property>\n", Element.Name, TypeText, Element.Value);
					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}

				case ZEML_VT_STRING:
				{
					TiXmlString String(Element.Value.GetString().ToCString());
					TiXmlString EncodedString;
					TiXmlBase::EncodeString(String, &EncodedString);

					ZEString Output	= ZEFormat::Format("<Property Name=\"{0}\" Type=\"String\">{1}</Property>\n", Element.Name, EncodedString.c_str());
					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}

				case ZEML_VT_VECTOR2:
				{
					const ZEVector2& Vector = Element.Value.GetVector2();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Vector2\">"
							"<x>{1}</x><y>{2}</y>"
						"</Property>\n", 
						Element.Name,
						Vector.x, Vector.y);

					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}
				
				case ZEML_VT_VECTOR3:
				{
					const ZEVector3& Vector = Element.Value.GetVector3();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Vector3\">"
						"<x>{1}</x><y>{2}</y><z>{3}</z>"
						"</Property>\n", 
						Element.Name, 
						Vector.x, Vector.y, Vector.z);

					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}
				
				case ZEML_VT_VECTOR4:
				{
					const ZEVector4& Vector = Element.Value.GetVector4();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Vector4\">"
						"<x>{1}</x><y>{2}</y><z>{3}</z><w>{4}</w>"
						"</Property>\n", 
						Element.Name,
						Vector.x, Vector.y, Vector.z, Vector.w);

					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
				}
				
				case ZEML_VT_QUATERNION:
				{
					const ZEQuaternion& Quaternion = Element.Value.GetQuaternion();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Quaternion3x3\">"
						"<w>{1}</w><x>{2}</x><y>{3}</y><z>{4}</z>"
						"</Property>\n", 
						Element.Name,
						Quaternion.w, Quaternion.x, Quaternion.y, Quaternion.z);

					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}
				
				case ZEML_VT_MATRIX3X3:
				{
					const ZEMatrix3x3& Matrix = Element.Value.GetMatrix3x3();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Matrix3x3\">"
						"<m11>{1}</m11><m12>{2}</m12><m13>{3}</m13>"
						"<m21>{4}</m21><m22>{5}</m22><m23>{6}</m23>"
						"<m31>{7}</m31><m32>{8}</m32><m33>{9}</m33>"
						"</Property>\n", 
						Element.Name,
						Matrix.M11, Matrix.M12, Matrix.M13,
						Matrix.M21, Matrix.M22, Matrix.M23,
						Matrix.M31, Matrix.M32, Matrix.M33);

					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}
				
				case ZEML_VT_MATRIX4X4:
				{
					const ZEMatrix4x4& Matrix = Element.Value.GetMatrix4x4();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Matrix3x3\">"
						"<m11>{1}</m11><m12>{2}</m12><m13>{3}</m13><m14>{4}</m14>"
						"<m21>{5}</m21><m22>{6}</m22><m23>{7}</m23><m24>{8}</m24>"
						"<m31>{9}</m31><m32>{10}</m32><m33>{11}</m33><m34>{12}</m34>"
						"<m31>{13}</m31><m32>{14}</m32><m33>{15}</m33><m34>{16}</m34>"
						"</Property>\n", 
						Element.Name,
						Matrix.M11, Matrix.M12, Matrix.M13, Matrix.M14,
						Matrix.M21, Matrix.M22, Matrix.M23, Matrix.M24,
						Matrix.M31, Matrix.M32, Matrix.M33, Matrix.M34,
						Matrix.M41, Matrix.M42, Matrix.M43, Matrix.M44);

					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}
			}
			break;
		}

		case ZEML_ET_DATA:
		{
			ZEString Output = ZEFormat::Format("<Data Name=\"{0}\" Size=\"{1}\">", Element.Name, Element.Size);
			if (File->Write(Output, Output.GetLength(), 1) != 1)
				return false;

			ZESize Base64Size = ZEBase64::EncodeSize(Element.Size);
			ZEPointer<char> Base64Buffer = new char[Base64Size];
			ZEBase64::Encode(Base64Buffer, (void*)Element.Value.GetUInt64(), Element.Size);
			if (File->Write(Base64Buffer, Base64Size, 1) != 1)
				return false;
			
			if (!File->Write("</Data>\n", 8, 1))
				return false;

			break;
			
		}
	};
	
	return true;
}

bool ZEMLFormatXMLV1::WriteElementClose(ZEFile* File, ZEMLFormatElement& Element)
{
	if (Element.ElementType == ZEML_ET_NODE)
	{
		C14NDepth--;
		PrintC14NSpace(File);
		if (File->Write("</Node>\n", 8, 1) != 1)
			return false;
	}

	return true;
}
