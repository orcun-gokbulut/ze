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
#include "ZERegEx\ZERegEx.h"
#include "ZEError.h"

#include <tinyxml.h>
#include "ZEFile\ZEFileInfo.h"

#define FormatError(Text) zeError(Text##" File Name: \"%s\", Element: \"%s\".", File->GetPath().ToCString(), CurrentNode->Attribute("Name") != NULL ? CurrentNode->Attribute("Name") : "Unknown")

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
	return "ZEML XML Format v1.0";
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

	ZERegEx RegEx("<\\?ZEML\\s*VersionMajor\\s*=\\s*\"(.*)\"\\s*VersionMinor\\s*=\\s*\"(.*)\"\\s*\\?>");
	ZERegExMatch Match;
	if (!RegEx.Match(StartBuffer, Match))
		return false;

	if (Match.SubMatches[0].String.ToInt32() != 1)
	{
		zeWarning("Unknown XML based ZEML format version detected. File Name: \"%s\"", File->GetPath());
		return false;
	}

	if (Match.SubMatches[1].String.ToInt32() != 0)
		zeWarning("Unknown XML based ZEML format minor version is detected. Can cause problems. File Name: \"%s\"", File->GetPath());

	if (File->GetSize() >= 64 * 1024)
		zeWarning("Loading XML based ZEML format which is slow and has high memory allocation. Please convert it to a faster binary ZEML format. File Name: \"%s\"", File->GetPath());

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


ZEMLFormatDescription* ZEMLFormatXMLV1::GetDescription() const
{
	return Description();
}


void ZEMLFormatXMLV1::PrintC14NSpace(ZEFile* File)
{
	for (ZESize I = 0; I < C14NDepth; I++)
		File->Write(" ", 1, 1);
}

bool ZEMLFormatXMLV1::ReadVectors(ZEFile* File, float* Output, const char** Members, ZESize MemberCount)
{
	bool ReadedMembers[32];
	memset(&ReadedMembers, 0, sizeof(bool) * MemberCount);

	TiXmlNode* Node = CurrentNode->FirstChild();
	while(Node != NULL)
	{
		if (Node->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			Node = Node->NextSibling();
			continue;
		}

		const char* Name = Node->Value();
		for (ZESize I = 0; I < MemberCount; I++)
		{
			if (strcmp(Members[I], Name) == 0)
			{
				if (ReadedMembers[I] == true)
				{
					FormatError("Multiple entries for the same vector/matrix elements detected.");
					return false;
				}

				const char* Value = ((TiXmlElement*)Node)->GetText();
				Output[I] = ZEString(Value).ToFloat();
				ReadedMembers[I] = true;
				break;
			}
		}

		Node = Node->NextSibling();
	}

	for (ZESize I = 0; I < MemberCount; I++)
	{
		if (!ReadedMembers[I])
		{
			FormatError("Missing vector/matrix entires in vector/matrix property have detected.");
			return false;
		}
	}

	return true;
}

bool ZEMLFormatXMLV1::Include(ZEFile* File, TiXmlElement* Node)
{
	const char* Source = Node->Attribute("Source");
	if (Source == NULL)
	{
		FormatError("ZEML Include element does not have Source attribute.");
		return false;
	}

	bool DiscardRoot = false;
	const char* DiscardRootAttribute = Node->Attribute("DiscardRoot");
	if (DiscardRootAttribute != NULL && strcmp(DiscardRootAttribute, "true") == 0)
		DiscardRoot = true;


	ZEFile IncludeFile;
	if (!IncludeFile.Open(ZEFileInfo::CombineRelativePath(File->GetPath(), Source), ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot open ZEML Include file source. Source: \"%s\".", Source);
		return false;
	}

	ZESize FileSize = IncludeFile.GetSize();
	ZEArray<char> Buffer;
	Buffer.SetCount(FileSize + 1);
	if (IncludeFile.Read(Buffer.GetCArray(), FileSize, 1) != 1)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", IncludeFile.GetPath().ToCString());
		IncludeFile.Close();
		return false;
	}

	IncludeFile.Close();

	Buffer[FileSize] = '\0';
	ZEPointer<TiXmlDocument> IncludeDocument = new TiXmlDocument();
	IncludeDocument->Parse(Buffer.GetConstCArray(), 0, TIXML_ENCODING_UTF8);
	if (IncludeDocument->Error())
	{
		zeError("Cannot parse XML in ZEML file. Corrupted ZEML file. File Name: \"%s\". Error Description: %s", IncludeFile.GetPath().ToCString(), IncludeDocument->ErrorDesc());
		return false;
	}

	if (strcmp(IncludeDocument->RootElement()->Value(), "Node") != 0)
	{
		zeError("XML based ZEML file must have <Node> tag as root element. File Name: \"%s\".", IncludeFile.GetPath().ToCString());
		return false;
	}

	if (DiscardRoot)
	{
		TiXmlNode* IncludeNode = IncludeDocument->RootElement()->FirstChild();
		while (IncludeNode != NULL)
		{
			Node->Parent()->InsertBeforeChild(Node, *IncludeNode->Clone());
			IncludeNode = IncludeNode->NextSibling();
		}
	}
	else
	{
		Node->Parent()->InsertBeforeChild(Node, *IncludeDocument->RootElement()->Clone());
	}
	IncludeDocument.Release();

	return true;
}

bool ZEMLFormatXMLV1::ParseIncludes(ZEFile* File, TiXmlElement* Node)
{
	TiXmlNode* IncludeNode = Node->FirstChild();
	while (IncludeNode != NULL)
	{
		if (IncludeNode->Type() != TiXmlNode::TINYXML_ELEMENT ||
			strcmp(IncludeNode->Value(), "Include") != 0)
		{
			IncludeNode = IncludeNode->NextSibling();
			continue;
		}

		if (!Include(File, IncludeNode->ToElement()))
			return false;

		TiXmlNode* NextNode = IncludeNode->NextSibling();
		IncludeNode->Parent()->RemoveChild(IncludeNode);
		IncludeNode = NextNode;
	}

	return true;
}

bool ZEMLFormatXMLV1::ReadDoubleVectors(ZEFile* File, double* Output, const char** Members, ZESize MemberCount)
{
	bool ReadedMembers[32];
	memset(&ReadedMembers, 0, sizeof(bool) * MemberCount);

	TiXmlNode* Node = CurrentNode->FirstChild();
	while(Node != NULL)
	{
		if (Node->Type() != TiXmlNode::TINYXML_ELEMENT)
		{
			Node = Node->NextSibling();
			continue;
		}

		const char* Name = Node->Value();
		for (ZESize I = 0; I < MemberCount; I++)
		{
			if (strcmp(Members[I], Name) == 0)
			{
				if (ReadedMembers[I] == true)
				{
					FormatError("Multiple entries for the same vector/matrix elements detected.");
					return false;
				}

				const char* Value = ((TiXmlElement*)Node)->GetText();
				Output[I] = ZEString(Value).ToDouble();
				ReadedMembers[I] = true;
				break;
			}
		}

		Node = Node->NextSibling();
	}

	for (ZESize I = 0; I < MemberCount; I++)
	{
		if (!ReadedMembers[I])
		{
			FormatError("Missing vector/matrix entires in vector/matrix property have detected.");
			return false;
		}
	}

	return true;
}

bool ZEMLFormatXMLV1::ReadHeader(ZEFile* File)
{
	File->Seek(0, ZE_SF_BEGINING);

	ZESize FileSize = File->GetSize();
	ZEArray<char> Buffer;
	Buffer.SetCount(FileSize + 1);
	if (File->Read(Buffer.GetCArray(), FileSize, 1) != 1)
	{
		zeError("Cannot load ZEML file. Corrupted ZEML file. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	Buffer[FileSize] = '\0';
	Document = new TiXmlDocument();
	Document->Parse(Buffer.GetConstCArray(), 0, TIXML_ENCODING_UTF8);
	if (Document->Error())
	{
		zeError("Cannot parse XML in ZEML file. Corrupted ZEML file. File Name: \"%s\". Error Description: %s", File->GetPath().ToCString(), Document->ErrorDesc());
		return false;
	}

	if (strcmp(Document->RootElement()->Value(), "Node") != 0)
	{
		zeError("XML base ZEML file must have <ZEML> tag as root element. File Name: \"%s\".", File->GetPath().ToCString());
		return false;
	}

	CurrentNode = Document->RootElement();

	return true;
}

bool ZEMLFormatXMLV1::ReadGoToNode(ZEFile* File, const ZEMLFormatElement& Element)
{
	if (Element.ElementType != ZEML_ET_NODE)
		return false;

	TiXmlElement* Node = (TiXmlElement*)Element.Offset;

	bool Found = false;
	TiXmlNode* NextNode = Node->FirstChild();
	while(NextNode != NULL)
	{
		if (NextNode->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			CurrentNode = NextNode->ToElement();
			Found = true;
			break;
		}
		NextNode = NextNode->NextSibling();
	}

	if (!Found)
		CurrentNode = NULL;

	return true;
}


bool ZEMLFormatXMLV1::ReadElement(ZEFile* File, ZEMLFormatElement& Element)
{
	if (CurrentNode == NULL)
		return false;

	const char* Name = CurrentNode->Attribute("Name");
	if (Name == NULL)
	{
		FormatError("ZEML element does not have Name attribute.");
		return false;
	}

	if (strcmp(CurrentNode->Value(), "Node") == 0)
	{
		Element.ElementType = ZEML_ET_NODE;
		Element.Name = CurrentNode->Attribute("Name");
		Element.NameHash = Element.Name.Hash();
		Element.Count = 0;
		Element.Offset = (ZEUInt64)CurrentNode;

		if (!ParseIncludes(File, CurrentNode))
			return false;

		TiXmlNode* Node = CurrentNode->FirstChild();
		while(Node != NULL)
		{
			if (Node->Type() == TiXmlNode::TINYXML_ELEMENT)
				Element.Count++;
			Node = Node->NextSibling();
		}
	}
	else if (strcmp(CurrentNode->Value(), "Property") == 0)
	{
		const char* Type = CurrentNode->Attribute("Type");
		if (Type == NULL)
		{
			FormatError("ZEML Property element does not have Type attribute.");
			return false;
		}
		
		Element.ElementType = ZEML_ET_PROPERTY;
		Element.Name = Name;
		Element.NameHash = Element.Name.Hash();

		if (strcmp(Type, "UInt8") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_UINT8;
			Element.Value.SetUInt8(ZEString(Text).ToUInt8());
		}
		else if (strcmp(Type, "UInt16") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_UINT16;
			Element.Value.SetUInt16(ZEString(Text).ToUInt16());
		}
		else if (strcmp(Type, "UInt32") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_UINT32;
			Element.Value.SetUInt32(ZEString(Text).ToUInt32());
		}
		else if (strcmp(Type, "UInt64") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");	
				return false;
			}

			Element.ValueType = ZEML_VT_UINT64;
			Element.Value.SetUInt64(ZEString(Text).ToUInt64());
		}
		else if (strcmp(Type, "Int8") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");			
				return false;
			}

			Element.ValueType = ZEML_VT_INT8;
			Element.Value.SetInt8(ZEString(Text).ToInt8());
		}
		else if (strcmp(Type, "Int16") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_INT16;
			Element.Value.SetInt16(ZEString(Text).ToInt16());
		}
		else if (strcmp(Type, "Int32") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_INT32;
			Element.Value.SetInt32(ZEString(Text).ToInt32());
		}
		else if (strcmp(Type, "Int64") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_INT64;
			Element.Value.SetInt64(ZEString(Text).ToUInt64());
		}
		else if (strcmp(Type, "Float") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_FLOAT;
			Element.Value.SetFloat(ZEString(Text).ToFloat());
		}
		else if (strcmp(Type, "Double") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_DOUBLE;
			Element.Value.SetDouble(ZEString(Text).ToDouble());
		}
		else if (strcmp(Type, "Bool") == 0)
		{
			const char* Text = CurrentNode->GetText();
			if (Text == NULL)
			{
				FormatError("Not valid property value.");
				return false;
			}

			Element.ValueType = ZEML_VT_BOOLEAN;
			if (stricmp(Text, "True") == 0)
				Element.Value.SetBoolean(true);
			else if (stricmp(Text, "False") == 0)
				Element.Value.SetBoolean(false);
			else
			{
				FormatError("Not valid property value.");
				return false;
			}
		}
		else if (strcmp(Type, "String") == 0)
		{
			const char* Text = CurrentNode->GetText();
			Element.ValueType = ZEML_VT_STRING;
			Element.Value.SetString(Text);
		}
		else if (strcmp(Type, "Vector2") == 0)
		{
			Element.ValueType = ZEML_VT_VECTOR2;

			const char* Members[2] = {"x", "y"};
			float Output[2];
			if (!ReadVectors(File, Output, Members, 2))
				return false;
			Element.Value.SetVector2(*(ZEVector2*)Output);
		}
		else if (strcmp(Type, "Vector2d") == 0)
		{
			Element.ValueType = ZEML_VT_VECTOR2D;

			const char* Members[2] = {"x", "y"};
			double Output[2];
			if (!ReadDoubleVectors(File, Output, Members, 2))
				return false;
			Element.Value.SetVector2d(*(ZEVector2d*)Output);
		}
		else if (strcmp(Type, "Vector3") == 0)
		{
			Element.ValueType = ZEML_VT_VECTOR3;

			const char* Members[3] = {"x", "y", "z"};
			float Output[3];
			if (!ReadVectors(File, Output, Members, 3))
				return false;
			Element.Value.SetVector3(*(ZEVector3*)Output);
		}
		else if (strcmp(Type, "Vector3d") == 0)
		{
			Element.ValueType = ZEML_VT_VECTOR3D;

			const char* Members[3] = {"x", "y", "z"};
			double Output[3];
			if (!ReadDoubleVectors(File, Output, Members, 3))
				return false;
			Element.Value.SetVector3d(*(ZEVector3d*)Output);
		}
		else if (strcmp(Type, "Vector4") == 0)
		{
			Element.ValueType = ZEML_VT_VECTOR4;

			const char* Members[4] = {"x", "y", "z", "w"};
			float Output[4];
			if (!ReadVectors(File, Output, Members, 4))
				return false;
			Element.Value.SetVector4(*(ZEVector4*)Output);
		}
		else if (strcmp(Type, "Vector4d") == 0)
		{
			Element.ValueType = ZEML_VT_VECTOR4D;

			const char* Members[4] = {"x", "y", "z", "w"};
			double Output[4];
			if (!ReadDoubleVectors(File, Output, Members, 4))
				return false;
			Element.Value.SetVector4d(*(ZEVector4d*)Output);
		}
		else if (strcmp(Type, "Quaternion") == 0)
		{
			Element.ValueType = ZEML_VT_VECTOR4;

			const char* Members[4] = {"w", "x", "y", "z"};
			float Output[4];
			if (!ReadVectors(File, Output, Members, 4))
				return false;
			Element.Value.SetQuaternion(*(ZEQuaternion*)Output);
		}
		else if (strcmp(Type, "Matrix3x3") == 0)
		{
			Element.ValueType = ZEML_VT_MATRIX3X3;

			const char* Members[9] = 
			{
				"M11", "M21", "M31",
				"M12", "M22", "M32",
				"M13", "M23", "M33",
			};
			float Output[9];
			if (!ReadVectors(File, Output, Members, 9))
				return false;
			Element.Value.SetMatrix3x3(*(ZEMatrix3x3*)Output);
		}
		else if (strcmp(Type, "Matrix3x3d") == 0)
		{
			Element.ValueType = ZEML_VT_MATRIX3X3D;

			const char* Members[9] = 
			{
				"M11", "M21", "M31",
				"M12", "M22", "M32",
				"M13", "M23", "M33",
			};
			double Output[9];
			if (!ReadDoubleVectors(File, Output, Members, 9))
				return false;
			Element.Value.SetMatrix3x3d(*(ZEMatrix3x3d*)Output);
		}
		else if (strcmp(Type, "Matrix4x4") == 0)
		{
			Element.ValueType = ZEML_VT_MATRIX4X4;

			const char* Members[16] = 
			{
				"M11", "M21", "M31", "M41",
				"M12", "M22", "M32", "M42",
				"M13", "M23", "M33", "M43",
				"M14", "M24", "M34", "M44"
			};
			float Output[16];
			if (!ReadVectors(File, Output, Members, 16))
				return false;
			Element.Value.SetMatrix4x4(*(ZEMatrix4x4*)Output);
		}
		else if (strcmp(Type, "Matrix4x4d") == 0)
		{
			Element.ValueType = ZEML_VT_MATRIX4X4D;

			const char* Members[16] = 
			{
				"M11", "M21", "M31", "M41",
				"M12", "M22", "M32", "M42",
				"M13", "M23", "M33", "M43",
				"M14", "M24", "M34", "M44"
			};
			double Output[16];
			if (!ReadDoubleVectors(File, Output, Members, 16))
				return false;
			Element.Value.SetMatrix4x4d(*(ZEMatrix4x4d*)Output);
		}
		else
		{
			FormatError("Unknown property type.");
		}
	}
	else if (strcmp(CurrentNode->Value(), "Data") == 0)
	{
		const char* Size = CurrentNode->Attribute("Size");
		if (Size == NULL)
		{
			FormatError("ZEML Data element does not have Size attribute.");
			return false;
		}
		
		Element.ElementType = ZEML_ET_DATA;
		Element.Name = Name;
		Element.NameHash = Element.Name.Hash();
		Element.Size = ZEString(Size).ToUInt64();
		Element.Offset = (ZEUInt64)CurrentNode;
	}
	else
	{
		FormatError("Unknown element type.");
	}

	bool Found = false;
	TiXmlNode* NextNode = CurrentNode->NextSibling();
	while(NextNode != NULL)
	{
		if (NextNode->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			CurrentNode = NextNode->ToElement();
			Found = true;
			break;
		}
		NextNode = NextNode->NextSibling();
	}
		
	if (!Found)
		CurrentNode = NULL;

	return true;
}

bool ZEMLFormatXMLV1::ReadData(ZEFile* File, const ZEMLFormatElement& Element, void* Buffer, ZESize Offset, ZESize Size)
{
	TiXmlElement* DataElement = (TiXmlElement*)Element.Offset;
	
	if (Offset == 0 && Size == Element.Size)
	{
		if (!ZEBase64::Decode(Buffer, DataElement->GetText(), ZEBase64::EncodeSize(Element.Size)))
		{
			FormatError("Base64 decoding has failed.");
			return false;
		}
	}
	else
	{
		ZEPointer<ZEBYTE> Data = new ZEBYTE[Element.Size];
		if (!ZEBase64::Decode(Data, DataElement->GetText(), ZEBase64::EncodeSize(Element.Size)))
		{
			FormatError("Base64 decoding has failed.");
			return false;
		}
		memcpy(Buffer, (ZEBYTE*)Data + Offset, Size);
	}

	return true;
}

bool ZEMLFormatXMLV1::WriteHeader(ZEFile* File)
{
	const char Header[] =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<?ZEML VersionMajor=\"1\" VersionMinor=\"0\"?>\n";

	if (File->Write(Header, sizeof(Header) - 1, 1) != 1)
		return false;

	C14NDepth = 1;

	return true;
}

bool ZEMLFormatXMLV1::WriteHeaderClose(ZEFile* File)
{
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

		case ZEML_VT_VECTOR2D:
			return "Vector2d";

		case ZEML_VT_VECTOR3:
			return "Vector3";

		case ZEML_VT_VECTOR3D:
			return "Vector3d";

		case ZEML_VT_VECTOR4:
			return "Vector4";

		case ZEML_VT_VECTOR4D:
			return "Vector4d";

		case ZEML_VT_MATRIX3X3:
			return "Matrix3x3";

		case ZEML_VT_MATRIX3X3D:
			return "Matrix3x3d";

		case ZEML_VT_MATRIX4X4:
			return "Matrix4x4";

		case ZEML_VT_MATRIX4X4D:
			return "Matrix4x4d";
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

				case ZEML_VT_VECTOR2D:
				{
					const ZEVector2d& Vector = Element.Value.GetVector2d();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Vector2d\">"
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

				case ZEML_VT_VECTOR3D:
				{
					const ZEVector3d& Vector = Element.Value.GetVector3d();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Vector3d\">"
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
					break;
				}

				case ZEML_VT_VECTOR4D:
				{
					const ZEVector4d& Vector = Element.Value.GetVector4d();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Vector4d\">"
						"<x>{1}</x><y>{2}</y><z>{3}</z><w>{4}</w>"
						"</Property>\n", 
						Element.Name,
						Vector.x, Vector.y, Vector.z, Vector.w);

					if (File->Write(Output.ToCString(), Output.GetLength(), 1) != 1)
						return false;
					break;
				}
				
				case ZEML_VT_QUATERNION:
				{
					const ZEQuaternion& Quaternion = Element.Value.GetQuaternion();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Quaternion\">"
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

				case ZEML_VT_MATRIX3X3D:
				{
					const ZEMatrix3x3d& Matrix = Element.Value.GetMatrix3x3d();
					ZEString Output	= ZEFormat::Format(
						"<Property Name=\"{0}\" Type=\"Matrix3x3d\">"
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

				case ZEML_VT_MATRIX4X4D:
				{
					const ZEMatrix4x4d& Matrix = Element.Value.GetMatrix4x4d();
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
