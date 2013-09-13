//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLProperty.cpp
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

#include "ZEMLProperty.h"
#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEEndian.h"
#include "TinyXML.h"

ZEMLProperty::ZEMLProperty()
{
	SetType(ZEML_IT_UNDEFINED);
}

ZEMLProperty::ZEMLProperty(const ZEString& Name)
{
	SetName(Name);
}

ZEMLProperty::ZEMLProperty(const ZEString& Name ,const ZEVariant& Value)
{
	SetName(Name);
	SetValue(Value);
}

ZEUInt64 ZEMLProperty::GetTotalSize()
{
	return sizeof(char) + sizeof(ZEUInt8) + sizeof(ZEUInt8) + GetName().GetSize() + sizeof(ZEUInt64) + (ZEUInt64)Value.SizeOf();
}

void ZEMLProperty::SetValue(const ZEVariant& Value)
{
	this->Value = Value;
	DataSize = Value.SizeOf();

	switch (this->Value.GetType())
	{
		case ZE_VRT_FLOAT:
			SetType(ZEML_IT_FLOAT);
			break;
		case ZE_VRT_DOUBLE:
			SetType(ZEML_IT_DOUBLE);
			break;
		case ZE_VRT_INTEGER_8:
			SetType(ZEML_IT_INT8);
			break;
		case ZE_VRT_INTEGER_16:
			SetType(ZEML_IT_INT16);
			break;
		case ZE_VRT_INTEGER_32:
			SetType(ZEML_IT_INT32);
			break;
		case ZE_VRT_INTEGER_64:
			SetType(ZEML_IT_INT64);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_8:
			SetType(ZEML_IT_UINT8);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_16:
			SetType(ZEML_IT_UINT16);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_32:
			SetType(ZEML_IT_UINT32);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
			SetType(ZEML_IT_UINT64);
			break;
		case ZE_VRT_BOOLEAN:
			SetType(ZEML_IT_BOOLEAN);
			break;
		case ZE_VRT_STRING:
			SetType(ZEML_IT_STRING);
			break;
		case ZE_VRT_QUATERNION:
			SetType(ZEML_IT_QUATERNION);
			break;
		case ZE_VRT_VECTOR2:
			SetType(ZEML_IT_VECTOR2);
			break;
		case ZE_VRT_VECTOR3:
			SetType(ZEML_IT_VECTOR3);
			break;
		case ZE_VRT_VECTOR4:
			SetType(ZEML_IT_VECTOR4);
			break;
		case ZE_VRT_MATRIX3X3:
			SetType(ZEML_IT_MATRIX3X3);
			break;
		case ZE_VRT_MATRIX4X4:
			SetType(ZEML_IT_MATRIX4X4);
			break;
		default:
			zeError("Unsupported ZEMLProperty type.");
			break;
	}
}

const ZEVariant& ZEMLProperty::GetValue() const
{
	return Value;
}

bool ZEMLProperty::WriteSelfToXML(TiXmlElement* Element)
{
	if(Element == NULL)
		return false;

	TiXmlElement* PropertElement = new TiXmlElement(GetName());
	PropertElement->SetAttribute("Type", GetTypeText());

	switch (this->Value.GetType())
	{
		case ZE_VRT_FLOAT:
			PropertElement->SetAttribute("Value", ZEString::FromFloat(Value.GetFloat()));
			break;
		case ZE_VRT_DOUBLE:
			PropertElement->SetAttribute("Value", ZEString::FromDouble(Value.GetDouble()));
			break;
		case ZE_VRT_INTEGER_8:
			PropertElement->SetAttribute("Value", ZEString::FromInt8(Value.GetInt8()));
			break;
		case ZE_VRT_INTEGER_16:
			PropertElement->SetAttribute("Value", ZEString::FromInt16(Value.GetInt16()));
			break;
		case ZE_VRT_INTEGER_32:
			PropertElement->SetAttribute("Value", ZEString::FromInt32(Value.GetInt32()));
			break;
		case ZE_VRT_INTEGER_64:
			PropertElement->SetAttribute("Value", ZEString::FromInt64(Value.GetInt64()));
			break;
		case ZE_VRT_UNSIGNED_INTEGER_8:
			PropertElement->SetAttribute("Value", ZEString::FromUInt8(Value.GetUInt8()));
			break;
		case ZE_VRT_UNSIGNED_INTEGER_16:
			PropertElement->SetAttribute("Value", ZEString::FromUInt16(Value.GetUInt16()));
			break;
		case ZE_VRT_UNSIGNED_INTEGER_32:
			PropertElement->SetAttribute("Value", ZEString::FromUInt32(Value.GetUInt32()));
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
			PropertElement->SetAttribute("Value", ZEString::FromUInt64(Value.GetUInt64()));
			break;
		case ZE_VRT_BOOLEAN:
			PropertElement->SetAttribute("Value", ZEString::FromBool(Value.GetBoolean()));
			break;
		case ZE_VRT_STRING:
			{
				if (Value.GetString().GetSize() == 0)
					PropertElement->SetAttribute("Value", "");
				else
					PropertElement->SetAttribute("Value", Value.GetString());
				break;
			}
		case ZE_VRT_QUATERNION:
			PropertElement->SetAttribute("X", ZEString::FromFloat(Value.GetQuaternion().x));
			PropertElement->SetAttribute("Y", ZEString::FromFloat(Value.GetQuaternion().y));
			PropertElement->SetAttribute("Z", ZEString::FromFloat(Value.GetQuaternion().z));
			PropertElement->SetAttribute("W", ZEString::FromFloat(Value.GetQuaternion().w));
			break;
		case ZE_VRT_VECTOR2:
			PropertElement->SetAttribute("X", ZEString::FromFloat(Value.GetVector2().x));
			PropertElement->SetAttribute("Y", ZEString::FromFloat(Value.GetVector2().y));
			break;
		case ZE_VRT_VECTOR3:
			PropertElement->SetAttribute("X", ZEString::FromFloat(Value.GetVector3().x));
			PropertElement->SetAttribute("Y", ZEString::FromFloat(Value.GetVector3().y));
			PropertElement->SetAttribute("Z", ZEString::FromFloat(Value.GetVector3().z));
			break;
		case ZE_VRT_VECTOR4:
			PropertElement->SetAttribute("X", ZEString::FromFloat(Value.GetVector4().x));
			PropertElement->SetAttribute("Y", ZEString::FromFloat(Value.GetVector4().y));
			PropertElement->SetAttribute("Z", ZEString::FromFloat(Value.GetVector4().z));
			PropertElement->SetAttribute("W", ZEString::FromFloat(Value.GetVector4().w));
			break;
		case ZE_VRT_MATRIX3X3:
			PropertElement->SetAttribute("M11", ZEString::FromFloat(Value.GetMatrix3x3().M11));
			PropertElement->SetAttribute("M12", ZEString::FromFloat(Value.GetMatrix3x3().M12));
			PropertElement->SetAttribute("M13", ZEString::FromFloat(Value.GetMatrix3x3().M13));
			PropertElement->SetAttribute("M21", ZEString::FromFloat(Value.GetMatrix3x3().M21));
			PropertElement->SetAttribute("M22", ZEString::FromFloat(Value.GetMatrix3x3().M22));
			PropertElement->SetAttribute("M23", ZEString::FromFloat(Value.GetMatrix3x3().M23));
			PropertElement->SetAttribute("M31", ZEString::FromFloat(Value.GetMatrix3x3().M31));
			PropertElement->SetAttribute("M32", ZEString::FromFloat(Value.GetMatrix3x3().M32));
			PropertElement->SetAttribute("M33", ZEString::FromFloat(Value.GetMatrix3x3().M33));
			break;
		case ZE_VRT_MATRIX4X4:
			PropertElement->SetAttribute("M11", ZEString::FromFloat(Value.GetMatrix4x4().M11));
			PropertElement->SetAttribute("M12", ZEString::FromFloat(Value.GetMatrix4x4().M12));
			PropertElement->SetAttribute("M13", ZEString::FromFloat(Value.GetMatrix4x4().M13));
			PropertElement->SetAttribute("M14", ZEString::FromFloat(Value.GetMatrix4x4().M14));
			PropertElement->SetAttribute("M21", ZEString::FromFloat(Value.GetMatrix4x4().M21));
			PropertElement->SetAttribute("M22", ZEString::FromFloat(Value.GetMatrix4x4().M22));
			PropertElement->SetAttribute("M23", ZEString::FromFloat(Value.GetMatrix4x4().M23));
			PropertElement->SetAttribute("M24", ZEString::FromFloat(Value.GetMatrix4x4().M24));
			PropertElement->SetAttribute("M31", ZEString::FromFloat(Value.GetMatrix4x4().M31));
			PropertElement->SetAttribute("M32", ZEString::FromFloat(Value.GetMatrix4x4().M32));
			PropertElement->SetAttribute("M33", ZEString::FromFloat(Value.GetMatrix4x4().M33));
			PropertElement->SetAttribute("M34", ZEString::FromFloat(Value.GetMatrix4x4().M34));
			PropertElement->SetAttribute("M41", ZEString::FromFloat(Value.GetMatrix4x4().M41));
			PropertElement->SetAttribute("M42", ZEString::FromFloat(Value.GetMatrix4x4().M42));
			PropertElement->SetAttribute("M43", ZEString::FromFloat(Value.GetMatrix4x4().M43));
			PropertElement->SetAttribute("M44", ZEString::FromFloat(Value.GetMatrix4x4().M44));
			break;
		default:
			zeError("Unsupported ZEMLProperty type.");
			return false;
			break;
	}
	
	Element->LinkEndChild(PropertElement);
	return true;
}

bool ZEMLProperty::ReadFromXML(TiXmlElement* Element)
{
	SetType(GetTypeFromText(Element->Attribute("Type")));
	SetName(Element->Value());
	
	switch (GetType())
	{
		case ZEML_IT_UNDEFINED:
			return false;
			break;
		case ZEML_IT_FLOAT:
			SetValue(ZEString(Element->Attribute("Value")).ToFloat());
			break;
		case ZEML_IT_DOUBLE:
			SetValue(ZEString(Element->Attribute("Value")).ToDouble());
			break;
		case ZEML_IT_INT8:
			SetValue(ZEString(Element->Attribute("Value")).ToInt8());
			break;
		case ZEML_IT_INT16:
			SetValue(ZEString(Element->Attribute("Value")).ToInt16());
			break;
		case ZEML_IT_INT32:
			SetValue(ZEString(Element->Attribute("Value")).ToInt32());
			break;
		case ZEML_IT_INT64:
			SetValue(ZEString(Element->Attribute("Value")).ToInt64());
			break;
		case ZEML_IT_UINT8:
			SetValue(ZEString(Element->Attribute("Value")).ToUInt8());
			break;
		case ZEML_IT_UINT16:
			SetValue(ZEString(Element->Attribute("Value")).ToUInt16());
			break;
		case ZEML_IT_UINT32:
			SetValue(ZEString(Element->Attribute("Value")).ToUInt32());
			break;
		case ZEML_IT_UINT64:
			SetValue(ZEString(Element->Attribute("Value")).ToUInt64());
			break;
		case ZEML_IT_BOOLEAN:
			{
				ZEString ValueText = Element->Attribute("Value");

				if (ValueText == "true")
					SetValue(true);
				else
					SetValue(false);
			}
			break;			
		case ZEML_IT_STRING:
			SetValue(ZEString(Element->Attribute("Value")));
			break;
		case ZEML_IT_QUATERNION:
			{
				ZEQuaternion Quaternion;
				Quaternion.x = ZEString(Element->Attribute("X")).ToFloat();
				Quaternion.y = ZEString(Element->Attribute("Y")).ToFloat();
				Quaternion.z = ZEString(Element->Attribute("Z")).ToFloat();
				Quaternion.w = ZEString(Element->Attribute("W")).ToFloat();
				SetValue(Quaternion);
			}
			break;
		case ZEML_IT_VECTOR2:
			{
				ZEVector2 Vector2;
				Vector2.x = ZEString(Element->Attribute("X")).ToFloat();
				Vector2.y = ZEString(Element->Attribute("Y")).ToFloat();
				SetValue(Vector2);
			}
			break;
		case ZEML_IT_VECTOR3:
			{
				ZEVector3 Vector3;
				Vector3.x = ZEString(Element->Attribute("X")).ToFloat();
				Vector3.y = ZEString(Element->Attribute("Y")).ToFloat();
				Vector3.z = ZEString(Element->Attribute("Z")).ToFloat();
				SetValue(Vector3);
			}
			break;
		case ZEML_IT_VECTOR4:
			{
				ZEVector4 Vector4;
				Vector4.x = ZEString(Element->Attribute("X")).ToFloat();
				Vector4.y = ZEString(Element->Attribute("Y")).ToFloat();
				Vector4.z = ZEString(Element->Attribute("Z")).ToFloat();
				Vector4.w = ZEString(Element->Attribute("W")).ToFloat();
				SetValue(Vector4);
			}
			break;
		case ZEML_IT_MATRIX3X3:
			{
				ZEMatrix3x3 Matrix3x3;	
				Matrix3x3.M11 = ZEString(Element->Attribute("M11")).ToFloat();
				Matrix3x3.M12 = ZEString(Element->Attribute("M12")).ToFloat();
				Matrix3x3.M13 = ZEString(Element->Attribute("M13")).ToFloat();
				Matrix3x3.M21 = ZEString(Element->Attribute("M21")).ToFloat();
				Matrix3x3.M22 = ZEString(Element->Attribute("M22")).ToFloat();
				Matrix3x3.M23 = ZEString(Element->Attribute("M23")).ToFloat();
				Matrix3x3.M31 = ZEString(Element->Attribute("M31")).ToFloat();
				Matrix3x3.M32 = ZEString(Element->Attribute("M32")).ToFloat();
				Matrix3x3.M33 = ZEString(Element->Attribute("M33")).ToFloat();
				SetValue(Matrix3x3);
			}
			break;
		case ZEML_IT_MATRIX4X4:
			{
				ZEMatrix4x4 Matrix4x4;	
				Matrix4x4.M11 = ZEString(Element->Attribute("M11")).ToFloat();
				Matrix4x4.M12 = ZEString(Element->Attribute("M12")).ToFloat();
				Matrix4x4.M13 = ZEString(Element->Attribute("M13")).ToFloat();
				Matrix4x4.M14 = ZEString(Element->Attribute("M14")).ToFloat();
				Matrix4x4.M21 = ZEString(Element->Attribute("M21")).ToFloat();
				Matrix4x4.M22 = ZEString(Element->Attribute("M22")).ToFloat();
				Matrix4x4.M23 = ZEString(Element->Attribute("M23")).ToFloat();
				Matrix4x4.M24 = ZEString(Element->Attribute("M24")).ToFloat();
				Matrix4x4.M31 = ZEString(Element->Attribute("M31")).ToFloat();
				Matrix4x4.M32 = ZEString(Element->Attribute("M32")).ToFloat();
				Matrix4x4.M33 = ZEString(Element->Attribute("M33")).ToFloat();
				Matrix4x4.M34 = ZEString(Element->Attribute("M34")).ToFloat();
				Matrix4x4.M41 = ZEString(Element->Attribute("M41")).ToFloat();
				Matrix4x4.M42 = ZEString(Element->Attribute("M42")).ToFloat();
				Matrix4x4.M43 = ZEString(Element->Attribute("M43")).ToFloat();
				Matrix4x4.M44 = ZEString(Element->Attribute("M44")).ToFloat();
				SetValue(Matrix4x4);
			}
			break;
		default:
			return false;
		break;
	}

	return true;
}

bool ZEMLProperty::WriteSelf(ZEFile* File)
{
	ZEInt64 TempInt64;
	ZEInt32	TempInt32;
	ZEInt16	TempInt16;
	ZEInt8	TempInt8;
	ZEUInt64 TempUInt64;
	ZEUInt32 TempUInt32;
	ZEUInt16 TempUInt16;
	ZEUInt8 TempUInt8;

	char Identifier = 'Z';
	if(File->Write(&Identifier, sizeof(char), 1) != 1)
	{
		zeError("Can not write ZEMLProperty identifier to file.");
		return false;
	}

	if(File->Write(&Type, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not write ZEMLProperty type to file.");
		return false;
	}

	ZEUInt8 NameLength = Name.GetSize();
	if(File->Write(&NameLength, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not write ZEMLProperty name lenght to file.");
		return false;
	}

	if(File->Write(Name.GetValue(), sizeof(char) * NameLength, 1) != 1)
	{
		zeError("Can not write ZEMLProperty name to file.");
		return false;
	}

	TempUInt64 = ZEEndian::Little(DataSize);
	if(File->Write(&TempUInt64, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Can not write ZEMLProperty data size to file.");
		return false;
	}

	ZEUInt64 IsDataWriten = 0;

	switch (this->Value.GetType())
	{
		case ZE_VRT_FLOAT:
			IsDataWriten = File->Write(&Value.ImplicitAcesss().Float, Value.SizeOf(), 1);
			break;
		case ZE_VRT_DOUBLE:
			IsDataWriten = File->Write(&Value.ImplicitAcesss().Double, Value.SizeOf(), 1);
			break;
		case ZE_VRT_INTEGER_8:
			TempInt8 = ZEEndian::Little(Value.GetInt8());
			IsDataWriten = File->Write(&TempInt8, sizeof(ZEInt8), 1);
			break;
		case ZE_VRT_INTEGER_16:
			TempInt16 = ZEEndian::Little(Value.GetInt16());
			IsDataWriten = File->Write(&TempInt16, sizeof(ZEInt16), 1);
			break;
		case ZE_VRT_INTEGER_32:
			TempInt32 = ZEEndian::Little(Value.GetInt32());
			IsDataWriten = File->Write(&TempInt32, sizeof(ZEInt32), 1);
			break;
		case ZE_VRT_INTEGER_64:
			TempInt64 = ZEEndian::Little(Value.GetInt64());
			IsDataWriten = File->Write(&TempInt64, sizeof(ZEInt64), 1);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_8:
			TempUInt8 = ZEEndian::Little(Value.GetUInt8());
			IsDataWriten = File->Write(&TempUInt8, sizeof(ZEUInt8), 1);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_16:
			TempUInt16 = ZEEndian::Little(Value.GetUInt16());
			IsDataWriten = File->Write(&TempUInt16, sizeof(ZEUInt16), 1);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_32:
			TempUInt32 = ZEEndian::Little(Value.GetUInt32());
			IsDataWriten = File->Write(&TempUInt32, sizeof(ZEUInt32), 1);
			break;
		case ZE_VRT_UNSIGNED_INTEGER_64:
			TempUInt64 = ZEEndian::Little(Value.GetUInt64());
			IsDataWriten = File->Write(&TempUInt64, sizeof(ZEUInt64), 1);
			break;
		case ZE_VRT_BOOLEAN:
			IsDataWriten = File->Write(&Value.ImplicitAcesss().Boolean, Value.SizeOf(), 1);
			break;
		case ZE_VRT_STRING:
			{
				if (Value.ImplicitAcesss().String.GetSize() == 0)
					IsDataWriten = 1;
				else
					IsDataWriten = File->Write(Value.ImplicitAcesss().String.GetValue(), Value.ImplicitAcesss().String.GetSize(), 1);
				break;
			}
		case ZE_VRT_QUATERNION:
			IsDataWriten = File->Write(&Value.ImplicitAcesss().Vectors, sizeof(ZEQuaternion), 1);
			break;
		case ZE_VRT_VECTOR2:
			IsDataWriten = File->Write(&Value.ImplicitAcesss().Vectors, sizeof(ZEVector2), 1);
			break;
		case ZE_VRT_VECTOR3:
			IsDataWriten = File->Write(&Value.ImplicitAcesss().Vectors, sizeof(ZEVector3), 1);
			break;
		case ZE_VRT_VECTOR4:
			IsDataWriten = File->Write(&Value.ImplicitAcesss().Vectors, sizeof(ZEVector4), 1);
			break;
		case ZE_VRT_MATRIX3X3:
			IsDataWriten = File->Write(Value.ImplicitAcesss().Matrix3x3, sizeof(ZEMatrix3x3), 1);
			break;
		case ZE_VRT_MATRIX4X4:
			IsDataWriten = File->Write(Value.ImplicitAcesss().Matrix4x4, sizeof(ZEMatrix4x4), 1);
			break;
		default:
			zeError("Unsupported ZEMLProperty type.");
			return false;
			break;
	}

	if(IsDataWriten != 1)
	{
		zeError("Can not write ZEMLProperty value to file.");
		return false;
	}

	return true;
}

bool ZEMLProperty::ReadSelf(ZEFile* File, bool DeferredDataReading)
{
	FilePosition = File->Tell();

	char		Identifier;	
	ZEUInt8		NameSize;
	char		TempNameBuffer[ZEML_MAX_NAME_SIZE];	

	if(File->Read(&Identifier, sizeof(char), 1) != 1)
	{
		zeError("Can not read ZEMLProperty identifier from file. Corrupted ZEML file.");
		return false;
	}

	if(Identifier != ZEML_ITEM_FILE_IDENTIFIER)
	{
		zeError("ZEMLProperty identifier mismatch. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(&Type, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not read ZEMLProperty type from file. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(&NameSize, sizeof(ZEUInt8), 1) != 1)
	{
		zeError("Can not read ZEMLProperty name length from file. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(TempNameBuffer, NameSize, 1) != 1)
	{
		zeError("Can not read ZEMLProperty name from file. Corrupted ZEML file.");
		return false;
	}

	if(File->Read(&DataSize, sizeof(ZEUInt64), 1) != 1)
	{
		zeError("Can not read ZEMLProperty value size from file. Corrupted ZEML file.");
		return false;
	}

	SetName(TempNameBuffer);

	ZEUInt64 IsDataRead = 0;

	switch(Type)
	{
		case ZEML_IT_FLOAT:
			Value.SetType(ZE_VRT_FLOAT);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Float, DataSize, 1);
			break;
		case ZEML_IT_DOUBLE:
			Value.SetType(ZE_VRT_DOUBLE);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Double, DataSize, 1);
			break;
		case ZEML_IT_INT8:
			Value.SetType(ZE_VRT_INTEGER_8);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int32, DataSize, 1);
			break;
		case ZEML_IT_INT16:
			Value.SetType(ZE_VRT_INTEGER_16);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int32, DataSize, 1);
			Value.SetInt16(ZEEndian::Little(Value.GetInt16()));
			break;
		case ZEML_IT_INT32:
			Value.SetType(ZE_VRT_INTEGER_32);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int32, DataSize, 1);
			Value.SetInt32(ZEEndian::Little(Value.GetInt32()));
			break;
		case ZEML_IT_INT64:
			Value.SetType(ZE_VRT_INTEGER_64);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int64, DataSize, 1);
			Value.SetInt64(ZEEndian::Little(Value.GetInt64()));
			break;
		case ZEML_IT_UINT8:
			Value.SetType(ZE_VRT_UNSIGNED_INTEGER_8);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int32, DataSize, 1);
			break;
		case ZEML_IT_UINT16:
			Value.SetType(ZE_VRT_UNSIGNED_INTEGER_16);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int32, DataSize, 1);
			Value.SetUInt16(ZEEndian::Little(Value.GetUInt16()));
			break;
		case ZEML_IT_UINT32:
			Value.SetType(ZE_VRT_UNSIGNED_INTEGER_32);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int32, DataSize, 1);
			Value.SetUInt32(ZEEndian::Little(Value.GetUInt32()));
			break;
		case ZEML_IT_UINT64:
			Value.SetType(ZE_VRT_UNSIGNED_INTEGER_64);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Int64, DataSize, 1);
			Value.SetUInt64(ZEEndian::Little(Value.GetUInt64()));
			break;
		case ZEML_IT_BOOLEAN:
			Value.SetType(ZE_VRT_BOOLEAN);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Boolean, DataSize, 1);
			break;
		case ZEML_IT_STRING:
			{
				Value.SetType(ZE_VRT_STRING);

				if (DataSize == 0)
				{
					IsDataRead = 1;
					Value.SetString(ZEString());
				}
				else
				{
					char* TempBuffer = new char[DataSize];
					IsDataRead = File->Read(TempBuffer, DataSize, 1);
					Value.SetString(TempBuffer);
					delete TempBuffer;
				}
			}
			break;
		case ZEML_IT_QUATERNION:
			Value.SetType(ZE_VRT_QUATERNION);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Vectors, DataSize, 1);
			break;
		case ZEML_IT_VECTOR2:
			Value.SetType(ZE_VRT_VECTOR2);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Vectors, DataSize, 1);
			break;
		case ZEML_IT_VECTOR3:
			Value.SetType(ZE_VRT_VECTOR3);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Vectors, DataSize, 1);
			break;
		case ZEML_IT_VECTOR4:
			Value.SetType(ZE_VRT_VECTOR4);
			IsDataRead = File->Read(&Value.ImplicitAcesss().Vectors, DataSize, 1);
			break;
		case ZEML_IT_MATRIX3X3:
			Value.SetType(ZE_VRT_MATRIX3X3);
			IsDataRead = File->Read(Value.ImplicitAcesss().Matrix3x3, DataSize, 1);
			break;
		case ZEML_IT_MATRIX4X4:
			Value.SetType(ZE_VRT_MATRIX4X4);
			IsDataRead = File->Read(Value.ImplicitAcesss().Matrix4x4, DataSize, 1);
			break;
		default:
			zeError("Unsupported ZEMLProperty type.");
			return false;
			break;
	}

	if(IsDataRead != 1)
	{
		zeError("Can not read ZEMLProperty value from file. Corrupted ZEML file.");
		return false;
	}

	return true;
}
