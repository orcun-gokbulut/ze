//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGenerator.cpp
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

#include "ZEMCGenerator.h"
#include "ZEMCContext.h"
#include "ZEDS\ZEFormat.h"
#include <stdarg.h>
#include "ZETimeStamp.h"
#include "ZEVersion.h"
#include "ZEFile\ZEPathInfo.h"


bool ZEMCGenerator::OpenFile()
{
	OutputFile = fopen(Options->OutputFileName.ToCString(), "w");
	if (OutputFile == NULL)
		return false;

	return true;
}

void ZEMCGenerator::WriteToFile(const char* Format, ...)
{
	va_list Args;
	va_start(Args, Format);

	vfprintf(OutputFile, Format, Args);

	va_end(Args);
}

void ZEMCGenerator::CloseFile()
{
	if (OutputFile != NULL)
	{
		fclose(OutputFile);
		OutputFile = NULL;
	}
}

void ZEMCGenerator::SetMetaContext(ZEMCContext* Context)
{
	this->Context = Context;
}

void ZEMCGenerator::SetOptions(ZEMCOptions* Options)
{
	this->Options = Options;
}


bool ZEMCGenerator::Generate()
{
	if (!OpenFile())
		return false;

	GenerateHeading();

	for (int I = 0; I < Context->TargetEnumerators.GetCount(); I++)
	{
		GenerateEnumerator(Context->TargetEnumerators[I]);
	}

	for (int I = 0; I < Context->TargetClasses.GetCount(); I++)
	{
		GenerateClass((ZEMCClass*)Context->TargetClasses[I]);
	}

	GenerateEnding();

	CloseFile();

	return true;
}

void ZEMCGenerator::GenerateIncludes()
{
	if (Context->Includes.GetCount() != 0)
	{
		WriteToFile(
			"\n// Include(s)\n"
			"////////////////////////////////////////////////////////////////////////////////////////\n\n");
	}

	for (ZESize I = 0; I < Context->Includes.GetCount(); I++)
		WriteToFile("#include \"%s\"\n", Context->Includes[I]->HeaderFileName.ToCString());
	
	if (Context->Includes.GetCount() != 0)
		WriteToFile("\n");
}

void ZEMCGenerator::GenerateForwardDeclaration()
{
	if (Context->ForwardDeclarations.GetCount() != 0)
	{
		WriteToFile(
			"\n// Forward Declaration(s)\n"
			"////////////////////////////////////////////////////////////////////////////////////////\n\n");
	}

	for(ZESize I = 0; I < Context->ForwardDeclarations.GetCount(); I++)
	{
		switch (Context->ForwardDeclarations[I]->Type)
		{
			case ZEMC_DT_CLASS:
				WriteToFile("ZEClass* %s_Class();\n", Context->ForwardDeclarations[I]->Name.ToCString());
				break;

			case ZEMC_DT_ENUMERATOR:
				WriteToFile("ZEClass* %s_Enumerator();\n", Context->ForwardDeclarations[I]->Name.ToCString());
				break;

			case ZEMC_DT_FLAGS:
			default:
				continue;
		}
	}
	
	if (Context->Includes.GetCount() != 0)
		WriteToFile("\n");
}

void ZEMCGenerator::GenerateHeading()
{
	ZEString VersionString = ZEVersion::GetZinekVersion().GetLongString();
	ZEString FileName = ZEPathInfo(Options->InputFileName).GetFileName();

	ZETimeStamp TimeStamp = ZETimeStamp::Now();
	ZEString TimeStampString = ZEFormat::Format("{0:d:04}-{1:d:02}-{2:d:02} {3:d:02}:{4:d:02}:{5:d:02}",
		TimeStamp.GetYear(), TimeStamp.GetMonth(), TimeStamp.GetDay(),
		TimeStamp.GetHour(), TimeStamp.GetMinute(), TimeStamp.GetSecond());

	WriteToFile("//  ZEMetaCompiler auto generated ZEMeta file.\n");
	WriteToFile("//  DO NOT EDIT (HACK) THIS FILE !!! \n");
	WriteToFile("// -----------------------------------------------------------------------------------\n");
	WriteToFile("//  Input File : %s\n", FileName.ToCString());
	WriteToFile("//  Version    : %s\n", VersionString.ToCString());
	WriteToFile("//  Time Stamp : %s\n", TimeStampString.ToCString());
	WriteToFile("////////////////////////////////////////////////////////////////////////////////////////\n");
	WriteToFile("\n");

	WriteToFile("#include \"%s\"\n", Options->InputFileName.ToCString());
	WriteToFile("#include \"ZEDS/ZEVariant.h\"\n");
	WriteToFile("#include \"ZEDS/ZEReference.h\"\n");
	WriteToFile("#include \"ZEMeta/ZEClass.h\"\n");
	WriteToFile("#include \"ZEMeta/ZEMTFundamental.h\"\n");
	WriteToFile("\n");

	WriteToFile("#include <stddef.h>\n");
	WriteToFile("\n");

	GenerateIncludes();
	GenerateForwardDeclaration();
}

void ZEMCGenerator::GenerateEnding()
{

}

const char* ZEMCGenerator::ConvertBaseTypeToEnum(ZEMCBaseType BaseType)
{
	switch (BaseType)
	{
		default:
		case ZEMC_BT_UNDEFINED:
			return "ZEMT_BT_UNDEFINED";

		case ZEMC_BT_VOID:
			return "ZEMT_BT_VOID";

		case ZEMC_BT_INTEGER_8:
			return "ZEMT_BT_INTEGER_8";

		case ZEMC_BT_INTEGER_16:
			return "ZEMT_BT_INTEGER_16";

		case ZEMC_BT_INTEGER_32:
			return "ZEMT_BT_INTEGER_32";

		case ZEMC_BT_UNSIGNED_INTEGER_8:
			return "ZEMT_BT_UNSIGNED_INTEGER_8";

		case ZEMC_BT_UNSIGNED_INTEGER_16:
			return "ZEMT_BT_UNSIGNED_INTEGER_16";

		case ZEMC_BT_UNSIGNED_INTEGER_32:
			return "ZEMT_BT_UNSIGNED_INTEGER_32";

		case ZEMC_BT_INTEGER_64:
			return "ZEMT_BT_INTEGER_64";

		case ZEMC_BT_UNSIGNED_INTEGER_64:
			return "ZEMT_BT_UNSIGNED_INTEGER_64";

		case ZEMC_BT_FLOAT:
			return "ZEMT_BT_FLOAT";

		case ZEMC_BT_DOUBLE:
			return "ZEMT_BT_DOUBLE";

		case ZEMC_BT_BOOLEAN:
			return "ZEMT_BT_BOOLEAN";

		case ZEMC_BT_STRING:
			return "ZEMT_BT_STRING";

		case ZEMC_BT_QUATERNION:
			return "ZEMT_BT_QUATERNION";

		case ZEMC_BT_VECTOR2:
			return "ZEMT_BT_VECTOR2";

		case ZEMC_BT_VECTOR2D:
			return "ZEMT_BT_VECTOR2D";

		case ZEMC_BT_VECTOR3:
			return "ZEMT_BT_VECTOR3";

		case ZEMC_BT_VECTOR3D:
			return "ZEMT_BT_VECTOR3D";

		case ZEMC_BT_VECTOR4:
			return "ZEMT_BT_VECTOR4";

		case ZEMC_BT_VECTOR4D:
			return "ZEMT_BT_VECTOR4D";

		case ZEMC_BT_MATRIX3X3:
			return "ZEMT_BT_MATRIX3X3";

		case ZEMC_BT_MATRIX3X3D:
			return "ZEMT_BT_MATRIX3X3D";

		case ZEMC_BT_MATRIX4X4:
			return "ZEMT_BT_MATRIX4X4";

		case ZEMC_BT_MATRIX4X4D:
			return "ZEMT_BT_MATRIX4X4D";

		case ZEMC_BT_CLASS:
			return "ZEMT_BT_CLASS";

		case ZEMC_BT_OBJECT:
			return "ZEMT_BT_OBJECT";

		case ZEMC_BT_OBJECT_PTR:
			return "ZEMT_BT_OBJECT_PTR";

		case ZEMC_BT_ENUMERATOR:
			return "ZEMT_BT_ENUMERATOR";
	}
}

const char* ZEMCGenerator::ConvertContainerTypeToEnum(ZEMCContainerType ContainerType)
{
	switch (ContainerType)
	{
		default:
		case ZEMC_CT_NONE:
			return "ZEMT_CT_NONE";
		case ZEMC_CT_ARRAY:
			return "ZEMT_CT_ARRAY";
		case ZEMC_CT_LIST:
			return "ZEMT_CT_LIST";
		case ZEMC_CT_CONTAINER:
			return "ZEMT_CT_CONTAINER";
	}
}

const char* ZEMCGenerator::ConvertTypeQualifierToEnum(ZEMCTypeQualifier TypeQualifier)
{
	switch (TypeQualifier)
	{
		default:
		case ZEMC_TQ_VALUE:
			return "ZEMT_TQ_VALUE";

		case ZEMC_TQ_CONST_VALUE:
			return "ZEMT_TQ_CONST_VALUE";

		case ZEMC_TQ_REFERENCE:
			return "ZEMT_TQ_REFERENCE";

		case ZEMC_TQ_CONST_REFERENCE:
			return "ZEMT_TQ_CONST_REFERENCE";
	}
}

ZEString ZEMCGenerator::ConvertBaseTypeToName(const ZEMCType& Type)
{
	switch (Type.BaseType)
	{
		default:
		case ZEMC_BT_UNDEFINED:
			return "";

		case ZEMC_BT_VOID:
			return "void";

		case ZEMC_BT_INTEGER_8:
			return "signed char";

		case ZEMC_BT_INTEGER_16:
			return "signed short";

		case ZEMC_BT_INTEGER_32:
			return "signed int";

		case ZEMC_BT_UNSIGNED_INTEGER_8:
			return "unsigned char";

		case ZEMC_BT_UNSIGNED_INTEGER_16:
			return "unsigned short";

		case ZEMC_BT_UNSIGNED_INTEGER_32:
			return "unsigned int";

		case ZEMC_BT_INTEGER_64:
			return "signed long long";

		case ZEMC_BT_UNSIGNED_INTEGER_64:
			return "unsigned long long";

		case ZEMC_BT_FLOAT:
			return "float";

		case ZEMC_BT_DOUBLE:
			return "double";

		case ZEMC_BT_BOOLEAN:
			return "bool";

		case ZEMC_BT_ENUMERATOR:
			return Type.Enumurator->Name.ToCString();

		case ZEMC_BT_STRING:
			return "ZEString";

		case ZEMC_BT_QUATERNION:
			return "ZEQuaternion";

		case ZEMC_BT_VECTOR2:
			return "ZEVector2";

		case ZEMC_BT_VECTOR2D:
			return "ZEVector2d";

		case ZEMC_BT_VECTOR3:
			return "ZEVector3";

		case ZEMC_BT_VECTOR3D:
			return "ZEVector3d";

		case ZEMC_BT_VECTOR4:
			return "ZEVector4";

		case ZEMC_BT_VECTOR4D:
			return "ZEVector4d";

		case ZEMC_BT_MATRIX3X3:
			return "ZEMatrix3x3";

		case ZEMC_BT_MATRIX3X3D:
			return "ZEMatrix3x3d";

		case ZEMC_BT_MATRIX4X4:
			return "ZEMatrix4x4";

		case ZEMC_BT_MATRIX4X4D:
			return "ZEMatrix4x4d";

		case ZEMC_BT_CLASS:
			return "ZEClass*";

		case ZEMC_BT_OBJECT:
			return Type.Class->Name.ToCString();

		case ZEMC_BT_OBJECT_PTR:
			return ZEFormat::Format("{0}*", Type.Class->Name);
	}
}
ZEString ZEMCGenerator::GenerateVariantPostfix(const ZEMCType& Type, ZEString& CastOutput, ZEString& SetterExtraParams)
{
	ZEString Output;

	if (Type.ContainerType == ZEMC_CT_ARRAY || Type.ContainerType == ZEMC_CT_LIST)
	{
		if (Type.ContainerType == ZEMC_CT_ARRAY)
			Output.Append("Array");
		else if (Type.ContainerType == ZEMC_CT_LIST)
			Output.Append("List");
		
		if (Type.TypeQualifier == ZEMC_TQ_REFERENCE)
			Output.Append("Ref");
		else if (Type.TypeQualifier == ZEMC_TQ_CONST_REFERENCE)
			Output.Append("ConstRef");

		Output.Append("<");
		Output.Append(ConvertBaseTypeToName(Type));
		Output.Append(">");

		if (Type.BaseType == ZEMC_BT_OBJECT_PTR && Type.Class->IsForwardDeclared)
			SetterExtraParams = ZEFormat::Format(", {0}_Class()", Type.Class->Name);
	}
	else if (Type.BaseType == ZEMC_BT_OBJECT)
	{
		Output.Append("Object");

		if (Type.TypeQualifier == ZEMC_TQ_REFERENCE)
			Output.Append("Ref");
		else if (Type.TypeQualifier == ZEMC_TQ_CONST_REFERENCE)
			Output.Append("ConstRef");

		Output.Append("<");
		Output.Append(Type.Class->Name);
		Output.Append(">");
	}
	else 
	{
		switch(Type.BaseType)
		{
			default:
				return "";

			case ZEMC_BT_INTEGER_8:
				Output.Append("Int8");
				break;

			case ZEMC_BT_INTEGER_16:
				Output.Append("Int16");
				break;

			case ZEMC_BT_ENUMERATOR:
				Output.Append("Int32");
				CastOutput = ZEFormat::Format("({0})", Type.Enumurator->Name.ToCString());
				break;

			case ZEMC_BT_INTEGER_32:
				Output.Append("Int32");
				break;

			case ZEMC_BT_UNSIGNED_INTEGER_8:
				Output.Append("UInt8");
				break;

			case ZEMC_BT_UNSIGNED_INTEGER_16:
				Output.Append("UInt16");
				break;

			case ZEMC_BT_UNSIGNED_INTEGER_32:
				Output.Append("UInt32");
				break;

			case ZEMC_BT_INTEGER_64:
				Output.Append("Int64");
				break;

			case ZEMC_BT_UNSIGNED_INTEGER_64:
				Output.Append("UInt64");
				break;

			case ZEMC_BT_FLOAT:
				Output.Append("Float");
				break;

			case ZEMC_BT_DOUBLE:
				Output.Append("Double");
				break;

			case ZEMC_BT_BOOLEAN:
				Output.Append("Bool");
				break;

			case ZEMC_BT_STRING:
				Output.Append("String");
				break;

			case ZEMC_BT_QUATERNION:
				Output.Append("Quaternion");
				break;

			case ZEMC_BT_VECTOR2:
				Output.Append("Vector2");
				break;

			case ZEMC_BT_VECTOR2D:
				Output.Append("Vector2d");
				break;

			case ZEMC_BT_VECTOR3:
				Output.Append("Vector3");
				break;

			case ZEMC_BT_VECTOR3D:
				Output.Append("Vector3d");
				break;

			case ZEMC_BT_VECTOR4:
				Output.Append("Vector4");
				break;

			case ZEMC_BT_VECTOR4D:
				Output.Append("Vector4d");
				break;

			case ZEMC_BT_MATRIX3X3:
				Output.Append("Matrix3x3");
				break;

			case ZEMC_BT_MATRIX3X3D:
				Output.Append("Matrix3x3d");
				break;

			case ZEMC_BT_MATRIX4X4:
				Output.Append("Matrix4x4");
				break;

			case ZEMC_BT_MATRIX4X4D:
				Output.Append("Matrix4x4d");
				break;

			case ZEMC_BT_OBJECT_PTR:
				Output.Append("ObjectPtr");
				CastOutput = ZEFormat::Format("({0}*)", Type.Class->Name);
				break;

			case ZEMC_BT_CLASS:
				Output.Append("Class");
				break;
		}

		if (Type.TypeQualifier == ZEMC_TQ_CONST_VALUE)
			Output.Append("Const");
		else if (Type.TypeQualifier == ZEMC_TQ_REFERENCE)
			Output.Append("Ref");
		else if (Type.TypeQualifier == ZEMC_TQ_CONST_REFERENCE)
			Output.Append("ConstRef");
	}

	return Output;
}

ZEString ZEMCGenerator::GenerateTypeSignature(const ZEMCType& Type)
{
	ZEString Output;
	if (Type.TypeQualifier == ZEMC_TQ_CONST_VALUE || Type.TypeQualifier == ZEMC_TQ_CONST_REFERENCE)
		Output.Append("const ");

	if (Type.ContainerType == ZEMC_CT_ARRAY)
		Output.Append("ZEArray<");
	else if (Type.ContainerType == ZEMC_CT_LIST)
		Output.Append("ZEList<");

	Output.Append(ConvertBaseTypeToName(Type));

	if (Type.ContainerType == ZEMC_CT_ARRAY || Type.ContainerType == ZEMC_CT_LIST)
		Output.Append(">");

	if (Type.TypeQualifier == ZEMC_TQ_REFERENCE || Type.TypeQualifier == ZEMC_TQ_CONST_REFERENCE)
		Output.Append("&");

	return Output;
}

ZEString ZEMCGenerator::GenerateTypeConstructor(const ZEMCType& Type)
{
	return ZEFormat::Format("ZEMTType({0}, {1}, {2}, {3}, {4})",
		ConvertBaseTypeToEnum(Type.BaseType), 
		ConvertTypeQualifierToEnum(Type.TypeQualifier),
		ConvertContainerTypeToEnum(Type.ContainerType),
		Type.BaseType == ZEMC_BT_OBJECT || Type.BaseType == ZEMC_BT_OBJECT_PTR ? 
			(Type.Class->IsForwardDeclared ? ZEFormat::Format("{0}_Class()", Type.Class->Name) : ZEFormat::Format("{0}::Class()", Type.Class->Name))  : 
			"NULL",
		Type.BaseType == ZEMC_BT_ENUMERATOR ? ZEFormat::Format("{0}_Enumerator()", Type.Enumurator->Name) : "NULL");
}
