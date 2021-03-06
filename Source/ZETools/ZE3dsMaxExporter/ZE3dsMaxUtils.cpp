//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxUtils.cpp
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

#include "ZE3dsMaxUtils.h"


ZEVector2 ZE3dsMaxUtils::MaxtoZE(const Point2& Point)
{
	return ZEVector2(Point.x, Point.y);
}

ZEVector3 ZE3dsMaxUtils::MaxtoZE(const Point3& Point)
{
	return ZEVector3(Point.x, Point.y, Point.z);
}

ZEQuaternion ZE3dsMaxUtils::MaxtoZE(const Quat& Quaternion)
{
	Quat Temp = Quaternion.Inverse();
	return ZEQuaternion(Temp.w, Temp.x, Temp.y, Temp.z);
}

Point2 ZE3dsMaxUtils::ZEtoMax(const ZEVector2& Vector)
{
	return Point2(Vector.x, Vector.y);
}

Point3 ZE3dsMaxUtils::ZEtoMax(const ZEVector3& Vector)
{
	return Point3(Vector.x, Vector.y, Vector.z);
}

Quat ZE3dsMaxUtils::ZEtoMax(const ZEQuaternion& Quaternion)
{
	return Quat(Quaternion.x, Quaternion.y, Quaternion.z, Quaternion.w);
}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, const ZEString& Property, INode*& Value)
{
	if (Object == NULL)
		return false;
	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		IParamBlock2* ParamBlock =  Prop->GetMaxParamBlock2();
		ZEInt ParamId = ParamBlock->IndextoID(Prop->GetParamIndex());
		if (ParamBlock->GetParameterType(ParamId) != TYPE_INODE)
			return false;

		Value = ParamBlock->GetINode(ParamId);
		return true;
	}
	else
	{
		Value = NULL;
		zeWarning("Can not find property. (Property Name : \"%s\")", Property.ToCString());
		return false;
	}
}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, const ZEString& Property, IGameScene* Scene, IGameNode*& Value)
{
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);

	if (Prop != NULL)
	{	
		IParamBlock2* ParamBlock = Prop->GetMaxParamBlock2();
		ZEInt ParamId = ParamBlock->IndextoID(Prop->GetParamIndex());

		INode* Node =  ParamBlock->GetINode(ParamId);
		if (Node == NULL)
		{
			Value = NULL;
			return true;
		}

		Value = Scene->GetIGameNode(Node);
		return true;
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property.ToCString());
		return false;
	}
}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, ZEPropType Type, const ZEString& Property, bool& Value)
{
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		if (Type == ZE_BOOL_PROP)
			Type = ZE_INT_PROP;

		if (Prop->GetType() == Type)
		{
			ZEInt Temp;
			if (Prop->GetPropertyValue(Temp))
			{
				Value = (Temp != 0);
				return true;
			}
			else
			{
				zeWarning("Can not read property value. (Property Name : \"%s\")", Property.ToCString());
				return false;
			}
		}
		else
		{
			zeWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property.ToCString(), Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property.ToCString());
		return false;
	}
}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, ZEPropType Type, const ZEString& Property, ZEString& Value)
{
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);

	if (Prop != NULL)
	{
		if (Type == ZE_BOOL_PROP)
			Type = ZE_INT_PROP;

		if (Prop->GetType() == Type)
		{
			const MCHAR* TempValue;

			if (Prop->GetPropertyValue(TempValue))
			{
				Value.SetValue(TempValue);
				return true;
			}
			else
			{
				zeWarning("Can not read property value. (Property Name : \"%s\")", Property.ToCString());
				return false;
			}
		}	
		else
		{
			zeWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property.ToCString(), Prop->GetType(), Type);
			return false;
		}
	}

	return false;
}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, ZEPropType Type, const ZEString& Property, ZEInt& Value)
{ 
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		if (Type == ZE_BOOL_PROP)
			Type = ZE_INT_PROP;

		if (Prop->GetType() == Type)
		{
			if (Prop->GetPropertyValue(Value))
				return true;
			else
			{
				zeWarning("Can not read property value. (Property Name : \"%s\")", Property.ToCString());
				return false;
			}
		}
		else
		{
			zeWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property.ToCString(), Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property.ToCString());
		return false;
	}
}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, ZEPropType Type, const ZEString& Property, ZEUInt& Value)
{
	ZEInt Temp;
	if (Object == NULL)
		return false;

	if (Type == ZE_BOOL_PROP)
		Type = ZE_INT_PROP;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);

	if (Prop != NULL)
	{
		if (Type == ZE_BOOL_PROP)
			Type = ZE_INT_PROP;

		if (Prop->GetType() == Type)
		{
			if (Prop->GetPropertyValue(Temp))
			{
				Value = Temp;
				return true;
			}
			else
			{
				zeWarning("Can not read property value. (Property Name : \"%s\")", Property.ToCString());
				return false;
			}
		}
		else
		{
			zeWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property.ToCString(), Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property.ToCString());
		return false;
	}

}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, ZEPropType Type, const ZEString& Property, float& Value)
{
	if (Object == NULL)
		return false;

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL)
	{
		if (Type == ZE_BOOL_PROP)
			Type = ZE_INT_PROP;

		if (Prop->GetType() == Type)
		{
			if (Prop->GetPropertyValue(Value))
				return true;
			else
			{
				zeWarning("Can not read property value. (Property Name : \"%s\")", Property.ToCString());
				return false;
			}
		}
		else
		{
			zeWarning("Wrong property type. (Property Name : \"%s\", Expected Type : %d, Type : %d)", Property.ToCString(), Prop->GetType(), Type);
			return false;
		}
	}
	else
	{
		zeWarning("Can not find property. (Property Name : \"%s\")", Property.ToCString());
		return false;
	}
}

bool ZE3dsMaxUtils::GetProperty(IExportEntity* Object, ZEPropType Type, const ZEString& Property, ZEMLProperty& Value)
{
	if (Object == NULL)
		return false;

	bool IsPropertyBoolean = false;

	if (Type == ZE_BOOL_PROP)
	{
		Type = ZE_INT_PROP;
		IsPropertyBoolean = true;
	};

	IGameProperty* Prop = Object->GetIPropertyContainer()->QueryProperty(Property);
	if (Prop != NULL && Prop->GetType() == Type)
	{
		bool Result = false;

		switch (Type)
		{
			case ZE_FLOAT_PROP:
				{
					float FloatValue;
					Result = Prop->GetPropertyValue(FloatValue);

					if (Result)
						Value.SetFloat(FloatValue);

					break;
				}
			case ZE_INT_PROP:
				{
					ZEInt IntegerValue;
					Result = Prop->GetPropertyValue(IntegerValue);

					if (Result)
					{
						if (IsPropertyBoolean)
							Value.SetBool((bool)(IntegerValue != 0));
						else
							Value.SetInt32(IntegerValue);
					}

					break;
				}
			case ZE_VECTOR3_PROP:
				{
					ZEVector3 Vector3Value;
					Result = Prop->GetPropertyValue((Point3&)Vector3Value);

					if (Result)
						Value.SetVector3(Vector3Value);

					break;
				}
			case ZE_VECTOR4_PROP:
				{
					ZEVector4 Vector4Value;
					Result = Prop->GetPropertyValue((Point4&)Vector4Value);

					if (Result)
						Value.SetVector4(Vector4Value);

					break;
				}
			case ZE_STRING_PROP:
				{
					const MCHAR* StringValue;
					Result = Prop->GetPropertyValue(StringValue);

					if (Result)
						Value.SetString(ZEString(StringValue));

					break;
				}
		}

		return Result;
	}
	else
		return false;
}

void ZE3dsMaxUtils::CalculateLocalBoundingBox(ZEAABBox& BoundingBox, const ZEArray<ZEModelFileVertex>& Vertices)
{
	if (Vertices.GetCount() == 0)
	{
		BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
		return;
	}

	BoundingBox.Min = BoundingBox.Max = Vertices[0].Position;

	for (ZESize I = 0; I < Vertices.GetCount(); I++)
	{
		ZEVector3 Point = Vertices[I].Position;
		if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
		if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
		if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;
		if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
		if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
		if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
	}

// 	if (Mesh->GetNumberOfFaces() == 0)
// 	{
// 		BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
// 		return;
// 	}
// 	else
// 		BoundingBox.Min = BoundingBox.Max = *(ZEVector3*)&Mesh->GetVertex(Mesh->GetFace(0)->vert[0], true);
// 
// 	for (ZEInt I = 0; I < Mesh->GetNumberOfFaces(); I++)
// 	{
// 		FaceEx* Face = Mesh->GetFace(I);
// 		for (ZESize N = 0; N < 3; N++)
// 		{
// 			Point3 Point;
// 			Mesh->GetVertex(Face->vert[N], Point, true);
// 			if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
// 			if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
// 			if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;
// 			if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
// 			if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
// 			if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
// 		}
// 	}
}

void ZE3dsMaxUtils::CalculateLocalBoundingBox(ZEAABBox& BoundingBox, const ZEArray<ZEModelFileSkinnedVertex>& Vertices)
{
	if (Vertices.GetCount() == 0)
	{
		BoundingBox.Min = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
		return;
	}

	BoundingBox.Min = BoundingBox.Max = Vertices[0].Position;

	for (ZESize I = 0; I < Vertices.GetCount(); I++)
	{
		ZEVector3 Point = Vertices[I].Position;
		if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
		if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
		if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;
		if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
		if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
		if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
	}
}
