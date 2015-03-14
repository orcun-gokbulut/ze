//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESceneConverter.cpp
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

#include "ZESceneConverter.h"

#include "ZEML/ZEMLSerialWriter.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEFile/ZEFile.h"

#define ZE_MAX_NAME_SIZE			128
#define ZE_MAX_FILE_NAME_SIZE		256
#define ZE_CLSF_CLASS_CHUNKID ((ZEUInt32)'CLAS')

static bool ConvertProperty(const char* Name, ZEMLSerialNode* Serializer, ZEFile* Unserializer)
{
	ZEUInt32 Type;
	ZEUInt32 StringSize;
	Unserializer->Read(&Type, sizeof(ZEUInt32), 1);

	struct ZEVariantValue
	{
		union
		{
			float			Float;
			double			Double;

			ZEInt32			Int32;
			ZEInt64			Int64;

			bool			Boolean;
			ZEObject*		Pointer;

			struct
			{
				float		A, B, C, D;
			} Vectors;

			ZEMatrix3x3*	Matrix3x3;
			ZEMatrix4x4*	Matrix4x4;
		};
		ZEString			String;
	} Value;

	switch(Type)
	{
		default:
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
			break;

		case ZE_VRT_FLOAT:
			Unserializer->Read(&Value.Float, sizeof(float), 1);
			Serializer->WriteProperty(Name, Value.Float);
			break;

		case ZE_VRT_DOUBLE:
			Unserializer->Read(&Value.Double, sizeof(double), 1);
			Serializer->WriteProperty(Name, Value.Double);
			break;

		case ZE_VRT_INTEGER_8:
			Unserializer->Read(&Value.Int32, sizeof(ZEInt8), 1);
			Serializer->WriteProperty(Name, (ZEInt8)Value.Int32);
			break;

		case ZE_VRT_INTEGER_16:
			Unserializer->Read(&Value.Int32, sizeof(ZEInt16), 1);
			Serializer->WriteProperty(Name, (ZEInt16)Value.Int32);
			break;

		case ZE_VRT_INTEGER_32:
			Unserializer->Read(&Value.Int32, sizeof(ZEInt32), 1);
			Serializer->WriteProperty(Name, (ZEInt32)Value.Int32);
			break;

		case ZE_VRT_INTEGER_64:
			Unserializer->Read(&Value.Int64, sizeof(ZEInt64), 1);
			Serializer->WriteProperty(Name, (ZEInt64)Value.Int64);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			Unserializer->Read(&Value.Int32, sizeof(ZEUInt8), 1);
			Serializer->WriteProperty(Name, (ZEUInt8)Value.Int32);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			Unserializer->Read(&Value.Int32, sizeof(ZEUInt16), 1);
			Serializer->WriteProperty(Name, (ZEUInt16)Value.Int32);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			Unserializer->Read(&Value.Int32, sizeof(ZEUInt32), 1);
			Serializer->WriteProperty(Name, (ZEUInt32)Value.Int32);
			break;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			Unserializer->Read(&Value.Int64, sizeof(ZEUInt64), 1);
			Serializer->WriteProperty(Name, (ZEUInt64)Value.Int64);
			break;

		case ZE_VRT_BOOLEAN:
			Unserializer->Read(&Value.Boolean, sizeof(bool), 1);
			Serializer->WriteProperty(Name, Value.Boolean);
			break;

		case ZE_VRT_STRING:
		{
			Unserializer->Read(&StringSize, sizeof(ZEUInt32), 1);
			char* Buffer = new char[StringSize];
			Unserializer->Read(Buffer, sizeof(char), StringSize);
			Buffer[StringSize - 1] = '\0';
			Serializer->WriteProperty(Name, Buffer);
			break;
		}

		case ZE_VRT_QUATERNION:
			Unserializer->Read(&Value.Vectors, sizeof(ZEQuaternion), 1);
			Serializer->WriteProperty(Name, *(ZEQuaternion*)&Value.Vectors);
			break;

		case ZE_VRT_VECTOR2:
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector2), 1);
			Serializer->WriteProperty(Name, *(ZEVector2*)&Value.Vectors);
			break;

		case ZE_VRT_VECTOR3:
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector3), 1);
			Serializer->WriteProperty(Name, *(ZEVector3*)&Value.Double);
			break;

		case ZE_VRT_VECTOR4:
			Unserializer->Read(&Value.Vectors, sizeof(ZEVector4), 1);
			Serializer->WriteProperty(Name, *(ZEVector4*)&Value.Vectors);
			break;

		case ZE_VRT_MATRIX3X3:
			Unserializer->Read(Value.Matrix3x3, sizeof(ZEMatrix3x3), 1);
			Serializer->WriteProperty(Name, *(ZEMatrix3x3*)&Value.Matrix3x3);
			break;

		case ZE_VRT_MATRIX4X4:
			Unserializer->Read(Value.Matrix4x4, sizeof(ZEMatrix4x4), 1);
			Serializer->WriteProperty(Name, *(ZEMatrix4x4*)&Value.Matrix4x4);
			break;
	}
	return true;
}

static void ConvertEntity(ZEFile* Unserializer, ZEMLSerialNode* Serializer)
{
	char EntityTypeName[ZE_MAX_NAME_SIZE];
	Unserializer->Read(EntityTypeName, sizeof(char), ZE_MAX_NAME_SIZE);
	Serializer->WriteProperty("Class", EntityTypeName);

	ZEMLSerialNode Properties = Serializer->OpenNode("Properties");

	struct ZEObjectFileChunk
	{
		ZEUInt32 Header;
		char ClassType[ZE_MAX_NAME_SIZE];
		ZEUInt32 PropertyCount;
	};

	ZEObjectFileChunk ClassChunk;
	Unserializer->Read(&ClassChunk, sizeof(ZEObjectFileChunk), 1);

	if (ClassChunk.Header != ZE_CLSF_CLASS_CHUNKID)
	{
		printf("Corrupted zeScene file.\n");
		exit(EXIT_FAILURE);
	}

	for (ZESize I = 0; I < ClassChunk.PropertyCount; I++)
	{
		char PropertyName[ZE_MAX_NAME_SIZE];
		Unserializer->Read(PropertyName, sizeof(char), ZE_MAX_NAME_SIZE);
		ConvertProperty(PropertyName, Serializer, Unserializer);
	}
}

static void ConvertScene(ZEFile* Unserializer, ZEMLSerialNode* Serializer)
{
	ZEUInt32 EntityCount;
	ZEUInt LastEntityId;
	Unserializer->Read(&EntityCount, sizeof(ZEUInt32), 1);
	Unserializer->Read(&LastEntityId, sizeof(ZEInt), 1);

	char MapFile[ZE_MAX_FILE_NAME_SIZE];
	Unserializer->Read(MapFile, sizeof(char), ZE_MAX_FILE_NAME_SIZE);

	ZEMLSerialNode EntitiesNode = Serializer->OpenNode("Entities");
	for (ZESize I = 0; I < EntityCount; I++)
	{
		ZEMLSerialNode EntityNode = Serializer->OpenNode("Entity");
		ConvertEntity(Unserializer, &EntityNode);
		EntityNode.CloseNode();
	}

	EntitiesNode.CloseNode();
	Serializer->CloseNode();
}

bool ZESceneConverter::Convert(const char* Source, const char* Destination)
{
	ZEFile Unserializer;
	if (!Unserializer.Open(Source, ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot open source ZEScene file. File Name: \"%s\"", Source);
		return false;
	}

	printf(" Checking source version.\n");
	char Identifier[4];
	if (Unserializer.Read(Identifier, 4, 1) != 1)
	{
		if (Identifier[0] == 'Z' ||
			Identifier[0] == 'E' ||
			Identifier[0] == 'M' ||
			Identifier[0] == 'L')
		{
			zeError("Scene file is already converted.\n");
			return true;
		}
	}

	Unserializer.Seek(0, ZE_SF_BEGINING);

	ZEFile DestinationFile;
	if (!DestinationFile.Open(Destination, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cannot open destination ZEScene file. File Name: \"%s\".", Destination);
		return false;
	}

	ZEMLSerialRootNode Serializer("Scene", &DestinationFile);
	ConvertScene(&Unserializer, &Serializer);

	DestinationFile.Close();
	Unserializer.Close();

	return true;
}
