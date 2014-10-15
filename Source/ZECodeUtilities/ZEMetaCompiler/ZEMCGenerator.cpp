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

#include "ZEFile\ZEFileInfo.h"
#include "ZEDS\ZEFormat.h"


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
	va_start (Args, Format);

	vfprintf(OutputFile, Format, Args);

	va_end (Args);
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

void ZEMCGenerator::GenerateHeading()
{
	WriteToFile("//////////////////////////////////////////////////\n");
	WriteToFile("//  ZEMetaCompiler auto generated ZEMeta file.  //\n");
	WriteToFile("//      DO NOT EDIT (HACK) THIS FILE !!!        //\n");
	WriteToFile("//////////////////////////////////////////////////\n");
	WriteToFile("\n\n");

	WriteToFile("#include \"%s\"\n", ZEFileInfo::GetFileName(Options->InputFileName).ToCString());
	WriteToFile("#include \"ZEDS/ZEVariant.h\"\n");
	WriteToFile("#include \"ZEDS/ZEReference.h\"\n");
	WriteToFile("#include \"ZEScript/ZEScriptEngine.h\"\n");
	WriteToFile("#include <stddef.h>\n\n");
}

void ZEMCGenerator::GenerateEnding()
{

}

const char* ZEMCGenerator::ConvertOperatorTypeToName(ZEMCMetaOperatorType OperatorType)
{
	switch (OperatorType)
	{
		case ZEMC_MOT_ADDITION:
			return "opAddition";
		case ZEMC_MOT_ADDITION_ASSIGNMENT:
			return "opAdditionAssignment";
		case ZEMC_MOT_INCREMENT:
			return "opIncrement";
		case ZEMC_MOT_SUBTRACTION:
			return "opSubtraction";
		case ZEMC_MOT_SUBSTRACTION_ASSIGNMENT:
			return "opSubtractionAssignment";
		case ZEMC_MOT_DECREMENT:
			return "opDecrement";
		case ZEMC_MOT_MULTIPLICATION:
			return "opMultiplication";
		case ZEMC_MOT_MULTIPLICATION_ASSIGNMENT:
			return "opMultiplicationAssignment";
		case ZEMC_MOT_DIVISION:
			return "opDivision";
		case ZEMC_MOT_DIVISION_ASSIGNMENT:
			return "opDivisionAssignment";
		case ZEMC_MOT_MODULO:
			return "opModulo";
		case ZEMC_MOT_MODULO_ASSIGNMENT:
			return "opModuloAssignment";
		case ZEMC_MOT_BITWISE_AND:
			return "opBitwiseAnd";
		case ZEMC_MOT_BITWISE_AND_ASSIGNMENT:
			return "opBitwiseAndAssignment";
		case ZEMC_MOT_LOGICAL_AND:
			return "opLogicalAnd";
		case ZEMC_MOT_BITWISE_OR:
			return "opBitwiseOr";
		case ZEMC_MOT_BITWISE_OR_ASSIGNMENT:
			return "opBitwiseOrAssignment";
		case ZEMC_MOT_LOGICAL_OR:
			return "opLogicalOr";
		case ZEMC_MOT_BITWISE_XOR:
			return "opBitwiseXor";
		case ZEMC_MOT_BITWISE_XOR_ASSIGNMENT:
			return "opBitwiseXorAssignment";
		case ZEMC_MOT_ASSIGNMENT:
			return "opAssignment";
		case ZEMC_MOT_EQUAL:
			return "opEqual";
		case ZEMC_MOT_NOT_EQUAL:
			return "opNotEquals";
		case ZEMC_MOT_LESS:
			return "opLess";
		case ZEMC_MOT_LESS_EQUAL:
			return "opCmp";
		case ZEMC_MOT_LEFT_SHIFT:
			return "opLeftShift";
		case ZEMC_MOT_LEFT_SHIFT_ASSIGNMENT:
			return "opLeftShiftAssignment";
		case ZEMC_MOT_GREATER:
			return "opGreater";
		case ZEMC_MOT_GREATER_AND_EQUAL:
			return "opGreaterAndEqual";
		case ZEMC_MOT_RIGHT_SHIFT:
			return "opRightShift";
		case ZEMC_MOT_RIGHT_SHIFT_ASSIGNMENT:
			return "opRightShiftAssignment";
		case ZEMC_MOT_FUNCTION_CALL:
			return "opFunctionCall";
		case ZEMC_MOT_ARRAY_SUBSCRIPT:
			return "opArraySubscript";
		default:
			return "";
	}
}

const char* ZEMCGenerator::ConvertOperatorTypeToString(ZEMCMetaOperatorType OperatorType)
{
	switch (OperatorType)
	{
		case ZEMC_MOT_ADDITION:
			return "ZE_MOT_PLUS";
		case ZEMC_MOT_ADDITION_ASSIGNMENT:
			return "ZE_MOT_PLUSEQUAL";
		case ZEMC_MOT_INCREMENT:
			return "ZE_MOT_PLUSPLUS";
		case ZEMC_MOT_SUBTRACTION:
			return "ZE_MOT_MINUS";
		case ZEMC_MOT_SUBSTRACTION_ASSIGNMENT:
			return "ZE_MOT_MINUSEQUAL";
		case ZEMC_MOT_DECREMENT:
			return "ZE_MOT_MINUSMINUS";
		case ZEMC_MOT_MULTIPLICATION:
			return "ZE_MOT_STAR";
		case ZEMC_MOT_MULTIPLICATION_ASSIGNMENT:
			return "ZE_MOT_STAREQUAL";
		case ZEMC_MOT_DIVISION:
			return "ZE_MOT_SLASH";
		case ZEMC_MOT_DIVISION_ASSIGNMENT:
			return "ZE_MOT_SLASHEQUAL";
		case ZEMC_MOT_MODULO:
			return "ZE_MOT_PERCENT";
		case ZEMC_MOT_MODULO_ASSIGNMENT:
			return "ZE_MOT_PERCENTEQUAL";
		case ZEMC_MOT_BITWISE_AND:
			return "ZE_MOT_AMP";
		case ZEMC_MOT_BITWISE_AND_ASSIGNMENT:
			return "ZE_MOT_AMPEQUAL";
		case ZEMC_MOT_LOGICAL_AND:
			return "ZE_MOT_AMPAMP";
		case ZEMC_MOT_BITWISE_OR:
			return "ZE_MOT_PIPE";
		case ZEMC_MOT_BITWISE_OR_ASSIGNMENT:
			return "ZE_MOT_PIPEEQUAL";
		case ZEMC_MOT_LOGICAL_OR:
			return "ZE_MOT_PIPEPIPE";
		case ZEMC_MOT_BITWISE_XOR:
			return "ZE_MOT_CARET";
		case ZEMC_MOT_BITWISE_XOR_ASSIGNMENT:
			return "ZE_MOT_CARETEQUAL";
		case ZEMC_MOT_ASSIGNMENT:
			return "ZE_MOT_EQUAL";
		case ZEMC_MOT_EQUAL:
			return "ZE_MOT_EQUALEQUAL";
		case ZEMC_MOT_NOT_EQUAL:
			return "ZE_MOT_EXCLAIMEQUAL";
		case ZEMC_MOT_LESS:
			return "ZE_MOT_LESS";
		case ZEMC_MOT_LESS_EQUAL:
			return "ZE_MOT_LESSEQUAL";
		case ZEMC_MOT_LEFT_SHIFT:
			return "ZE_MOT_LESSLESS";
		case ZEMC_MOT_LEFT_SHIFT_ASSIGNMENT:
			return "ZE_MOT_LESSLESSEQUAL";
		case ZEMC_MOT_GREATER:
			return "ZE_MOT_GREATER";
		case ZEMC_MOT_GREATER_AND_EQUAL:
			return "ZE_MOT_GREATEREQUAL";
		case ZEMC_MOT_RIGHT_SHIFT:
			return "ZE_MOT_GREATERGREATER";
		case ZEMC_MOT_RIGHT_SHIFT_ASSIGNMENT:
			return "ZE_MOT_GREATERGREATEREQUAL";
		case ZEMC_MOT_FUNCTION_CALL:
			return "ZE_MOT_CALL";
		case ZEMC_MOT_ARRAY_SUBSCRIPT:
			return "ZE_MOT_SUBSCRIPT";
		default:
			return "ZE_MOT_UNDEFINED";
		}
}

const char* ZEMCGenerator::ConvertBaseTypeToEnum(ZEMCBaseType BaseType)
{
	switch (BaseType)
	{
		default:
		case ZEMC_BT_UNDEFINED:
			return "ZE_TT_UNDEFINED";

		case ZEMC_BT_VOID:
			return "ZE_TT_NULL";

		case ZEMC_BT_INTEGER_8:
			return "ZE_TT_INTEGER_8";

		case ZEMC_BT_INTEGER_16:
			return "ZE_TT_INTEGER_16";

		case ZEMC_BT_INTEGER_32:
			return "ZE_TT_INTEGER_32";

		case ZEMC_BT_UNSIGNED_INTEGER_8:
			return "ZE_TT_UNSIGNED_INTEGER_8";

		case ZEMC_BT_UNSIGNED_INTEGER_16:
			return "ZE_TT_UNSIGNED_INTEGER_16";

		case ZEMC_BT_UNSIGNED_INTEGER_32:
			return "ZE_TT_UNSIGNED_INTEGER_32";

		case ZEMC_BT_INTEGER_64:
			return "ZE_TT_INTEGER_64";

		case ZEMC_BT_UNSIGNED_INTEGER_64:
			return "ZE_TT_UNSIGNED_INTEGER_64";

		case ZEMC_BT_FLOAT:
			return "ZE_TT_FLOAT";

		case ZEMC_BT_DOUBLE:
			return "ZE_TT_DOUBLE";

		case ZEMC_BT_BOOLEAN:
			return "ZE_TT_BOOLEAN";

		case ZEMC_BT_STRING:
			return "ZE_TT_STRING";

		case ZEMC_BT_QUATERNION:
			return "ZE_TT_QUATERNION";

		case ZEMC_BT_VECTOR2:
			return "ZE_TT_VECTOR2";

		case ZEMC_BT_VECTOR3:
			return "ZE_TT_VECTOR3";

		case ZEMC_BT_VECTOR4:
			return "ZE_TT_VECTOR4";

		case ZEMC_BT_MATRIX3X3:
			return "ZE_TT_MATRIX3X3";

		case ZEMC_BT_MATRIX4X4:
			return "ZE_TT_MATRIX4X4";

		case ZEMC_BT_CLASS:
			return "ZE_TT_CLASS";

		case ZEMC_BT_OBJECT:
			return "ZE_TT_OBJECT";

		case ZEMC_BT_OBJECT_PTR:
			return "ZE_TT_OBJECT_PTR";

		case ZEMC_BT_ENUMERATOR:
			return "ZE_TT_ENUMERATOR";
	}
}

const char* ZEMCGenerator::ConvertContainerTypeToEnum(ZEMCContainerType ContainerType)
{
	switch (ContainerType)
	{
		default:
		case ZEMC_CT_NONE:
			return "ZE_CT_NONE";
		case ZEMC_CT_ARRAY:
			return "ZE_CT_ARRAY";
		case ZEMC_CT_LIST:
			return "ZE_CT_LIST";
		case ZEMC_CT_CONTAINER:
			return "ZE_CT_CONTAINER";
	}
}

const char* ZEMCGenerator::ConvertTypeQualifierToEnum(ZEMCTypeQualifier TypeQualifier)
{
	switch (TypeQualifier)
	{
		default:
		case ZEMC_TQ_VALUE:
			return "ZE_TQ_VALUE";

		case ZEMC_TQ_CONST_VALUE:
			return "ZE_TQ_CONST_VALUE";

		case ZEMC_TQ_REFERENCE:
			return "ZE_TQ_REFERENCE";

		case ZEMC_TQ_CONST_REFERENCE:
			return "ZE_TQ_CONST_REFERENCE";
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

		case ZEMC_BT_VECTOR3:
			return "ZEVector3";

		case ZEMC_BT_VECTOR4:
			return "ZEVector4";

		case ZEMC_BT_MATRIX3X3:
			return "ZEMatrix3x3";

		case ZEMC_BT_MATRIX4X4:
			return "ZEMatrix4x4";

		case ZEMC_BT_CLASS:
			return "ZEClass*";

		case ZEMC_BT_OBJECT:
			return Type.Class->Name.ToCString();

		case ZEMC_BT_OBJECT_PTR:
			return ZEFormat::Format("{0}*", Type.Class->Name);
	}
}
ZEString ZEMCGenerator::GenerateVariantPostfix(const ZEMCType& Type, ZEString& CastOutput)
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

			case ZEMC_BT_VECTOR3:
				Output.Append("Vector3");
				break;

			case ZEMC_BT_VECTOR4:
				Output.Append("Vector4");
				break;

			case ZEMC_BT_MATRIX3X3:
				Output.Append("Matrix3x3");
				break;

			case ZEMC_BT_MATRIX4X4:
				Output.Append("Matrix4x4");
				break;

			case ZEMC_BT_OBJECT_PTR:
				Output.Append("ObjectPtr");
				CastOutput = ZEFormat::Format("({0})", Type.Class->Name);
				break;

			case ZEMC_BT_CLASS:
				Output.Append("Class");
				break;
		}

		if (Type.TypeQualifier == ZEMC_TQ_REFERENCE)
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
	return ZEFormat::Format("ZEType({0}, {1}, {2}, {3}, {4})",
		ConvertBaseTypeToEnum(Type.BaseType), 
		ConvertTypeQualifierToEnum(Type.TypeQualifier),
		ConvertContainerTypeToEnum(Type.ContainerType),
		Type.BaseType == ZEMC_BT_OBJECT || Type.BaseType == ZEMC_BT_OBJECT_PTR ? ZEFormat::Format("{0}::Class()", Type.Class->Name) : "NULL",
		Type.BaseType == ZEMC_BT_ENUMERATOR ? ZEFormat::Format("{0}_Declaration()", Type.Enumurator->Name) : "NULL");
}
