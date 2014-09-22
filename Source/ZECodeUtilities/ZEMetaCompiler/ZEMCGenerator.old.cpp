//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGenerator.old.cpp
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

#include "ZEDS/ZEList.h"
#include "ZEDS/ZEType.h"
#include "ZEFile/ZEFileInfo.h"

#include <stdio.h>

void SetMetaData(ZEMCContext* Data)
{
	
}

void SetOptions(ZEMCOptions* Options)
{

}


static ZEInt SortPropertiesByHash(ZEMCProperty* const* PropertyDataA, ZEMCProperty* const* PropertyDataB)
{
	ZEUInt32 HashA = (**PropertyDataA).Hash;
	ZEUInt32 HashB = (**PropertyDataB).Hash;

	if(HashA > HashB)
		return 1;
	else if (HashA == HashB)
		return 0;
	else
		return -1;
}

static ZEInt SortMethodsByHash(ZEMCMethod* const* MethodDataA, ZEMCMethod* const* MethodDataB)
{
	ZEUInt32 HashA = (**MethodDataA).Hash;
	ZEUInt32 HashB = (**MethodDataB).Hash;

	if(HashA > HashB)
		return 1;
	else if (HashA == HashB)
		return 0;
	else
		return -1;
}

static const char* GetOperatorName(ZEMCMetaOperatorType OperatorType)
{
	switch(OperatorType)
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

static const char* GetOperatorType(ZEMCMetaOperatorType OperatorType)
{
	switch(OperatorType)
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
		return "";
	}
}

static const char* GetTypeString(ZEMCBaseType ParameterType)
{
	switch(ParameterType)
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

		case ZEMC_BT_ARRAY:
			return "ZE_TT_ARRAY";

		case ZEMC_BT_LIST:
			return "ZE_TT_LIST";

		case ZEMC_BT_ENUMERATOR:
			return "ZE_TT_ENUMERATOR";
	}
}

static const char* GetTypeQualifierString(ZEMCTypeQualifier TypeQualifier)
{
	switch(TypeQualifier)
	{
		default:
		case ZE_TQ_VALUE:
			return "ZE_TQ_VALUE";

		case ZE_TQ_REFERENCE:
			return "ZE_TQ_REFERENCE";

		case ZE_TQ_CONST_REFERENCE:
			return "ZE_TQ_CONST_REFERENCE";
	}
}

static const char* ReturnVariantType(ZEMCBaseType VariantType)
{
	switch(VariantType)
	{
		default:
			return "NULL";

		case ZEMC_BT_INTEGER_8:
			return "Int8";

		case ZEMC_BT_INTEGER_16:
			return "Int16";

		case ZEMC_BT_INTEGER_32:
			return "Int32";

		case ZEMC_BT_UNSIGNED_INTEGER_8:
			return "UInt8";

		case ZEMC_BT_UNSIGNED_INTEGER_16:
			return "UInt16";

		case ZEMC_BT_UNSIGNED_INTEGER_32:
			return "UInt32";

		case ZEMC_BT_INTEGER_64:
			return "Int64";

		case ZEMC_BT_UNSIGNED_INTEGER_64:
			return "UInt64";

		case ZEMC_BT_FLOAT:
			return "Float";

		case ZEMC_BT_DOUBLE:
			return "Double";

		case ZEMC_BT_BOOLEAN:
			return "Bool";

		case ZEMC_BT_ENUMERATOR:
			return "Enum";

		case ZEMC_BT_STRING:
			return "String";

		case ZEMC_BT_QUATERNION:
			return "Quaternion";

		case ZEMC_BT_VECTOR2:
			return "Vector2";

		case ZEMC_BT_VECTOR3:
			return "Vector3";

		case ZEMC_BT_VECTOR4:
			return "Vector4";

		case ZEMC_BT_MATRIX3X3:
			return "Matrix3x3";

		case ZEMC_BT_MATRIX4X4:
			return "Matrix4x4";

		case ZEMC_BT_ARRAY:
			return "Array";

		case ZEMC_BT_OBJECT:
			return "Object";

		case ZEMC_BT_OBJECT_PTR:
			return "ObjectPtr";

		case ZEMC_BT_CLASS:
			return "Class";
	}
}

static const char* ReturnTypeCast(ZEMCBaseType VariantType)
{
	switch(VariantType)
	{
		default:
			return "NULL";

		case ZEMC_BT_INTEGER_8:
			return "ZEInt8";

		case ZEMC_BT_INTEGER_16:
			return "ZEInt16";

		case ZEMC_BT_INTEGER_32:
			return "ZEInt32";

		case ZEMC_BT_UNSIGNED_INTEGER_8:
			return "ZEUInt8";

		case ZEMC_BT_UNSIGNED_INTEGER_16:
			return "ZEUInt16";

		case ZEMC_BT_UNSIGNED_INTEGER_32:
			return "ZEUInt32";

		case ZEMC_BT_INTEGER_64:
			return "ZEInt64";

		case ZEMC_BT_UNSIGNED_INTEGER_64:
			return "ZEUInt64";

		case ZEMC_BT_FLOAT:
			return "float";

		case ZEMC_BT_DOUBLE:
			return "double";

		case ZEMC_BT_BOOLEAN:
			return "bool";

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

		case ZEMC_BT_ARRAY:
			return "ZEArray";

		case ZEMC_BT_CLASS:
			return "ZEClass";

		case ZEMC_BT_OBJECT:
			return "ZEObject";

		case ZEMC_BT_OBJECT_PTR:
			return "ZEObject*";

		case ZEMC_BT_VOID:
			return "void";
	}
}

static void PrepareClassDependencies(FILE* File, const char* ClassName, ZEArray<ZEMCForwardDeclaration*> ForwardDeclaredClasses)
{
	for(ZESize I = 0; I < ForwardDeclaredClasses.GetCount(); I++)
	{
		if(ForwardDeclaredClasses[I]->HeaderFileDeclaredIn == ClassName)
			fprintf(File, "#include \"%s\"\n", ForwardDeclaredClasses[I]->HeaderName.ToCString());
	}

	fprintf(File, "\n");
}

static void AppendScriptEngineCallMethod(FILE* File, ZEMCMethod* Method, ZESize Index)
{
	for(ZESize J = 0; J < Method->Parameters.GetCount(); J++)
	{
		if(Method->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT || Method->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
		{
			fprintf(File, "\t\t\t%sScriptFunction%d->Parameters[%d].SetObjectPtr(%s);\n", 
				Method->IsOperator && Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(),
				Index, J,
				Method->Parameters[J]->Name.ToCString());
		}
		else
		{
			fprintf(File, "\t\t\t%sScriptFunction%d->Parameters[%d].Set%s%s%s(%s);\n", 
				Method->IsOperator && Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(),
				Index, J, 
				ReturnVariantType(Method->Parameters[J]->Type.BaseType),
				Method->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
				Method->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Method->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE ? "Ref" : "",
				Method->Parameters[J]->Name.ToCString());
		}
	}

	if(Method->ReturnValue.BaseType != ZEMC_BT_VOID && Method->Name != "operator=")
	{
		if(Method->ReturnValue.BaseType == ZEMC_BT_ARRAY)
		{
			fprintf(File, 
				"\t\t\treturn *(ZEArray<%s*>*)(ScriptEngine->CallScriptFunction(%sScriptFunction%d));\n",
				Method->ReturnValue.Class->Name.ToCString(),
				Method->IsOperator && Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(), Index);
		}
		else if(Method->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
		{
			fprintf(File, 
				"\t\t\treturn *((%s*)(ScriptEngine->CallScriptFunction(%sScriptFunction%d)));\n", 
				Method->ReturnValue.Enumurator->Name.ToCString(),
				Method->IsOperator && Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(), Index);
		}
		else if(Method->ReturnValue.BaseType == ZEMC_BT_OBJECT || Method->ReturnValue.BaseType == ZEMC_BT_OBJECT_PTR)
		{
			if(Method->Name == "CreateInstance")
			{
				fprintf(File, 
					"\t\t\treturn (%s*)(ScriptEngine->CallScriptFunction(%sScriptFunction%d));\n", 
					Method->MemberOf.ToCString(),
					Method->IsOperator && Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(), Index);
			}
			else
			{
				fprintf(File, 
					"\t\t\treturn ((%s*)(ScriptEngine->CallScriptFunction(%sScriptFunction%d)));\n",
					Method->ReturnValue.Class->Name.ToCString(),
					Method->IsOperator && Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(), Index);
			}
		}
		else
		{
			fprintf(File, 
				"\t\t\treturn *((%s*)(ScriptEngine->CallScriptFunction(%sScriptFunction%d)));\n",
				ReturnTypeCast(Method->ReturnValue.BaseType),
				Method->IsOperator && Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(), Index);
		}
	}
	else
	{
		fprintf(File, 
			"\t\t\tScriptEngine->CallScriptFunction(%sScriptFunction%d);\n",
			Method->Name == "operator=" ? "opAssign" : Method->Name.ToCString(), Index);
	}
}

static void CreateScriptBaseClass(FILE* File, const char* ClassName, ZEArray<ZEMCMethod*> Methods, bool IsAbstract, bool HasPublicConstructor)
{
	return;

	if(!HasPublicConstructor)
		return;

	fprintf(File, 
		"class %sScriptBase : public %s\n"
		"{\n", ClassName, ClassName);

	fprintf(File,
		"\tpublic:\n"
		"\t\tvoid*\t\t\t\tScriptObject;\n"
		"\t\tZEScriptEngine*\t\tScriptEngine;\n");

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(!Methods[I]->IsVirtual)
			continue;

		fprintf(File, "\t\tZEScriptFunction*\t%sScriptFunction%d;\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I);
	}

	if(HasPublicConstructor && !IsAbstract)
		fprintf(File, "\n\t\t%sScriptBase()\n\t\t{\n\n\t\t}\n\n\t\t~%sScriptBase()\n\t\t{\n\n\t\t}\n\n", ClassName, ClassName);

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(!Methods[I]->IsVirtual)
			continue;

		if(Methods[I]->IsPure)
			continue;

		///////////////////////////////////////////////////////////////////////////
		//     This augmented method which we create is supposed to prepare stack
		// for script object that derives from him. Most scripting systems dont
		// support native C++ class inheritance in scripts. So we are creating
		// this virtual methods to simulate inheritance mechanisms in scripts.
		///////////////////////////////////////////////////////////////////////////

		if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
		{
			if(Methods[I]->ReturnValue.SubType == ZEMC_BT_UNDEFINED);
				continue;

			fprintf(File, 
				"\t\tvirtual %s%sZEArray<%s%s%s>%s%s %s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR ? Methods[I]->ReturnValue.Class->Name.ToCString() : ReturnTypeCast(Methods[I]->ReturnValue.SubType),
				(Methods[I]->ReturnValue.SubType != ZEMC_BT_OBJECT_PTR && (Methods[I]->ReturnValue.SubTypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.SubTypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
				Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
				(Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR && (Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
				Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT_PTR ? "*" : "",
				Methods[I]->Name.ToCString());
		}
		else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
		{
			fprintf(File, 
				"\t\tvirtual %s%s%s%s %s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				Methods[I]->ReturnValue.Enumurator->Name.ToCString(),
				(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
				Methods[I]->Name.ToCString());
		}
		else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT_PTR)
		{
			fprintf(File, 
				"\t\tvirtual %s%s%s* %s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				Methods[I]->ReturnValue.Class->Name.ToCString(),
				Methods[I]->Name.ToCString());
		}
		else
		{
			fprintf(File, 
				"\t\tvirtual %s%s%s%s %s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
				(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
				Methods[I]->Name.ToCString());
		}

		for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
		{
			if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
			{
				if(Methods[I]->ReturnValue.SubType == ZEMC_BT_UNDEFINED);
				continue;

				fprintf(File,
					"%sZEArray<%s%s%s>%s%s %s%s",
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? Methods[I]->Parameters[J]->Type.Class->Name.ToCString() : ReturnTypeCast(Methods[I]->Parameters[J]->Type.SubType),
					(Methods[I]->Parameters[J]->Type.SubType != ZEMC_BT_OBJECT_PTR && (Methods[I]->Parameters[J]->Type.SubTypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.SubTypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
					Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
					(Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR && (Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
					Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR ? "*" : "",
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
			else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
			{
				fprintf(File, 
					"%s%s%s %s%s", 
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(),
					(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
			else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
			{
				fprintf(File, 
					"%s%s* %s%s", 
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
			else
			{
				fprintf(File, 
					"%s%s%s %s%s", 
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					ReturnTypeCast(Methods[I]->Parameters[J]->Type.BaseType),
					(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
		}

		fprintf(File, ")%s\n\t\t{\n", Methods[I]->IsConst ? " const" : "");

		AppendScriptEngineCallMethod(File, Methods[I], I);

		fprintf(File, "\t\t}\n\n");


		///////////////////////////////////////////////////////////////////////////
		//       This second NOT virtual method will be called from script side via
		// method wrappers for triggering script object's C++ parent. We create this
		// method for backward inheritance calling mechanism which is not supported 
		// natively in most scripting systems.
		///////////////////////////////////////////////////////////////////////////

		if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
		{
			if(Methods[I]->ReturnValue.SubType == ZEMC_BT_UNDEFINED);
			continue;

			fprintf(File, 
				"\t\t%s%sZEArray<%s%s%s>%s%s ScriptTriggered%s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR ? Methods[I]->ReturnValue.Class->Name.ToCString() : ReturnTypeCast(Methods[I]->ReturnValue.SubType),
				(Methods[I]->ReturnValue.SubType != ZEMC_BT_OBJECT_PTR && (Methods[I]->ReturnValue.SubTypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.SubTypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
				Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
				(Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR && (Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
				Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT_PTR ? "*" : "",
				Methods[I]->Name.ToCString());
		}
		else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
		{
			fprintf(File, 
				"\t\t%s%s%s%s ScriptTriggered%s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				Methods[I]->ReturnValue.Enumurator->Name.ToCString(),
				(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
				Methods[I]->Name.ToCString());
		}
		else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT_PTR)
		{
			fprintf(File, 
				"\t\t%s%s%s* ScriptTriggered%s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				Methods[I]->ReturnValue.Class->Name.ToCString(),
				Methods[I]->Name.ToCString());
		}
		else
		{
			fprintf(File, 
				"\t\t%s%s%s%s ScriptTriggered%s(", 
				Methods[I]->IsStatic ? "static " : "",
				Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
				ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
				(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
				Methods[I]->Name.ToCString());
		}

		for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
		{
			if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
			{
				if(Methods[I]->ReturnValue.SubType == ZEMC_BT_UNDEFINED);
				continue;

				fprintf(File,
					"%sZEArray<%s%s%s>%s%s %s%s",
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? Methods[I]->Parameters[J]->Type.Class->Name.ToCString() : ReturnTypeCast(Methods[I]->Parameters[J]->Type.SubType),
					(Methods[I]->Parameters[J]->Type.SubType != ZEMC_BT_OBJECT_PTR && (Methods[I]->Parameters[J]->Type.SubTypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.SubTypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
					Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
					(Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR && (Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE)) ? "&" : "",
					Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR ? "*" : "",
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
			else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
			{
				fprintf(File, 
					"%s%s%s %s%s", 
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(),
					(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
			else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
			{
				fprintf(File, 
					"%s%s* %s%s", 
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
			else
			{
				fprintf(File, 
					"%s%s%s %s%s", 
					Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					ReturnTypeCast(Methods[I]->Parameters[J]->Type.BaseType),
					(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) ? "&" : "",
					Methods[I]->Parameters[J]->Name.ToCString(),
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "");
			}
		}

		fprintf(File, ")%s\n\t\t{\n", Methods[I]->IsConst ? " const" : "");

		fprintf(File, 
			"\t\t\t%s%s::%s(", 
			Methods[I]->ReturnValue.BaseType == ZEMC_BT_VOID ? "" : "return ",
			ClassName, Methods[I]->Name.ToCString());

		for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
		{
			if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
			{
				if(Methods[I]->Parameters.GetCount() > 0)
				{
					fprintf(File, "(%s*)%s%s", 
						Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
						Methods[I]->Parameters[J]->Name.ToCString(),
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
				}
				else
				{
					fprintf(File, "(%s*)%s%s", 
						Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
						Methods[I]->Parameters[J]->Name.ToCString(),
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
				}
			}
			else
			{
				fprintf(File, "%s%s", 
					Methods[I]->Parameters[J]->Name.ToCString(), 
					J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
			}
		}

		if(Methods[I]->Parameters.GetCount() == 0)
			fprintf(File, ");\n\t\t}\n\n");
		else
			fprintf(File, "\t\t}\n\n");

	}

	fprintf(File, "};\n\n");
}

static void CreateGetParentClassMethod(FILE* File, const char* ClassName, const char* ParentClass)
{
	fprintf(File, 
		"ZEClass* %sClass::GetParentClass()\n"
		"{\n"
		"\treturn %s::Class();\n"
		"}\n\n",
		ClassName, ParentClass);
}

static void CreateGetNameMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"const char* %sClass::GetName()\n"
		"{\n"
		"\treturn \"%s\";\n"
		"}\n\n",
		ClassName, ClassName);
}

static void CreateGetAttributesMethod(FILE* File, const char* ClassName, ZEArray<ZEMCAttribute*> Attributes)
{
	fprintf(File, 
		"const ZEMetaAttribute* %sClass::GetAttributes()\n"
		"{\n", ClassName);

	for(ZESize I = 0; I < Attributes.GetCount(); I++)
	{
		fprintf(File,
			"\tstatic const char* Attribute%dParameters[%d] = {", I, Attributes[I]->Parameters.GetCount());

		for(ZESize J = 0; J < Attributes[I]->Parameters.GetCount(); J++)
		{
			fprintf(File, "\"%s\"%s", Attributes[I]->Parameters[J].ToCString(), J != Attributes[I]->Parameters.GetCount() - 1 ? ", " : "");
		}

		fprintf(File, "};\n\n");
	}

	if(Attributes.GetCount() > 0)
	{
		fprintf(File,
			"\tstatic ZEMetaAttribute Attributes[%d] =\n"
			"\t{\n", Attributes.GetCount());
	}

	for(ZESize I = 0; I < Attributes.GetCount(); I++)
	{
		fprintf(File,
			"\t\t{\"%s\", Attribute%dParameters, %d}%s\n",
			Attributes[I]->Name.ToCString(), I, Attributes[I]->Parameters.GetCount(), I != Attributes.GetCount() - 1 ? "," : "");
	}

	if(Attributes.GetCount() > 0)
	{
		fprintf(File,
			"\t};\n\n"
			"\treturn Attributes;\n"
			"}\n\n");
	}
	else
		fprintf(File, "\treturn NULL;\n}\n\n");
}

static void CreateGetAttributeCountMethod(FILE* File, const char* ClassName, ZESize AttributeCount)
{
	fprintf(File, 
		"ZESize %sClass::GetAttributeCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		ClassName, AttributeCount);
}

static void CreateGetPropertiesMethod(FILE* File, bool IsBuiltInClass, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File, 
		"const ZEProperty* %sClass::GetProperties()\n"
		"{\n", ClassName);

	//Creating Attributes
	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if(Properties[I]->Attributes.GetCount() > 0)
		{
			for(ZESize J = 0; J < Properties[I]->Attributes.GetCount(); J++)
			{
				if(Properties[I]->Attributes[J]->Parameters.GetCount() > 0)
				{
					fprintf(File, "\tstatic const char* Property%dAttribute%dParameters[%d] = {", I, J, Properties[I]->Attributes[J]->Parameters.GetCount());
			
					for(ZESize K = 0; K < Properties[I]->Attributes[J]->Parameters.GetCount(); K++)
					{
						fprintf(File, "\"%s\"%s", Properties[I]->Attributes[J]->Parameters[K].ToCString(), K != Properties[I]->Attributes[J]->Parameters.GetCount() - 1 ? ", " : "");
					}

					fprintf(File, "};\n\n");
				}
			}

			fprintf(File, 
				"\tstatic ZEMetaAttribute Property%dAttributes[%d] =\n"
				"\t{\n", I, Properties[I]->Attributes.GetCount());

			for(ZESize J = 0; J < Properties[I]->Attributes.GetCount(); J++)
			{
				if(Properties[I]->Attributes[J]->Parameters.GetCount() > 0)
				{
					fprintf(File, 
						"\t\t{\"%s\", Property%dAttribute%dParameters, %d", 
						Properties[I]->Attributes[J]->Name.ToCString(),
						I, J, Properties[I]->Attributes[J]->Parameters.GetCount());
				}
				else
					fprintf(File, "\t\t{\"%s\", NULL, 0", Properties[I]->Attributes[J]->Name.ToCString());

				fprintf(File, "}%s\n", J < Properties[I]->Attributes.GetCount() - 1 ? "," : "");
			}

			fprintf(File, "\t};\n\n");
		}
	}

	//Creating Enumerator if Property type is enum
	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if(Properties[I]->Type.BaseType == ZEMC_BT_ENUMERATOR)
		{
			fprintf(File,
				"\tstatic ZEEnumParameter Property%dEnumParameters[%d] =\n"
				"\t{\n", I, Properties[I]->Type.Enumurator->Items.GetCount());

			for(ZESize J = 0; J < Properties[I]->Type.Enumurator->Items.GetCount(); J++)
			{
				fprintf(File,
					"\t\t{\"%s\", %d}%s\n",
					Properties[I]->Type.Enumurator->Items[J]->Name.ToCString(),
					Properties[I]->Type.Enumurator->Items[J]->Value,
					J < Properties[I]->Type.Enumurator->Items.GetCount() - 1 ? "," : "");
			}

			fprintf(File,
				"\t};\n\n");

			fprintf(File,
				"\tstatic ZEEnum Property%dEnumerator[1] =\n"
				"\t{\n"
				"\t\t{\"%s\", %#x, Property%dEnumParameters, %d}\n"
				"\t};\n\n",
				I, Properties[I]->Type.Enumurator->Name.ToCString(), Properties[I]->Type.Enumurator->Hash, I, Properties[I]->Type.Enumurator->Items.GetCount());
		}
	}

	//Creating Properties
	if(Properties.GetCount() > 0)
	{
		fprintf(File,
			"\tstatic ZEProperty Properties[%d] =\n\t"
			"{\n", Properties.GetCount());

		fprintf(File, "\t\t//{ID, MemberOf*, PropertyName, Hash, Type, IsGeneratedByMeta, IsContainer, IsStatic, Enum*, EnumParameterCount, Attributes*, AttributeCount}\n");
	}
	
	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		fprintf(File, "\t\t{%d, %s%s, \"%s\", %#x, ZEType(%s, %s, %s, %s, ",
			Properties[I]->ID, 
			IsBuiltInClass ? "NULL" : Properties[I]->MemberOf.ToCString(),
			IsBuiltInClass ? "" : "::Class()",
			Properties[I]->Name.ToCString(), Properties[I]->Hash,
			GetTypeString(Properties[I]->Type.BaseType), GetTypeQualifierString(Properties[I]->Type.TypeQualifier),
			GetTypeString(Properties[I]->Type.SubType), GetTypeQualifierString(Properties[I]->Type.SubTypeQualifier));
			
		if(Properties[I]->Type.BaseType == ZEMC_BT_OBJECT_PTR && Properties[I]->Type.Class->Name != NULL && Properties[I]->Type.Class->Name != "")
			fprintf(File, "%s::Class()), ", Properties[I]->Type.Class->Name.ToCString());
		else if(Properties[I]->Type.BaseType == ZEMC_BT_ARRAY && Properties[I]->Type.SubType == ZEMC_BT_OBJECT_PTR && Properties[I]->Type.Class->Name != NULL && Properties[I]->Type.Class->Name != "")
			fprintf(File, "%s::Class()), ", Properties[I]->Type.Class->Name.ToCString());
		else
			fprintf(File, "NULL), ");	
			
		fprintf(File, "%s, %s, %s, ",
			Properties[I]->HasSetterGetter ? "true" : "false",
			Properties[I]->IsContainer ? "true" : "false",
			Properties[I]->IsStatic ? "true" : "false");

		if(Properties[I]->Type.BaseType == ZEMC_BT_ENUMERATOR)
			fprintf(File, "Property%dEnumerator, %d, ", I, Properties[I]->Type.Enumurator->Items.GetCount());
		else
			fprintf(File, "NULL, 0, ");

		if(Properties[I]->Attributes.GetCount() > 0)
			fprintf(File, "Property%dAttributes, %d}", I, Properties[I]->Attributes.GetCount());
		else
			fprintf(File, "NULL, 0}");

		fprintf(File, "%s\n", I != Properties.GetCount() - 1 ? "," : "");
	}

	if(Properties.GetCount() > 0)
	{
		fprintf(File, 
			"\t};\n\n"
			"\treturn Properties;\n"
			"}\n\n");
	}
	else
	{
		fprintf(File,
			"\treturn NULL;\n"
			"}\n\n");
	}
}

static void CreateGetPropertyCountMethod(FILE* File, const char* CurrentClassName, ZESize PropertyCount)
{
	fprintf(File, 
		"ZESize %sClass::GetPropertyCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		CurrentClassName, PropertyCount);
}



static void CreateGetPropertyOffsetMethod(FILE* File, const char* CurrentClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File,
		"ZESize %sClass::GetPropertyOffset(ZESize PropertyId)\n"
		"{\n", CurrentClassName);

	bool PropertyFound = false;

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if(!Properties[I]->HasSetterGetter)
		{
			PropertyFound = true;
			break;
		}
	}

	if(PropertyFound)
		fprintf(File, "\tswitch(PropertyId)\n\t{\n");

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if(!Properties[I]->HasSetterGetter)
		{
			fprintf(File,
				"\t\tcase %d:\n"
				"\t\t\treturn offsetof(%s, %s);\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString());
		}
	}

	if(PropertyFound)
		fprintf(File, "\t\tdefault:\n\t\t\treturn 0;\n\t}\n\n");

	fprintf(File, "\treturn 0;\n}\n\n");
}

static void CreateGetPropertyOffsetMethod(FILE* File, const char* CurrentClassName)
{
	fprintf(File,
		"ZESize %sClass::GetPropertyOffset(ZEString PropertyName)\n"
		"{\n"
		"\treturn GetPropertyOffset(GetPropertyId(PropertyName));\n"
		"}\n\n", CurrentClassName);
}

static void CreateMethodWrappers(FILE* File, ZEMCClass* ClassData, ZEArray<ZEMCMethod*> Methods)
{
	const char* CurrentClassName = ClassData->Name.ToCString();
	bool HasCreateInstanceMethod = ClassData->HasCreateInstanceMethod;
	bool HasPublicCopyConstructor = ClassData->HasPublicCopyConstructor;
	bool HasPublicConstructor = ClassData->HasScriptBase;
	bool IsAbstract = ClassData->IsAbstract;

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsPure)
			continue;

		//FACTORY METHODS - CONSTRUCTORS
		if(Methods[I]->Name == "CreateInstance")
		{
			fprintf(File, 
				"static %s* %s_%d_CreateInstance()\n"
				"{\n"
				"\treturn %s::CreateInstance();\n"
				"}\n\n", 
				CurrentClassName, CurrentClassName, I, CurrentClassName);

			continue;
		}
		else if((strcmp(Methods[I]->Name.ToCString(), Methods[I]->MemberOf.ToCString()) == 0) && !ClassData->IsBuiltInClass)
		{
			if(IsAbstract || (strcmp(CurrentClassName, Methods[I]->MemberOf.ToCString()) != 0))
				continue;

			if(Methods[I]->Parameters.GetCount() == 0)
			{
				fprintf(File, 
					"static %s* %s_%d_%s()\n"
					"{\n"
					"\treturn new %s();\n"
					"}\n\n", CurrentClassName, CurrentClassName, I, CurrentClassName, CurrentClassName);
				
				continue;
			}
			else
			{
				fprintf(File, "static %s* %s_%d_%s(", CurrentClassName, CurrentClassName, I, CurrentClassName);

				for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
				{
					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
					{
						if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
						{
							fprintf(File, "%sZEArray<%s%s>%s %s%s",
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
								Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
								(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
								Methods[I]->Parameters[J]->Name.ToCString(), 
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
						}
					}
					else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
					{
						fprintf(File, "%s%s%s %s%s",
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(),
							(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
					}
					else
					{
						fprintf(File, "%s%s%s%s %s%s", 
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							ReturnTypeCast(Methods[I]->Parameters[J]->Type.BaseType), 
							Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT ? "*" : "",
							(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
					}
				}

				fprintf(File, "{\n\treturn new %s(", CurrentClassName);

				for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
				{
					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
					{
						fprintf(File, "(%s*)%s%s", 
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							Methods[I]->Parameters[J]->Name.ToCString(),
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else
					{
						fprintf(File, "%s%s", 
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
				}

				fprintf(File, "}\n\n");
			}

			continue;
		}
		else if((strcmp(Methods[I]->Name.ToCString(), Methods[I]->MemberOf.ToCString()) == 0) && ClassData->IsBuiltInClass)
		{
			if(Methods[I]->Parameters.GetCount() == 0)
			{
				fprintf(File, 
					"static void %s_%d_%s(%s* This)\n"
					"{\n"
					"\tnew(This) %s();\n"
					"}\n\n", CurrentClassName, I, CurrentClassName, CurrentClassName, CurrentClassName);

				continue;
			}
			else
			{
				fprintf(File, "static void %s_%d_%s(%s* This, ", CurrentClassName, I, CurrentClassName, CurrentClassName);

				for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
				{
					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
					{
						if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
						{
							fprintf(File, "%sZEArray<%s%s>%s %s%s",
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
								Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
								(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
								Methods[I]->Parameters[J]->Name.ToCString(), 
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
						}
					}
					else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
					{
						fprintf(File, "%s%s%s %s%s",
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(),
							(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
					}
					else
					{
						fprintf(File, "%s%s%s%s %s%s", 
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							ReturnTypeCast(Methods[I]->Parameters[J]->Type.BaseType), 
							Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT ? "*" : "",
							(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
					}
				}

				fprintf(File, "{\n\tnew(This) %s(", CurrentClassName);

				for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
				{
					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
					{
						fprintf(File, "(%s*)%s%s", 
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							Methods[I]->Parameters[J]->Name.ToCString(),
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else
					{
						fprintf(File, "%s%s", 
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
				}

				fprintf(File, "}\n\n");
			}

			continue;
		}

		//OPERATORS
		if(Methods[I]->IsOperator && Methods[I]->Name == "operator=" && !ClassData->HasCreateInstanceMethod)
		{
			fprintf(File, "static void %s_%d_opAssign(%s* This, const void* Arg0)\n"
				"{\n"
				"\tThis->operator=(*(%s*)Arg0);\n"
				"}\n\n", 
				Methods[I]->MemberOf.ToCString(), I, 
				Methods[I]->MemberOf.ToCString(), 
				Methods[I]->MemberOf.ToCString());

			continue;
		}
		else if(Methods[I]->IsOperator && Methods[I]->Name == "operator=" && ClassData->HasCreateInstanceMethod)
		{
			fprintf(File, 
				"static void %s_%d_opAssign(%s* This, const void* Arg0)\n"
				"{\n\n}\n\n", 
				Methods[I]->MemberOf.ToCString(), I, 
				Methods[I]->MemberOf.ToCString());

			continue;
		}
		else if(Methods[I]->IsOperator && Methods[I]->Parameters.GetCount() > 0)
		{
			if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
			{
				if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
				{
					fprintf(File, 
						"static %sZEArray<%s%s>%s %s_%d_%s(%s* This, ",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						Methods[I]->ReturnValue.Class->Name.ToCString(),
						Methods[I]->ReturnValue.SubType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
						(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
						Methods[I]->MemberOf.ToCString(), I, 
						GetOperatorName(Methods[I]->OperatorType), 
						Methods[I]->MemberOf.ToCString());
				}
			}
			else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
			{
				fprintf(File, 
					"static %s%s%s %s_%d_%s(%s* This, ",
					Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->ReturnValue.Enumurator->Name.ToCString(),
					(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
					Methods[I]->MemberOf.ToCString(), I, 
					GetOperatorName(Methods[I]->OperatorType), 
					Methods[I]->MemberOf.ToCString());
			}
			else
			{
				fprintf(File, 
					"static %s%s%s%s %s_%d_%s(%s* This, ", 
					Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
					Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
					(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
					Methods[I]->MemberOf.ToCString(), I, 
					GetOperatorName(Methods[I]->OperatorType), 
					Methods[I]->MemberOf.ToCString());
			}

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
				{
					if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
					{
						fprintf(File, "%sZEArray<%s%s>%s %s%s",
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
							(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
							Methods[I]->Parameters[J]->Name.ToCString(),
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
					}
				}
				else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
				{
					fprintf(File, "%s%s%s %s%s",
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(),
						(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
						Methods[I]->Parameters[J]->Name.ToCString(),
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
				}
				else
				{
					fprintf(File, "%s%s%s%s %s%s", 
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						ReturnTypeCast(Methods[I]->Parameters[J]->Type.BaseType), 
						Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT ? "*" : "",
						(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
						Methods[I]->Parameters[J]->Name.ToCString(),
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
				}
			}

			fprintf(File, "{\n\t%sThis->%s(",
				Methods[I]->ReturnValue.BaseType == ZEMC_BT_VOID ? "" : "return ",
				Methods[I]->Name.ToCString());

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
				{
					fprintf(File, "%s(%s*)%s%s", 
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE ? "*" : "",
						Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
						Methods[I]->Parameters[J]->Name.ToCString(),
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n}\n\n");
				}
				else
				{
					fprintf(File, "%s%s", 
						Methods[I]->Parameters[J]->Name.ToCString(), 
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n}\n\n");
				}
			}

			continue;
		}
		else if (Methods[I]->IsOperator)
		{
			if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
			{
				if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
				{
					fprintf(File, 
						"static %sZEArray<%s%s> %s_%d_%s(%s* This)\n"
						"{\n"
						"\treturn This->%s();\n"
						"}\n\n",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						Methods[I]->ReturnValue.Class->Name.ToCString(),
						Methods[I]->ReturnValue.SubType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT  || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR  ? "*" : "",
						Methods[I]->MemberOf.ToCString(), I, 
						GetOperatorName(Methods[I]->OperatorType), 
						Methods[I]->MemberOf.ToCString(), 
						Methods[I]->Name.ToCString());
				}
			}
			else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
			{
				fprintf(File, 
					"static %s%s %s_%d_%s(%s* This)\n"
					"{\n"
					"\treturn This->%s();\n"
					"}\n\n",
					Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->ReturnValue.Enumurator->Name.ToCString(),
					Methods[I]->MemberOf.ToCString(), I, 
					GetOperatorName(Methods[I]->OperatorType), 
					Methods[I]->MemberOf.ToCString(), 
					Methods[I]->Name.ToCString());
			}
			else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_VOID)
			{
				fprintf(File, 
					"static %s%s%s %s_%d_%s(%s* This)\n"
					"{\n"
					"\tThis->%s();\n"
					"}\n\n",
					Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
					Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
					Methods[I]->MemberOf.ToCString(), I, 
					GetOperatorName(Methods[I]->OperatorType), 
					Methods[I]->MemberOf.ToCString(), 
					Methods[I]->Name.ToCString());
			}
			else
			{
				fprintf(File, 
					"static %s%s%s %s_%d_%s(%s* This)\n"
					"{\n"
					"\treturn This->%s();\n"
					"}\n\n",
					Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
					Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
					Methods[I]->MemberOf.ToCString(), I, 
					GetOperatorName(Methods[I]->OperatorType), 
					Methods[I]->MemberOf.ToCString(), 
					Methods[I]->Name.ToCString());
			}

			continue;
		}

		//STANDART METHODS
		if(Methods[I]->Parameters.GetCount() > 0)
		{
			if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
			{
				if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
				{
					fprintf(File, 
						"static %sZEArray<%s%s>%s %s_%d_%s(%s* This, ",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						Methods[I]->ReturnValue.Class->Name.ToCString(),
						Methods[I]->ReturnValue.SubType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
						(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
						Methods[I]->MemberOf.ToCString(), I, 
						Methods[I]->Name.ToCString(), 
						Methods[I]->MemberOf.ToCString());
				}
			}
			else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
			{
				fprintf(File, 
					"static %s%s%s %s_%d_%s(%s* This, ",
					Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					Methods[I]->ReturnValue.Enumurator->Name.ToCString(),
					(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
					Methods[I]->MemberOf.ToCString(), I, 
					Methods[I]->Name.ToCString(), 
					Methods[I]->MemberOf.ToCString());
			}
			else
			{
				fprintf(File, 
					"static %s%s%s%s %s_%d_%s(%s* This, ", 
					Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
					ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
					Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
					(Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->ReturnValue.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
					Methods[I]->MemberOf.ToCString(), I, 
					Methods[I]->Name.ToCString(), 
					Methods[I]->MemberOf.ToCString());
			}

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
				{
					if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
					{
						fprintf(File, "%sZEArray<%s%s>%s %s%s",
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR ? "*" : "",
							(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
					}
				}
				else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
				{
					fprintf(File, "%s%s%s %s%s",
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(),
						(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
						Methods[I]->Parameters[J]->Name.ToCString(), 
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
				}
				else
				{
					fprintf(File, "%s%s%s%s %s%s", 
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						ReturnTypeCast(Methods[I]->Parameters[J]->Type.BaseType), 
						Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT ? "*" : "",
						(Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE || Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_REFERENCE) && Methods[I]->Parameters[J]->Type.BaseType != ZEMC_BT_OBJECT_PTR ? "&" : "",
						Methods[I]->Parameters[J]->Name.ToCString(), 
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ")\n");
				}
			}

			/*if(Methods[I]->IsVirtual && ClassData->HasPublicConstructor)
			{
				if(Methods[I]->ReturnParameter.Type.Type == ZEMC_BT_NULL)
					fprintf(File, "{\n\t((%sScriptBase*)This)->ScriptTriggered%s(", ClassData->Name.ToCString(), Methods[I]->Name.ToCString());
				else
					fprintf(File, "{\n\treturn ((%sScriptBase*)This)->ScriptTriggered%s(", ClassData->Name.ToCString(), Methods[I]->Name.ToCString());
			}
			else
			{*/
				fprintf(File, "{\n\t%sThis->%s(",
					Methods[I]->ReturnValue.BaseType == ZEMC_BT_VOID ? "" : "return ",
					Methods[I]->Name.ToCString());
			//}

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				if(Methods[I]->IsVirtual && ClassData->HasPublicConstructor)
				{
					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
					{
						fprintf(File, "(%s*)%s%s", 
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							Methods[I]->Parameters[J]->Name.ToCString(),
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n}\n\n");
					}
					else
					{
						fprintf(File, "%s%s", 
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n}\n\n");
					}
				}
				else
				{
					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_CLASS || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT || Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
					{
						fprintf(File, "(%s*)%s%s", 
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							Methods[I]->Parameters[J]->Name.ToCString(),
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n}\n\n");
					}
					else
					{
						fprintf(File, "%s%s", 
							Methods[I]->Parameters[J]->Name.ToCString(), 
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n}\n\n");
					}
				}
			}
		}
		else
		{
			if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
			{
				if(Methods[I]->ReturnValue.SubType != ZEMC_BT_UNDEFINED)
				{
					if (false) //if(Methods[I]->IsVirtual && ClassData->HasPublicConstructor)
					{
						fprintf(File, 
							"static %sZEArray<%s%s> %s_%d_%s(%s* This)\n"
							"{\n"
							"\treturn ((%sScriptBase*)This)->ScriptTriggered%s();\n"
							"}\n\n",
							Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							Methods[I]->ReturnValue.Class->Name.ToCString(),
							Methods[I]->ReturnValue.SubType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT  || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR  ? "*" : "",
							Methods[I]->MemberOf.ToCString(), I, 
							Methods[I]->Name.ToCString(), 
							Methods[I]->MemberOf.ToCString(),
							ClassData->Name.ToCString(),
							Methods[I]->Name.ToCString());
					}
					else
					{
						fprintf(File, 
							"static %sZEArray<%s%s> %s_%d_%s(%s* This)\n"
							"{\n"
							"\treturn This->%s();\n"
							"}\n\n",
							Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
							Methods[I]->ReturnValue.Class->Name.ToCString(),
							Methods[I]->ReturnValue.SubType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT  || Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR  ? "*" : "",
							Methods[I]->MemberOf.ToCString(), I, 
							Methods[I]->Name.ToCString(), 
							Methods[I]->MemberOf.ToCString(), 
							Methods[I]->Name.ToCString());
					}
				}
			}
			else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
			{
				if (false) // if(Methods[I]->IsVirtual && ClassData->HasPublicConstructor)
				{
					fprintf(File, 
						"static %s%s %s_%d_%s(%s* This)\n"
						"{\n"
						"\treturn ((%sScriptBase*)This)->ScriptTriggered%s();\n"
						"}\n\n",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						Methods[I]->ReturnValue.Enumurator->Name.ToCString(),
						Methods[I]->MemberOf.ToCString(), I, 
						Methods[I]->Name.ToCString(), 
						Methods[I]->MemberOf.ToCString(), 
						ClassData->Name.ToCString(),
						Methods[I]->Name.ToCString());
				}
				else
				{
					fprintf(File, 
						"static %s%s %s_%d_%s(%s* This)\n"
						"{\n"
						"\treturn This->%s();\n"
						"}\n\n",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						Methods[I]->ReturnValue.Enumurator->Name.ToCString(),
						Methods[I]->MemberOf.ToCString(), I, 
						Methods[I]->Name.ToCString(), 
						Methods[I]->MemberOf.ToCString(), 
						Methods[I]->Name.ToCString());
				}
			}
			else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_VOID)
			{
				if (false) // if(Methods[I]->IsVirtual && ClassData->HasPublicConstructor)
				{
					fprintf(File, 
						"static %s%s%s %s_%d_%s(%s* This)\n"
						"{\n"
						"\t((%sScriptBase*)This)->ScriptTriggered%s();\n"
						"}\n\n",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
						Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
						Methods[I]->MemberOf.ToCString(), I, 
						Methods[I]->Name.ToCString(), 
						Methods[I]->MemberOf.ToCString(), 
						ClassData->Name.ToCString(),
						Methods[I]->Name.ToCString());
				}
				else
				{
					fprintf(File, 
						"static %s%s%s %s_%d_%s(%s* This)\n"
						"{\n"
						"\tThis->%s();\n"
						"}\n\n",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
						Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
						Methods[I]->MemberOf.ToCString(), I, 
						Methods[I]->Name.ToCString(), 
						Methods[I]->MemberOf.ToCString(), 
						Methods[I]->Name.ToCString());
				}
			}
			else
			{
				if (false) // if(Methods[I]->IsVirtual && ClassData->HasPublicConstructor)
				{
					fprintf(File, 
						"static %s%s%s %s_%d_%s(%s* This)\n"
						"{\n"
						"\treturn ((%sScriptBase*)This)->ScriptTriggered%s();\n"
						"}\n\n",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
						Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
						Methods[I]->MemberOf.ToCString(), I, 
						Methods[I]->Name.ToCString(), 
						Methods[I]->MemberOf.ToCString(), 
						ClassData->Name.ToCString(),
						Methods[I]->Name.ToCString());
				}
				else
				{
					fprintf(File, 
						"static %s%s%s %s_%d_%s(%s* This)\n"
						"{\n"
						"\treturn This->%s();\n"
						"}\n\n",
						Methods[I]->ReturnValue.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "const " : "",
						ReturnTypeCast(Methods[I]->ReturnValue.BaseType),
						Methods[I]->ReturnValue.BaseType == ZEMC_BT_CLASS || Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT ? "*" : "",
						Methods[I]->MemberOf.ToCString(), I, 
						Methods[I]->Name.ToCString(), 
						Methods[I]->MemberOf.ToCString(), 
						Methods[I]->Name.ToCString());
				}
			}
		}
	}

	//SCRIPT BASE CLASS FACTORY METHOD
	if (false)// if(!ClassData->IsBuiltInClass && HasPublicConstructor && !IsAbstract)
	{
		fprintf(File,
			"static %sScriptBase* %s_CreateScriptBaseClassInstance()\n"
			"{\n"
			"\treturn new %sScriptBase();\n"
			"}\n\n", CurrentClassName, CurrentClassName, CurrentClassName);

		fprintf(File,
			"static void %s_BindScriptBaseClassInstance(%sScriptBase* This, void* ScriptBaseClassInstance)\n"
			"{\n"
			"\tThis->ScriptObject = *reinterpret_cast<void**>(ScriptBaseClassInstance);\n"
			"\t//This->ScriptEngine = ZEAngelScriptEngine::GetInstance();\n\n", CurrentClassName, CurrentClassName);

		for(ZESize I = 0; I < Methods.GetCount(); I++)
		{
			if(!Methods[I]->IsVirtual)
				continue;

			fprintf(File, "\tThis->%sScriptFunction%d = new ZEScriptFunction();\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I);
			fprintf(File, "\tThis->%sScriptFunction%d->Name = \"%s\";\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I, Methods[I]->Name.ToCString());

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
				fprintf(File, "\tThis->%sScriptFunction%d->Parameters.Add();\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I);

			fprintf(File, "\tThis->%sScriptFunction%d->Declaration = NULL;\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I);
			fprintf(File, "\tThis->%sScriptFunction%d->ReturnType = %s;\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I, GetTypeString(Methods[I]->ReturnValue.BaseType));
			fprintf(File, "\tThis->%sScriptFunction%d->ScriptObject = This->ScriptObject;\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I);
			fprintf(File, "\tThis->ScriptEngine->BindScriptFunctionToNativeFunction(This->%sScriptFunction%d);\n\n", strcmp(Methods[I]->Name.ToCString(), "operator=") == 0 ? "opAssign" : Methods[I]->Name.ToCString(), I);
		}

		fprintf(File, "}\n\n");
	}

	if(!ClassData->HasPublicCopyConstructor)
	{
		fprintf(File, "static void %s_opAssign(%s* This, const void* Arg0)\n"
			"{\n"
			"\tThis->operator=(*(%s*)Arg0);\n"
			"}\n\n", 
			CurrentClassName, 
			CurrentClassName, 
			CurrentClassName);
	}
}

static void CreateGetMethodsMethod(FILE* File, ZEMCClass* ClassData, ZEArray<ZEMCMethod*> Methods)
{
	const char* ClassName = ClassData->Name;
	bool IsBuiltInClass = ClassData->IsBuiltInClass;
	bool IsAbstract = ClassData->IsAbstract;

	fprintf(File, 
		"const ZEMethod* %sClass::GetMethods()\n"
		"{\n", ClassName);

	//Crating method attributes
	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->Attributes.GetCount() > 0)
		{
			for(ZESize J = 0; J < Methods[I]->Attributes.GetCount(); J++)
			{
				if(Methods[I]->Attributes[J]->Parameters.GetCount() > 0)
				{
					fprintf(File, "\tstatic const char* Method%dAttribute%dParameters[%d] = {", I, J, Methods[I]->Attributes[J]->Parameters.GetCount());

					for(ZESize K = 0; K < Methods[I]->Attributes[J]->Parameters.GetCount(); K++)
					{
						fprintf(File, "\"%s\"%s", Methods[I]->Attributes[J]->Parameters[K].ToCString(), K != Methods[I]->Attributes[J]->Parameters.GetCount() - 1 ? ", " : "");
					}

					fprintf(File, "};\n\n");
				}
			}

			fprintf(File, 
				"\tstatic ZEMetaAttribute Method%dAttributes[%d] =\n"
				"\t{\n", I, Methods[I]->Attributes.GetCount());

			for(ZESize J = 0; J < Methods[I]->Attributes.GetCount(); J++)
			{
				if(Methods[I]->Attributes[J]->Parameters.GetCount() > 0)
				{
					fprintf(File, 
						"\t\t{\"%s\", Method%dAttribute%dParameters, %d", 
						Methods[I]->Attributes[J]->Name.ToCString(),
						I, J, Methods[I]->Attributes[J]->Parameters.GetCount());
				}
				else
					fprintf(File, "\t\t{\"%s\", NULL, 0", Methods[I]->Attributes[J]->Name.ToCString());

				fprintf(File, "}%s\n", J < Methods[I]->Attributes.GetCount() - 1 ? "," : "");
			}

			fprintf(File, "\t};\n\n");
		}
	}

	//Creating Enumerator if MethodParameter type is enum
	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
		{
			if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
			{
				fprintf(File,
					"\tstatic ZEEnumParameter Method%dParameter%dEnumParameters[%d] =\n"
					"\t{\n", I, J, Methods[I]->Parameters[J]->Type.Enumurator->Items.GetCount());

				for(ZESize K = 0; K < Methods[I]->Parameters[J]->Type.Enumurator->Items.GetCount(); K++)
				{
					fprintf(File,
						"\t\t{\"%s\", %d}%s\n",
						Methods[I]->Parameters[J]->Type.Enumurator->Items[K]->Name.ToCString(),
						Methods[I]->Parameters[J]->Type.Enumurator->Items[K]->Value,
						K < Methods[I]->Parameters[J]->Type.Enumurator->Items.GetCount() - 1 ? "," : "");
				}

				fprintf(File,
					"\t};\n\n");

				fprintf(File,
					"\tstatic ZEEnum Method%dParameter%dEnumerator[1] =\n"
					"\t{\n"
					"\t\t{\"%s\", %#x, Method%dParameter%dEnumParameters, %d}\n"
					"\t};\n\n",
					I, J, 
					Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), 
					Methods[I]->Parameters[J]->Type.Enumurator->Hash, 
					I, J,
					Methods[I]->Parameters[J]->Type.Enumurator->Items.GetCount());
			}
		}
	}

	//Creating method parameters
	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsOperator && Methods[I]->Name == "operator=")
			continue;

		if(Methods[I]->Parameters.GetCount() > 0)
		{
			fprintf(File,
				"\tstatic ZEMethodParameter Method%dParameters[%d] ="
				"\n\t{\n", I, Methods[I]->Parameters.GetCount());

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				fprintf(File,
					"\t\t{\"%s\", ZEType(%s, %s, %s, %s, ",
					Methods[I]->Parameters[J]->Name.ToCString(),
					GetTypeString(Methods[I]->Parameters[J]->Type.BaseType), GetTypeQualifierString(Methods[I]->Parameters[J]->Type.TypeQualifier),
					GetTypeString(Methods[I]->Parameters[J]->Type.SubType), GetTypeQualifierString(Methods[I]->Parameters[J]->Type.SubTypeQualifier));

				if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR && Methods[I]->Parameters[J]->Type.Class->Name != NULL && Methods[I]->Parameters[J]->Type.Class->Name != "")
					fprintf(File, "%s::Class()), ", Methods[I]->Parameters[J]->Type.Class->Name.ToCString());
				else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY && Methods[I]->Parameters[J]->Type.SubType == ZEMC_BT_OBJECT_PTR && Methods[I]->Parameters[J]->Type.Class->Name != NULL && Methods[I]->Parameters[J]->Type.Class->Name != "")
					fprintf(File, "%s::Class()), ", Methods[I]->Parameters[J]->Type.Class->Name.ToCString());
				else
					fprintf(File, "NULL), ");


				if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
					fprintf(File, "Method%dParameter%dEnumerator, %d}",	I, J, Methods[I]->Parameters[J]->Type.Enumurator->Items.GetCount());
				else
					fprintf(File, "NULL, 0}");

				fprintf(File, "%s\n", J != Methods[I]->Parameters.GetCount() - 1 ? "," : "");
			}

			fprintf(File, "\t};\n\n");
		}
	}

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
		{
			fprintf(File,
				"\tstatic ZEEnumParameter Method%dReturnValueEnumParameters[%d] =\n"
				"\t{\n", I, Methods[I]->ReturnValue.Enumurator->Items.GetCount());

			for(ZESize J = 0; J < Methods[I]->ReturnValue.Enumurator->Items.GetCount(); J++)
			{
				fprintf(File,
					"\t\t{\"%s\", %d}%s\n",
					Methods[I]->ReturnValue.Enumurator->Items[J]->Name.ToCString(),
					Methods[I]->ReturnValue.Enumurator->Items[J]->Value,
					J < Methods[I]->ReturnValue.Enumurator->Items.GetCount() - 1 ? "," : "");
			}

			fprintf(File,
				"\t};\n\n");

			fprintf(File,
				"\tstatic ZEEnum Method%dReturnValueEnum[1] =\n"
				"\t{\n"
				"\t\t{\"%s\", %#x, Method%dReturnValueEnumParameters, %d}\n"
				"\t};\n\n",
				I, Methods[I]->ReturnValue.Enumurator->Name.ToCString(), 
				Methods[I]->ReturnValue.Enumurator->Hash, 
				I, Methods[I]->ReturnValue.Enumurator->Items.GetCount());
		}
	}

	ZEString ClassType = "ZE_TT_OBJECT_PTR";

	if(!ClassData->HasPublicCopyConstructor)
	{
		if(ClassData->IsBuiltInClass)
		{
			if(ClassData->Name == "ZEVector2")
				ClassType = "ZE_TT_VECTOR2";
			if(ClassData->Name == "ZEVector3")
				ClassType = "ZE_TT_VECTOR3";
			if(ClassData->Name == "ZEVector4")
				ClassType = "ZE_TT_VECTOR4";
			if(ClassData->Name == "ZEMatrix3x3")
				ClassType = "ZE_TT_MATRIX3X3";
			if(ClassData->Name == "ZEMatrix4x4")
				ClassType = "ZE_TT_MATRIX4X4";
			if(ClassData->Name == "ZEQuaternion")
				ClassType = "ZE_TT_QUATERNION";
			if(ClassData->Name == "ZEString")
				ClassType = "ZE_TT_STRING";
		}

		fprintf(File,
			"\tstatic ZEMethodParameter AssignOperatorParameter[1] =\n"
			"\t{\n"
			"\t\t{\"Arg0\", ZEType(%s, ZE_TQ_CONST_REFERENCE, ZE_TT_UNDEFINED, ZE_TQ_VALUE), NULL, 0}\n"
			"\t};\n\n", ClassType.ToCString());
	}

	if(Methods.GetCount() > 0)
	{
		if(!ClassData->HasPublicCopyConstructor && !ClassData->HasCreateInstanceMethod)
		{
			if(ClassData->HasPublicConstructor && !ClassData->IsAbstract)
			{
				fprintf(File,
					"\tstatic ZEMethod Methods[%d] =\n\t"
					"{\n", Methods.GetCount() + 3); // +1 for assign operator, +1 for script instance factory, +1 for bind script instance function
			}
			else
			{
				fprintf(File,
					"\tstatic ZEMethod Methods[%d] =\n\t"
					"{\n", Methods.GetCount() + 1); // 1 for assign operator
			}
			
		}
		else
		{
			if(ClassData->HasPublicConstructor && !ClassData->IsAbstract)
			{
				fprintf(File,
					"\tstatic ZEMethod Methods[%d] =\n\t"
					"{\n", Methods.GetCount() + 2); // +1 for script instance factory, +1 for bind script instance function
			}
			else
			{
				fprintf(File,
					"\tstatic ZEMethod Methods[%d] =\n\t"
					"{\n", Methods.GetCount());
			}
		}

		fprintf(File, "\t\t//{ID, MemberOf, MethodPtr*, MethodName, Hash, IsEvent, IsVirtual, IsStatic, IsOperator, EnumOperatorType, ReturnType, EnumReturnType*, EnumReturnParameterCount, Parameters*, ParameterCount, Attributes*, AttributeCount}\n");
	}
	
	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(ClassData->IsBuiltInClass)
			fprintf(File,"\t\t{%d, %s%s, ", Methods[I]->ID, Methods[I]->MemberOf.ToCString(), "Class::Class()");
		else
			fprintf(File,"\t\t{%d, %s%s, ", Methods[I]->ID, Methods[I]->MemberOf.ToCString(), "::Class()");

		if ((Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY && Methods[I]->ReturnValue.SubType == ZEMC_BT_UNDEFINED))
			fprintf(File, "NULL, ");
		else if(Methods[I]->IsPure)
			fprintf(File, "NULL, ");
		else if(Methods[I]->Name.Equals(Methods[I]->MemberOf) && IsAbstract)
			fprintf(File, "NULL, ");
		else if((Methods[I]->Name.Equals(Methods[I]->MemberOf) && strcmp(Methods[I]->Name.ToCString(), ClassName) != 0))
			fprintf(File, "NULL, ");
		else if(Methods[I]->IsOperator)
			fprintf(File, "&%s_%d_%s, ", Methods[I]->MemberOf.ToCString(), I, GetOperatorName(Methods[I]->OperatorType));
		else if(strcmp(Methods[I]->Name.ToCString(), "CreateInstance") == 0)
			fprintf(File, "&%s_%d_CreateInstance, ", ClassName, I);
		else
			fprintf(File, "&%s_%d_%s, ", Methods[I]->MemberOf.ToCString(), I, Methods[I]->Name.ToCString());

		fprintf(File,
			"\"%s\", %#x, %s, %s, %s, %s, %s, ZEType(%s, %s, %s, %s, ",
			Methods[I]->Name.ToCString(), Methods[I]->Hash,
			Methods[I]->IsEvent ? "true" : "false",
			Methods[I]->IsVirtual ? "true" : "false",
			Methods[I]->IsStatic ? "true" : "false",
			Methods[I]->IsOperator ? "true" : "false",
			Methods[I]->IsOperator ? GetOperatorType(Methods[I]->OperatorType) : "ZE_MOT_UNDEFINED",
			GetTypeString(Methods[I]->ReturnValue.BaseType), GetTypeQualifierString(Methods[I]->ReturnValue.TypeQualifier),
			GetTypeString(Methods[I]->ReturnValue.SubType), GetTypeQualifierString(Methods[I]->ReturnValue.SubTypeQualifier));

		if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_OBJECT_PTR && Methods[I]->ReturnValue.Class->Name != NULL && Methods[I]->ReturnValue.Class->Name != "")
			fprintf(File, "%s::Class()), ", Methods[I]->ReturnValue.Class->Name.ToCString());
		else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY && Methods[I]->ReturnValue.SubType == ZEMC_BT_OBJECT_PTR && Methods[I]->ReturnValue.Class->Name != NULL && Methods[I]->ReturnValue.Class->Name != "")
			fprintf(File, "%s::Class()), ", Methods[I]->ReturnValue.Class->Name.ToCString());
		else
			fprintf(File, "NULL), ");

		if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
			fprintf(File, "Method%dReturnValueEnum, %d, ", I, Methods[I]->ReturnValue.Enumurator->Items.GetCount());
		else
			fprintf(File, "NULL, 0, ");

		if(Methods[I]->Parameters.GetCount() > 0)
		{
			if(Methods[I]->IsOperator && Methods[I]->Name == "operator=")
				fprintf(File, "NULL, 0, ");
			else
				fprintf(File, "Method%dParameters, %d, ", I, Methods[I]->Parameters.GetCount());
		}
		else
			fprintf(File, "NULL, 0, ");

		if(Methods[I]->Attributes.GetCount() > 0)
			fprintf(File, "Method%dAttributes, %d", I, Methods[I]->Attributes.GetCount());
		else
			fprintf(File, "NULL, 0");

		if((I == Methods.GetCount() - 1) && (!ClassData->HasPublicCopyConstructor))
			fprintf(File, "},\n");
		else if((I == Methods.GetCount() - 1) && (ClassData->HasPublicCopyConstructor) && !ClassData->IsBuiltInClass)
			fprintf(File, "},\n");
		else if((I == Methods.GetCount() - 1) && (ClassData->HasPublicCopyConstructor) && ClassData->IsBuiltInClass)
			fprintf(File, "}\n");
		else
			fprintf(File, "},\n");

	}

	if (false) //if(!ClassData->HasPublicCopyConstructor)
	{
		ZEString OperatorHash = "operator=";
		fprintf(File, "\t\t{%d, %s%s::Class(), &%s_opAssign, \"operator=\", %#x, false, false, false, true, ZE_MOT_EQUAL, ZEType(%s, ZE_TQ_REFERENCE, ZE_TT_UNDEFINED, ZE_TQ_VALUE, %s%s::Class()), NULL, 0, AssignOperatorParameter, 1, NULL, 0}%s\n",
			Methods.GetCount(), ClassName, ClassData->IsBuiltInClass ? "Class" : "", ClassName, OperatorHash.Hash(), ClassType.ToCString(), ClassName, ClassData->IsBuiltInClass ? "Class" : "", ClassData->IsBuiltInClass ? "" : ",");
	}

	if (false)// if(!ClassData->IsBuiltInClass && ClassData->HasPublicConstructor && !ClassData->IsAbstract)
	{
		ZEString ScriptClassWrapperHash = ClassName;
		ScriptClassWrapperHash.Append("_CreateScriptBaseClassInstance");

		fprintf(File, "\t\t{%d, %s::Class(), &%s_CreateScriptBaseClassInstance, \"CreateScriptBaseClassInstance\", %#x, false, false, false, true, ZE_MOT_UNDEFINED, ZEType(ZE_TT_NULL, ZE_TQ_REFERENCE, ZE_TT_UNDEFINED, ZE_TQ_VALUE, NULL), NULL, 0, NULL, 0, NULL, 0},\n",
			ClassData->HasPublicCopyConstructor ? Methods.GetCount() : Methods.GetCount() + 1, ClassName, ClassName, ScriptClassWrapperHash.Hash());

		ScriptClassWrapperHash = ClassName;
		ScriptClassWrapperHash.Append("_BindScriptBaseClassInstance");

		fprintf(File, "\t\t{%d, %s::Class(), &%s_BindScriptBaseClassInstance, \"BindScriptBaseClassInstance\", %#x, false, false, false, true, ZE_MOT_UNDEFINED, ZEType(ZE_TT_NULL, ZE_TQ_REFERENCE, ZE_TT_UNDEFINED, ZE_TQ_VALUE, NULL), NULL, 0, NULL, 0, NULL, 0}\n",
			ClassData->HasPublicCopyConstructor ? Methods.GetCount() + 1 : Methods.GetCount() + 2, ClassName, ClassName, ScriptClassWrapperHash.Hash());
	}

	if(Methods.GetCount() > 0)
	{
		fprintf(File, 
			"\t};\n\n"
			"\treturn Methods;\n"
			"}\n\n");
	}
	else
	{
		fprintf(File, 
			"\n"
			"\treturn NULL;\n"
			"}\n\n");
	}
}

static void CreateGetMethodCountMethod(FILE* File, const char* ClassName, ZESize MethodCount)
{
	fprintf(File, 
		"ZESize %sClass::GetMethodCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		ClassName, MethodCount);
}

static void CreateGetPropertyIdMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File,
		"ZESize %sClass::GetPropertyId(ZEString PropertyName)\n"
		"{\n"
		"\tstruct ZESortedPropertyData\n"
		"\t{\n"
		"\t\tZESize\t\tHash;\n"
		"\t\tZESize\t\t\tID;\n"
		"\t\tconst char*\t\tPropertyName;\n"
		"\t};\n\n", ClassName);

	if(Properties.GetCount() > 0)
	{
		fprintf(File,
			"\tstatic ZESortedPropertyData SortedProperties[%d] = \n"
			"\t{\n"
			"\t\t//{Hash, ID, PropertyName}\n", Properties.GetCount());
	}

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		fprintf(File, 
			"\t\t{%#x, %d, \"%s\"}%s\n",
			Properties[I]->Hash, Properties[I]->ID, Properties[I]->Name.ToCString(),
			I < Properties.GetCount() - 1 ? "," : "");
	}

	if (Properties.GetCount() > 0)
		fprintf(File, "\t};\n\n");

	if(Properties.GetCount() > 0)
	{
		fprintf(File,
			"\tZESize Hash = PropertyName.Hash();\n\n"
			"\tZESize LeftmostIndex = 0, RightmostIndex = %d, MiddleIndex;\n\n"
			"\t//Binary Search Algorithm\n"
			"\twhile(RightmostIndex >= LeftmostIndex)\n"
			"\t{\n"
			"\t\tMiddleIndex = (LeftmostIndex + RightmostIndex) / 2;\n"
			"\t\tif(SortedProperties[MiddleIndex].Hash < Hash)\n"
			"\t\t\tLeftmostIndex  = MiddleIndex + 1;\n"
			"\t\telse if(SortedProperties[MiddleIndex].Hash > Hash)\n"
			"\t\t\tRightmostIndex = MiddleIndex - 1;\n"
			"\t\telse\n"
			"\t\t\tbreak;\n"
			"\t}\n\n"
			"\tif(PropertyName == SortedProperties[MiddleIndex].PropertyName)\n"
			"\t\treturn SortedProperties[MiddleIndex].ID;\n"
			"\treturn -1;\n", Properties.GetCount() - 1);
	}
	else
		fprintf(File, "\treturn -1;\n");

	fprintf(File, "}\n\n");

}

static void CreateGetMethodIdMethod(FILE* File, const char* ClassName, ZEArray<ZEMCMethod*> Methods)
{
	fprintf(File,
		"ZESize %sClass::GetMethodId(ZEString MethodName, ZESize OverloadIndex)\n"
		"{\n"
		"\tstruct ZESortedMethodData\n"
		"\t{\n"
		"\t\tZEUInt32\t\tHash;\n"
		"\t\tZESize\t\t\tID;\n"
		"\t\tconst char*\t\tMethodName;\n"
		"\t};\n\n", ClassName);

	if(Methods.GetCount() > 0)
	{
		fprintf(File,
			"\tstatic ZESortedMethodData SortedMethods[%d] = \n"
			"\t{\n"
			"\t\t//{Hash, ID, MethodName}\n", 
			Methods.GetCount());
	}

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		fprintf(File, 
			"\t\t{%#x, %d, \"%s\"}%s\n",
			Methods[I]->Hash, Methods[I]->ID, Methods[I]->Name.ToCString(),
			I < Methods.GetCount() - 1 ? "," : "");
	}

	if (Methods.GetCount() > 0)
		fprintf(File, "\t};\n\n");

	if(Methods.GetCount() > 0)
	{
		fprintf(File,
			"\tZESize Hash = MethodName.Hash();\n\n"
			"\tZESize LeftmostIndex = 0, RightmostIndex = %d, MiddleIndex;\n\n"
			"\t//Binary Search Algorithm\n"
			"\twhile(RightmostIndex >= LeftmostIndex)\n"
			"\t{\n"
			"\t\tMiddleIndex = (LeftmostIndex + RightmostIndex) / 2;\n"
			"\t\tif(SortedMethods[MiddleIndex].Hash < Hash)\n"
			"\t\t\tLeftmostIndex  = MiddleIndex + 1;\n"
			"\t\telse if(SortedMethods[MiddleIndex].Hash > Hash)\n"
			"\t\t\tRightmostIndex = MiddleIndex - 1;\n"
			"\t\telse\n"
			"\t\t\tbreak;\n"
			"\t}\n\n"
			"\tif(MethodName == SortedMethods[MiddleIndex].MethodName)\n"
			"\t\treturn SortedMethods[MiddleIndex].ID;\n\n"
			"\treturn -1;\n", Methods.GetCount() - 1);
	}
	else
		fprintf(File, "\treturn -1;\n");

	fprintf(File, "}\n\n");

}

static void CreateSetPropertyMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File,
		"bool %sClass::SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	if(Properties.GetCount() > 0)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");
	}

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		ZEString VariantType = ReturnVariantType(Properties[I]->Type.BaseType);

		if(Properties[I]->IsStatic)
		{
			if(Properties[I]->Type.BaseType == ZEMC_BT_CLASS || Properties[I]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
			{
				if(Properties[I]->IsContainer)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t%s::%s = (%s*)Value.GetObjectPtr();\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.Class->Name.ToCString());
				}
			}
			else if(Properties[I]->Type.BaseType == ZEMC_BT_ENUMERATOR)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\t%s::%s = (%s)Value.Get%s();\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(),
					Properties[I]->Type.BaseType == ZEMC_BT_ENUMERATOR ? Properties[I]->Type.Enumurator->Name.ToCString() : "",
					VariantType.ToCString());
			}
			else if(Properties[I]->Type.BaseType == ZEMC_BT_ARRAY)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\t%s::%s = Value.Get%s();\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(),
					VariantType.ToCString());
			}
		}
		else
		{
			if(Properties[I]->Type.BaseType == ZEMC_BT_CLASS || Properties[I]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
			{
				if(Properties[I]->IsContainer)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else if(Properties[I]->HasSetterGetter)
				{
					if(Properties[I]->Setter != NULL && Properties[I]->Setter != "")
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\t((%s*)Object)->Set%s((%s*)Value.GetObjectPtr());\n"
							"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString(), Properties[I]->Type.Class->Name.ToCString());
					}
					else
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\treturn false;\n", I);
					}
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s = (%s*)Value.GetObjectPtr();\n"
						"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString(), Properties[I]->Type.Class->Name.ToCString());
				}
			}
			else if(Properties[I]->Type.BaseType == ZEMC_BT_ENUMERATOR)
			{
				if(Properties[I]->HasSetterGetter)
				{
					if(Properties[I]->Setter != NULL && Properties[I]->Setter != "")
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\t((%s*)Object)->Set%s((%s)Value.Get%s());\n"
							"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString(),
							Properties[I]->Type.BaseType == ZEMC_BT_ENUMERATOR ? Properties[I]->Type.Enumurator->Name.ToCString() : "",
							VariantType.ToCString());
					}
					else
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\treturn false;\n", I);
					}
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s = (%s)Value.Get%s();\n"
						"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString(),
						Properties[I]->Type.BaseType == ZEMC_BT_ENUMERATOR ? Properties[I]->Type.Enumurator->Name.ToCString() : "",
						VariantType.ToCString());
				}
			}
			else if(Properties[I]->Type.BaseType == ZEMC_BT_ARRAY)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				if(Properties[I]->HasSetterGetter)
				{
					if(Properties[I]->Setter != NULL && Properties[I]->Setter != "")
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\t((%s*)Object)->Set%s(Value.Get%s%s());\n"
							"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString(),
							VariantType.ToCString(),
							Properties[I]->Type.TypeQualifier == ZE_TQ_VALUE ? "" : Properties[I]->Type.TypeQualifier == ZE_TQ_REFERENCE ? "Ref" : "ConstRef");
					}
					else
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\treturn false;\n", I);
					}
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s = Value.Get%s%s();\n"
						"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString(),
						VariantType.ToCString(),
						Properties[I]->Type.TypeQualifier == ZE_TQ_VALUE ? "" : Properties[I]->Type.TypeQualifier == ZE_TQ_REFERENCE ? "Ref" : "ConstRef");
				}
			}
		}
	}

	if(Properties.GetCount() > 0)
	{
		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateSetPropertyMethod(FILE* File, const char* ClassName)
{
	fprintf(File,
		"bool %sClass::SetProperty(ZEObject* Object, ZEString PropertyName, const ZEVariant& Value)\n"
		"{\n"
		"\treturn SetProperty(Object, GetPropertyId(PropertyName), Value);\n"
		"}\n\n", ClassName);
}

static void CreateGetPropertyMethod(FILE* File,const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File,
		"bool %sClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	if(Properties.GetCount() > 0)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");
	}

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		ZEString VariantType = ReturnVariantType(Properties[I]->Type.BaseType);

		if(Properties[I]->IsStatic)
		{
			if(Properties[I]->Type.BaseType == ZEMC_BT_CLASS || Properties[I]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
			{
				if(Properties[I]->IsContainer)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\tValue.SetObjectPtr(%s::%s);\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
				}
			}
			else if(Properties[I]->Type.BaseType == ZEMC_BT_ARRAY)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\tValue.Set%s(%s::%s);\n"
					"\t\t\treturn true;\n", I, VariantType.ToCString(), ClassName, Properties[I]->Name.ToCString());
			}
		}
		else
		{
			if(Properties[I]->Type.BaseType == ZEMC_BT_CLASS || Properties[I]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
			{
				if(Properties[I]->IsContainer)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					if(Properties[I]->HasSetterGetter)
					{
						if(Properties[I]->Getter != NULL && Properties[I]->Getter != "")
						{
							fprintf(File,
								"\t\tcase %d:\n"
								"\t\t\tValue.SetObjectPtr((ZEObject*)(((%s*)Object)->Get%s()));\n"
								"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString());
						}
						else
						{
							fprintf(File,
								"\t\tcase %d:\n"
								"\t\t\treturn false;\n", I);
						}
					}
					else
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\tValue.SetObjectPtr((ZEObject*)(((%s*)Object)->%s));\n"
							"\t\t\treturn true;\n", I, Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString());
					}
				}
			}
			else if(Properties[I]->Type.BaseType == ZEMC_BT_ARRAY)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				if(Properties[I]->HasSetterGetter)
				{
					if(Properties[I]->Getter != NULL && Properties[I]->Getter != "")
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\tValue.Set%s(((%s*)Object)->Get%s());\n"
							"\t\t\treturn true;\n", I, VariantType.ToCString(), Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString());
					}
					else
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\treturn false;\n", I);
					}
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\tValue.Set%s(((%s*)Object)->%s);\n"
						"\t\t\treturn true;\n", I, VariantType.ToCString(), Properties[I]->MemberOf.ToCString(), Properties[I]->Name.ToCString());
				}
			}
		}
	}

	if(Properties.GetCount() > 0)
	{
		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateGetPropertyMethod(FILE* File, const char* ClassName)
{
	fprintf(File,
		"bool %sClass::GetProperty(ZEObject* Object, ZEString PropertyName, ZEVariant& Value)\n"
		"{\n"
		"\treturn GetProperty(Object, GetPropertyId(PropertyName), Value);\n"
		"}\n\n", ClassName);
}

static void CreateSetPropertyItemMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File, 
		"bool %sClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	bool IsArrayFound = false;

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if((Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED) || Properties[I]->IsContainer)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			IsArrayFound = true;
			break;
		}
	}

	if(IsArrayFound)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for(ZESize I = 0; I < Properties.GetCount(); I++)
		{
			if(Properties[I]->HasSetterGetter)	
				continue;

			if(Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
			{
				if(Properties[I]->Type.SubType != ZE_TT_OBJECT_PTR)
				{
					ZEString VariantType = ReturnVariantType(Properties[I]->Type.SubType);

					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s[Index] = Value.Get%s%s();\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), VariantType.ToCString(),
						Properties[I]->Type.SubTypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "");
				}
				else
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s[Index] = ((%s*)Value.GetObjectPtr());\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.Class->Name.ToCString());
				}
			}
			else if(Properties[I]->IsContainer)
			{
				if(Properties[I]->Type.SubType != ZE_TT_OBJECT_PTR)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s->SetItem(Index, ((%s*)Value.GetObjectPtr()));\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.Class->Name.ToCString());
				}
			}
		}

		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateSetPropertyItemMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::SetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn SetPropertyItem(Object, GetPropertyId(PropertyName), Index, Value);\n"
		"}\n\n", ClassName);
}

static void CreateGetPropertyItemMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	bool IsArrayFound = false;

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if((Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED) || Properties[I]->IsContainer)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			IsArrayFound = true;
			break;
		}
	}

	if(IsArrayFound)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for(ZESize I = 0; I < Properties.GetCount(); I++)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			if(Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
			{
				if(Properties[I]->Type.SubTypeQualifier == ZE_TQ_REFERENCE)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\tValue = ((%s*)((%s*)Object)->%s[Index]);\n"
						"\t\t\treturn true;\n", I, Properties[I]->Type.Class->Name.ToCString(), ClassName, Properties[I]->Name.ToCString());
				}
				else
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\tValue = ((%s*)Object)->%s[Index];\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
				}
			}
			else if(Properties[I]->IsContainer)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s->GetItem(Index, Value.GetObjectPtrRef());\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
			}
		}

		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateGetPropertyItemMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn GetPropertyItem(Object, GetPropertyId(PropertyName), Index, Value);\n"
		"}\n\n", ClassName);
}

static void CreateAddItemToPropertyMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	bool IsArrayFound = false;

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if((Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED) || Properties[I]->IsContainer)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			IsArrayFound = true;
			break;
		}
	}

	if(IsArrayFound)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for(ZESize I = 0; I < Properties.GetCount(); I++)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			if(Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
			{
				if(Properties[I]->Type.SubType != ZE_TT_OBJECT_PTR)
				{
					ZEString VariantType = ReturnVariantType(Properties[I]->Type.SubType);

					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s.Add(Value.Get%s%s());\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), VariantType.ToCString(),
						Properties[I]->Type.SubTypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "");
				}
				else
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s.Add(((%s*)Value.GetObjectPtr()));\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.Class->Name.ToCString());
				}
			}
		}

		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateAddItemToPropertyMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZEVariant& Value)\n"
		"{\n"
		"\treturn AddItemToProperty(Object, GetPropertyId(PropertyName), Value);\n"
		"}\n\n", ClassName);
}

static void CreateAddItemToPropertyWithIndexMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	bool IsContainerFound = false;

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if(Properties[I]->IsContainer)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			IsContainerFound = true;
			break;
		}
	}

	if(IsContainerFound)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for(ZESize I = 0; I < Properties.GetCount(); I++)
		{
			if(Properties[I]->HasSetterGetter)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else if(Properties[I]->IsContainer)
			{
				if(Properties[I]->Type.SubType != ZE_TT_CLASS)
					continue;

				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s->Insert(Index, ((%s*)Value.GetObjectPtr()));\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.Class->Name.ToCString());
			}
		}

		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateAddItemToPropertyWithIndexMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn AddItemToProperty(Object, GetPropertyId(PropertyName), Index, Value);\n"
		"}\n\n", ClassName);
}

static void CreateRemoveItemFromPropertyWithIndexMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File, 
		"bool %sClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	bool IsArrayFound = false;

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if((Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED) || Properties[I]->IsContainer)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			IsArrayFound = true;
			break;
		}
	}

	if(IsArrayFound)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for(ZESize I = 0; I < Properties.GetCount(); I++)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			if(Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s.DeleteAt(Index);\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
			}
			else if(Properties[I]->IsContainer)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s->Remove(Index);\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
			}
		}

		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateRemoveItemFromPropertyWithIndexMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::RemoveItemFromProperty(ZEObject* Object, ZEString PropertyName, ZESize Index)\n"
		"{\n"
		"\treturn RemoveItemFromProperty(Object, GetPropertyId(PropertyName), Index);\n"
		"}\n\n", ClassName);
}

static void CreateGetPropertyItemCountMethod(FILE* File, const char* ClassName, ZEArray<ZEMCProperty*> Properties)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count)\n"
		"{\n"
		"\tconst ZEProperty* Properties = GetProperties();\n"
		"\tif(Properties[PropertyId].Name == \"\" || Properties[PropertyId].Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	bool IsArrayFound = false;

	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		if((Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED) || Properties[I]->IsContainer)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			IsArrayFound = true;
			break;
		}
	}

	if(IsArrayFound)
	{
		fprintf(File,
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for(ZESize I = 0; I < Properties.GetCount(); I++)
		{
			if(Properties[I]->HasSetterGetter)
				continue;

			if(Properties[I]->Type.BaseType == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\tCount = ((%s*)Object)->%s.GetCount();\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
			}
			else if(Properties[I]->IsContainer)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\tCount = ((%s*)Object)->%s->GetCount();\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
			}
		}

		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateGetPropertyItemCountMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItemCount(ZEObject* Object, ZEString PropertyName, ZESize& Count)\n"
		"{\n"
		"\treturn GetPropertyItemCount(Object, GetPropertyId(PropertyName), Count);\n"
		"}\n\n", ClassName);
}

static void CreateCallWrapperMethodMethod(FILE* File, ZEMCClass* Class, ZEArray<ZEMCMethod*> Methods)
{
	return;

	const char* ClassName = Class->Name;

	if(!Class->HasPublicConstructor)
	{
		fprintf(File,
			"bool %sClass::CallWrapperMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
			"{\n"
			"\treturn false;\n"
			"}\n\n", ClassName);

		return;
	}

	fprintf(File,
		"bool %sClass::CallWrapperMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
		"{\n"
		"\tconst ZEMethod Method = GetMethods()[MethodId];\n"
		"\tif(Method.Name == \"\" || Method.Name == NULL)\n"
		"\t\treturn false;\n\n"
		"\tif(Method.ParameterCount != ParameterCount)\n"
		"\t\treturn false;\n\n", ClassName);

	if(Methods.GetCount() > 0)
	{
		fprintf(File,
			"\tswitch(MethodId)\n"
			"\t{\n");
	}

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Class->HasPublicConstructor && Methods[I]->IsVirtual && !Methods[I]->IsPure)
		{
			if(Methods[I]->ReturnValue.BaseType != ZE_TT_NULL)
			{
				if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\tReturnValue = (ZEInt32)(((%sScriptBase*)Object)->ScriptTriggered%s(", I, ClassName, Methods[I]->Name.ToCString());

					for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
					{
						ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

						if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
						{
							fprintf(File,
								"((%s)(Parameters[%d]->GetInt32ConstRef()))%s",
								Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
						{
							fprintf(File,
								"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
								Methods[I]->IsOperator ? "*" : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
						{
							fprintf(File,
								"Parameters[%d]->GetArray%sRef<%s>()%s", J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
						else
						{
							fprintf(File,
								"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
						}
					}
					fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? "));\n" : "");
					fprintf(File, "\t\t\treturn true;\n");
				}
				else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\tReturnValue = ((%sScriptBase*)Object)->ScriptTriggered%s(", I, ClassName, Methods[I]->Name.ToCString());

					for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
					{
						ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

						if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
						{
							fprintf(File,
								"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
								Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
						{
							if(Methods[I]->IsOperator)
							{
								fprintf(File,
									"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
									Methods[I]->IsOperator ? "*" : "",
									ClassName, J,
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
							else
							{
								fprintf(File,
									"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
									Methods[I]->IsOperator ? "*" : "",
									Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
						{
							fprintf(File,
								"Parameters[%d]->GetArray%sRef<%s>()%s", J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
						else
						{
							fprintf(File,
								"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
					}
					fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? ");\n" : "");
					fprintf(File, "\t\t\treturn true;\n");
				}
			}
			else
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%sScriptBase*)Object)->ScriptTriggered%s(", I, ClassName, Methods[I]->Name.ToCString());

				for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
				{
					ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
					{
						fprintf(File,
							"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
							Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
					{
						fprintf(File,
							"(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
					{
						fprintf(File,
							"Parameters[%d]->GetArray%sRef<%s>()%s", J,
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else
					{
						fprintf(File,
							"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
				}
				fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? ");\n" : "");
				fprintf(File, "\t\t\treturn true;\n");
			}
		}
		else
		{
			if(Methods[I]->IsOperator && (!Class->HasPublicConstructor || Class->IsAbstract))
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);

				continue;
			}

			if(Methods[I]->ReturnValue.BaseType != ZE_TT_NULL)
			{
				if(Methods[I]->Name == "Class")
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\tReturnValue.SetClass(%s::Class());\n"
						"\t\t\treturn true;\n", I, ClassName);
				}
				else if(Methods[I]->Name == "CreateInstance")
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\tReturnValue.SetObjectPtr(%s::CreateInstance());\n"
						"\t\t\treturn true;\n", I, ClassName);
				}
				else if(strcmp(Methods[I]->Name.ToCString(), Methods[I]->MemberOf.ToCString()) == 0)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
					{
						if(Methods[I]->IsStatic)
						{
							fprintf(File, 
								"\t\tcase %d:\n"
								"\t\t\tReturnValue = (ZEInt32)(%s::%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
						}
						else
						{
							fprintf(File, 
								"\t\tcase %d:\n"
								"\t\t\tReturnValue = (ZEInt32)(((%s*)Object)->%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
						}

						for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
						{
							ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

							if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
							{
								fprintf(File,
									"((%s)(Parameters[%d]->GetInt32ConstRef()))%s",
									Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
							}
							else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
							{
								fprintf(File,
									"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
									Methods[I]->IsOperator ? "*" : "",
									Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
							}
							else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
							{
								fprintf(File,
									"Parameters[%d]->GetArray%sRef<%s>()%s", J,
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
							else
							{
								fprintf(File,
									"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
							}
						}
						fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? "));\n" : "");
						fprintf(File, "\t\t\treturn true;\n");
					}
					else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\treturn false;\n", I);
					}
					else
					{
						if(Methods[I]->IsStatic)
						{
							fprintf(File, 
								"\t\tcase %d:\n"
								"\t\t\tReturnValue = %s::%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
						}
						else
						{
							if(Methods[I]->IsOperator)
							{
								fprintf(File, 
									"\t\tcase %d:\n"
									"\t\t\tReturnValue = ((%s*)Object)->%s(", I, ClassName, Methods[I]->Name.ToCString());
							}
							else
							{
								fprintf(File, 
									"\t\tcase %d:\n"
									"\t\t\tReturnValue = ((%s*)Object)->%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
							}
						}

						for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
						{
							ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

							if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
							{
								fprintf(File,
									"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
									Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
							else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
							{
								if(Methods[I]->IsOperator)
								{
									fprintf(File,
										"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
										Methods[I]->IsOperator ? "*" : "",
										ClassName, J,
										Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
										J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
								}
								else
								{
									fprintf(File,
										"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
										Methods[I]->IsOperator ? "*" : "",
										Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
										Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
										J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
								}
							}
							else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
							{
								fprintf(File,
									"Parameters[%d]->GetArray%sRef<%s>()%s", J,
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
							else
							{
								fprintf(File,
									"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
						}
						fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? ");\n" : "");
						fprintf(File, "\t\t\treturn true;\n");
					}
				}
			}
			else
			{
				if(strcmp(Methods[I]->Name.ToCString(), Methods[I]->MemberOf.ToCString()) == 0)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);

					continue;
				}

				if(Methods[I]->IsStatic)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\t%s::%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
				}
				else
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
				}

				for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
				{
					ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

					if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
					{
						fprintf(File,
							"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
							Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
					{
						fprintf(File,
							"(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
					{
						fprintf(File,
							"Parameters[%d]->GetArray%sRef<%s>()%s", J,
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
							Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
					else
					{
						fprintf(File,
							"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
							Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
							J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
					}
				}
				fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? ");\n" : "");
				fprintf(File, "\t\t\treturn true;\n");
			}
		}
	}


	if(Methods.GetCount() > 0)
	{
		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateCallMethodMethod(FILE* File, ZEMCClass* Class, ZEArray<ZEMCMethod*> Methods)
{
	const char* ClassName = Class->Name;

	fprintf(File,
		"bool %sClass::CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
		"{\n"
		"\tconst ZEMethod Method = GetMethods()[MethodId];\n"
		"\tif(Method.Name == \"\" || Method.Name == NULL)\n"
		"\t\treturn false;\n\n"
		"\tif(Method.ParameterCount != ParameterCount)\n"
		"\t\treturn false;\n\n", ClassName);

	if(Methods.GetCount() > 0)
	{
		fprintf(File,
			"\tswitch(MethodId)\n"
			"\t{\n");
	}

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsOperator && (!Class->HasPublicConstructor || Class->IsAbstract))
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);

			continue;
		}

		if(Methods[I]->ReturnValue.BaseType != ZE_TT_NULL)
		{
			if(Methods[I]->Name == "Class")
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\tReturnValue.SetClass(%s::Class());\n"
					"\t\t\treturn true;\n", I, ClassName);
			}
			else if(Methods[I]->Name == "CreateInstance")
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\tReturnValue.SetObjectPtr(%s::CreateInstance());\n"
					"\t\t\treturn true;\n", I, ClassName);
			}
			else if(strcmp(Methods[I]->Name.ToCString(), Methods[I]->MemberOf.ToCString()) == 0)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
				{
					if(Methods[I]->IsStatic)
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\tReturnValue = (ZEInt32)(%s::%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
					}
					else
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\tReturnValue = (ZEInt32)(((%s*)Object)->%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
					}

					for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
					{
						ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

						if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
						{
							fprintf(File,
								"((%s)(Parameters[%d]->GetInt32ConstRef()))%s",
								Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
						{
							fprintf(File,
								"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
								Methods[I]->IsOperator ? "*" : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
						{
							fprintf(File,
								"Parameters[%d]->GetArray%sRef<%s>()%s", J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
						else
						{
							fprintf(File,
								"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
						}
					}
					fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? "));\n" : "");
					fprintf(File, "\t\t\treturn true;\n");
				}
				else if(Methods[I]->ReturnValue.BaseType == ZEMC_BT_ARRAY)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					if(Methods[I]->IsStatic)
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\tReturnValue = %s::%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
					}
					else
					{
						if(Methods[I]->IsOperator)
						{
							fprintf(File, 
								"\t\tcase %d:\n"
								"\t\t\tReturnValue = ((%s*)Object)->%s(", I, ClassName, Methods[I]->Name.ToCString());
						}
						else
						{
							fprintf(File, 
								"\t\tcase %d:\n"
								"\t\t\tReturnValue = ((%s*)Object)->%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
						}
					}

					for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
					{
						ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

						if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
						{
							fprintf(File,
								"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
								Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
						{
							if(Methods[I]->IsOperator)
							{
								fprintf(File,
									"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
									Methods[I]->IsOperator ? "*" : "",
									ClassName, J,
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
							else
							{
								fprintf(File,
									"%s(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
									Methods[I]->IsOperator ? "*" : "",
									Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
									Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
									J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
							}
						}
						else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
						{
							fprintf(File,
								"Parameters[%d]->GetArray%sRef<%s>()%s", J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
						else
						{
							fprintf(File,
								"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
					}
					fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? ");\n" : "");
					fprintf(File, "\t\t\treturn true;\n");
				}
			}
		}
		else
		{
			if(strcmp(Methods[I]->Name.ToCString(), Methods[I]->MemberOf.ToCString()) == 0)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);

				continue;
			}

			if(Methods[I]->IsStatic)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t%s::%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
			}
			else
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s(", I, Methods[I]->MemberOf.ToCString(), Methods[I]->Name.ToCString());
			}

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.BaseType);

				if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ENUMERATOR)
				{
					fprintf(File,
						"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
						Methods[I]->Parameters[J]->Type.Enumurator->Name.ToCString(), J,
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
				}
				else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_OBJECT_PTR)
				{
					fprintf(File,
						"(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
						Methods[I]->Parameters[J]->Type.Class->Name.ToCString(), J,
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
				}
				else if(Methods[I]->Parameters[J]->Type.BaseType == ZEMC_BT_ARRAY)
				{
					fprintf(File,
						"Parameters[%d]->GetArray%sRef<%s>()%s", J,
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
						Methods[I]->Parameters[J]->Type.Class->Name.ToCString(),
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
				}
				else
				{
					fprintf(File,
						"Parameters[%d]->Get%s%sRef()%s", J, VariantType.ToCString(),
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
				}
			}
			fprintf(File, Methods[I]->Parameters.GetCount() == 0 ? ");\n" : "");
			fprintf(File, "\t\t\treturn true;\n");
		}
	}
	if(Methods.GetCount() > 0)
	{
		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");
}

static void CreateCallMethodMethod(FILE* File, const char* ClassName)
{
	fprintf(File,
		"bool %sClass::CallMethod(ZEObject* Object, ZEString MethodName, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
		"{\n"
		"\tZESize MethodId = GetMethodId(MethodName);\n"
		"\tZESize RealId = MethodId;\n\n"
		"\tbool IsMethodIdFound = false;\n"
		"\tbool IsReachedTopLimit = false;\n\n"
		"\t//Looking for exact method in list.\n"
		"\twhile(!IsMethodIdFound)\n"
		"\t{\n"
		"\t\tconst ZEMethod Method = GetMethods()[RealId];\n\n"
		"\t\tif(Method.Name != MethodName)\n"
		"\t\t{\n"
		"\t\t\tif(!IsReachedTopLimit)\n"
		"\t\t\t{\n"
		"\t\t\t\tIsReachedTopLimit = true; //Could not find method through top.We'll look at bottom now.\n"
		"\t\t\t\tRealId = MethodId + 1; //Original Id + One Id underneath it.\n"
		"\t\t\t\tcontinue;\n"
		"\t\t\t}\n"
		"\t\t\telse\n"
		"\t\t\t\treturn false; //Reached bottom limit.Method could not found.Break\n"
		"\t\t}\n\n"
		"\t\tif(Method.ParameterCount == ParameterCount)\n"
		"\t\t{\n"
		"\t\t\tfor(ZESize I = 0; I < ParameterCount; I++)\n"
		"\t\t\t{\n"
		"\t\t\t\tif(Method.Parameters[I].Type.Type != Parameters[I]->GetType().Type)\n"
		"\t\t\t\t{\n"
		"\t\t\t\t\tIsMethodIdFound = false;\n"
		"\t\t\t\t\tif(!IsReachedTopLimit)\n"
		"\t\t\t\t\t\tRealId--;\n"
		"\t\t\t\t\telse\n"
		"\t\t\t\t\t\tRealId++;\n\n"
		"\t\t\t\t\tbreak;\n"
		"\t\t\t\t}\n\n"
		"\t\t\t\tIsMethodIdFound = true;\n"
		"\t\t\t}\n"
		"\t\t}\n\n"
		"\t\tif(Method.ParameterCount == 0)\n"
		"\t\t\tbreak;\n"
		"\t}\n\n"
		"\treturn CallMethod(Object, RealId, ReturnValue, Parameters, ParameterCount);\n"
		"}\n\n", ClassName);
}

static void CreateZEClassImplementation(FILE* File, ZEMCClass* ClassData, ZEArray<ZEMCMethod*> Methods)
{
	const char* ClassName = ClassData->Name;

	fprintf(File,
	"ZEGUID %sClass::GetGUID()\n"
	"{\n"
	"\treturn ZEGUID();\n"
	"}\n\n", ClassName);

	fprintf(File,
		"ZESize %sClass::GetSizeOfClass()\n"
		"{\n"
		"\treturn sizeof(%s);\n"
		"}\n\n", ClassName, ClassName);

	if (false) //if(ClassData->HasPublicConstructor && !ClassData->IsAbstract)
	{
		fprintf(File,
			"ZESize %sClass::GetSizeOfScriptBaseClass()\n"
			"{\n"
			"\treturn sizeof(%sScriptBase);\n"
			"}\n\n", ClassName, ClassName);
	}
	else
	{
		fprintf(File,
			"ZESize %sClass::GetSizeOfScriptBaseClass()\n"
			"{\n"
			"\treturn 0;\n"
			"}\n\n", ClassName);
	}

	fprintf(File,
	"bool %sClass::AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)\n"
	"{\n"
	"\tconst ZEMethod Method = GetMethods()[EventId];\n"
	"\tif(Method.Name == \"\" || Method.Name == NULL)\n"
	"\t\treturn false;\n\n", ClassName);

	bool IsEventFound = false;

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsEvent)
		{
			IsEventFound = true;
			break;
		}
	}

	if(IsEventFound)
	{
		fprintf(File, 
			"\tswitch(EventId)\n"
			"\t{\n", ClassName);
	}

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsEvent)
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn ((%s*)Target)->%s.AddEventHandler(Handler) ? true : false;\n", I, ClassName, Methods[I]->Name.ToCString());
		}
	}

	if(IsEventFound)
	{
		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");

	fprintf(File,
		"bool %sClass::AddEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler)\n"
		"{\n"
		"\treturn AddEventHandler(Target, GetMethodId(EventName), Handler);\n"
		"}\n\n", ClassName);

	fprintf(File,
		"bool %sClass::RemoveEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)\n"
		"{\n"
		"\tconst ZEMethod Method = GetMethods()[EventId];\n"
		"\tif(Method.Name == \"\" || Method.Name == NULL)\n"
		"\t\treturn false;\n\n", ClassName);

	if(IsEventFound)
	{
		fprintf(File, 
			"\tswitch(EventId)\n"
			"\t{\n", ClassName);
	}

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsEvent)
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn ((%s*)Target)->%s.RemoveEventHandler(Handler) ? true : false;\n", I, ClassName, Methods[I]->Name.ToCString());
		}
	}

	if(IsEventFound)
	{
		fprintf(File, 
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	fprintf(File,
		"\treturn false;\n"
		"}\n\n");

	fprintf(File,
		"bool %sClass::RemoveEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler)\n"
		"{\n"
		"\treturn RemoveEventHandler(Target, GetMethodId(EventName), Handler);\n"
		"}\n\n", ClassName);

	fprintf(File,
	"ZEClass* %sClass::Class()\n"
	"{\n"
	"\tstatic %sClass Class;\n"
	"\treturn &Class;\n"
	"}\n\n", ClassName, ClassName);

	fprintf(File,
	"ZEClass* %sClass::GetClass() const\n"
	"{\n"
	"\treturn %sClass::Class();\n"
	"}\n\n", ClassName, ClassName);

	if(ClassData->HasCreateInstanceMethod)
	{
		fprintf(File,
			"ZEObject* %sClass::CreateInstance()\n"
			"{\n"
			"\treturn %s::CreateInstance();\n"
			"}\n\n", ClassName, ClassName);
	}
	else if(ClassData->HasPublicConstructor && !ClassData->IsAbstract)
	{
		fprintf(File,
			"ZEObject* %sClass::CreateInstance()\n"
			"{\n"
			"\treturn new %s();\n"
			"}\n\n", ClassName, ClassName);
	}
	else
	{
		fprintf(File,
			"ZEObject* %sClass::CreateInstance()\n"
			"{\n"
			"\treturn NULL;\n"
			"}\n\n", ClassName);
	}

	if (false)//if(ClassData->HasPublicConstructor && !ClassData->IsAbstract)
	{
		fprintf(File, 
			"void* %sClass::CreateScriptInstance()\n"
			"{\n"
			"\t//return ZEAngelScriptEngine::GetInstance()->CreateScriptClassInstance(%s::Class());\n"
			"\treturn NULL;\n"
			"}\n\n",ClassName, ClassName);
	}
	else
	{
		fprintf(File, 
			"void* %sClass::CreateScriptInstance()\n"
			"{\n"
			"\treturn NULL;\n"
			"}\n\n",ClassName);
	}
}

static void CreateBuiltInClassImplementation(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"ZEClass* %sClass::GetParentClass()\n"
		"{\n"
		"\treturn NULL;\n"
		"}\n"
		"\n"
		"ZEGUID %sClass::GetGUID()\n"
		"{\n"
		"\treturn ZEGUID();\n"
		"}\n"
		"\n"
		"const ZEMetaAttribute*	%sClass::GetAttributes()\n"
		"{\n"
		"\treturn NULL;\n"
		"}\n"
		"\n"
		"ZESize %sClass::GetAttributeCount()\n"
		"{\n"
		"\treturn 0;\n"
		"}\n"
		"\n"
		"bool %sClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::GetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::SetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZESize Index, ZEVariant& Value)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::RemoveItemFromProperty(ZEObject* Object, ZEString PropertyName, ZESize Index)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::GetPropertyItemCount(ZEObject* Object, ZEString PropertyName, ZESize& Count)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::AddEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::RemoveEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"bool %sClass::RemoveEventHandler(ZEObject* Target, ZEString EventName, ZEEventHandlerBase* Handler)\n"
		"{\n"
		"\treturn false;\n"
		"}\n"
		"\n"
		"ZEObject* %sClass::CreateInstance()\n"
		"{\n"
		"\treturn NULL;\n"
		"}\n"
		"void* %sClass::CreateScriptInstance()\n"
		"{\n"
		"\treturn NULL;\n"
		"}\n",
		ClassName, ClassName, ClassName, ClassName, ClassName, ClassName, ClassName,
		ClassName, ClassName, ClassName, ClassName, ClassName, ClassName, ClassName, 
		ClassName, ClassName, ClassName, ClassName, ClassName, ClassName, ClassName, 
		ClassName, ClassName, ClassName);
}

bool ZEMCGenerator::OpenFile()
{
	OutputFile = fopen(Options->OutputFileName.ToCString(), "w");
	if (OutputFile == NULL)
		return false;

	return true;
}

void ZEMCGenerator::WriteToFile(const char* format, ...)
{
	va_list args;
	va_start (args, format);

	vfprintf(OutputFile, format, args);

	va_end (args);
}

void ZEMCGenerator::CloseFile()
{
	if (OutputFile != NULL)
	{
		fclose(OutputFile);
		OutputFile = NULL;
	}
}

void ZEMCGenerator::SetMetaContext(ZEMCContext* context)
{
	Context = context;
}

void ZEMCGenerator::SetOptions(ZEMCOptions* options)
{
	Options = options;
}

bool ZEMCGenerator::GenerateOld()
{
	if (Context->TargetDeclarations.GetCount() == 0)
	{
		return true;
	}

	FILE* File;
	File = fopen(Options->OutputFileName.ToCString(), "w");

	bool IsBuiltInClass = ((ZEMCClass*)Context->TargetDeclarations.GetLastItem())->IsBuiltInClass;

	//if targettype is not a builtin class container.
	if(((ZEMCClass*)Context->TargetDeclarations.GetCount() > 0) && !IsBuiltInClass)
	{
		fprintf(File, "#include \"%s\"\n", ZEFileInfo::GetFileName(Options->InputFileName).ToCString());
		fprintf(File, "#include \"ZEDS/ZEVariant.h\"\n");
		fprintf(File, "#include \"ZEMeta/ZEReference.h\"\n");
		fprintf(File, "#include \"ZEScript/ZEScriptEngine.h\"\n");
		fprintf(File, "#include <stddef.h>\n");

		for(ZESize TargetIndex = 0; TargetIndex < Context->TargetDeclarations.GetCount(); TargetIndex++)
		{
			ZEMCContext* MetaData = new ZEMCContext();
			MetaData->Declarations.Add(Context->TargetDeclarations[TargetIndex]);

			ZEMCClass* ProcessedClass = (ZEMCClass*)MetaData->Declarations.GetLastItem();

			for(ZESize I = Context->Declarations.GetCount() - 1; I >= 0; I--)
			{
				if(ProcessedClass->BaseClass->Name == Context->Declarations[I]->Name)
				{
					if(((ZEMCClass*)Context->Declarations[I])->IsBuiltInClass)
						break;

					MetaData->Declarations.Add(Context->Declarations[I]);
					ProcessedClass = (ZEMCClass*)Context->Declarations[I];

					if(ProcessedClass->Name == "ZEObject")
						break;
				}
			}

			MetaData->ForwardDeclarations = Context->ForwardDeclarations;
			MetaData->Enumurators = Context->Enumurators;

			ZEArray<ZEMCAttribute*> Attributes;
			ZEArray<ZEMCProperty*> Properties;
			ZEArray<ZEMCMethod*> Methods;

			//searching for duplicate class attributes.we only add one item per object type.
			//Last item in MetaData->Types is ZEObject so we skip it.
			for(int Index = MetaData->Declarations.GetCount() - 2; Index >= 0; Index--)
			{
				ZEMCClass* CurrentClassData = (ZEMCClass*)MetaData->Declarations[Index];
				const char* CurrentClassName = CurrentClassData->Name;

				for(ZESize I = 0; I < CurrentClassData->Attributes.GetCount(); I++)
				{
					bool IsSameAttributeFound = false;

					for(ZESize J = 0; J < Attributes.GetCount(); J++)
					{
						if(Attributes[J]->Name == CurrentClassData->Attributes[I]->Name)
						{
							IsSameAttributeFound = true;

							if(Attributes[J]->Parameters.GetCount() == CurrentClassData->Attributes[I]->Parameters.GetCount())
							{
								for(ZESize K = 0; K < Attributes[J]->Parameters.GetCount(); K++)
								{
									if(Attributes[J]->Parameters[K] != CurrentClassData->Attributes[I]->Parameters[K])
										IsSameAttributeFound = false;
								}
							}
							else
								IsSameAttributeFound = false;
						}
					}

					if(IsSameAttributeFound)
						continue;
					else
					{
						CurrentClassData->Attributes[I]->MemberOf = CurrentClassName;
						Attributes.Add(CurrentClassData->Attributes[I]);
					}
				}

				for(ZESize I = 0; I < CurrentClassData->Properties.GetCount(); I++)
				{
					CurrentClassData->Properties[I]->MemberOf = CurrentClassName;
					Properties.Add(CurrentClassData->Properties[I]);
				}

				//searching for duplicate class methods.we only add one item per object type.
				for(ZESize I = 0; I < CurrentClassData->Methods.GetCount(); I++)
				{
					bool IsSameMethodFound = false;

					for(ZESize J = 0; J < Methods.GetCount(); J++)
					{
						if(Methods[J]->Name == CurrentClassData->Methods[I]->Name)
						{
							IsSameMethodFound = true;

							if(Methods[J]->Parameters.GetCount() == CurrentClassData->Methods[I]->Parameters.GetCount())
							{
								for(ZESize K = 0; K < Methods[J]->Parameters.GetCount(); K++)
								{
									if(Methods[J]->Parameters[K]->Type != CurrentClassData->Methods[I]->Parameters[K]->Type)
										IsSameMethodFound = false;
								}
							}
							else
								IsSameMethodFound = false;
						}
					}

					if(IsSameMethodFound)
						continue;
					else
					{
						CurrentClassData->Methods[I]->MemberOf = CurrentClassName;
						Methods.Add(CurrentClassData->Methods[I]);
					}
				}
			}

			for(ZESize I = 0; I < Properties.GetCount(); I++)
				Properties[I]->ID = I;

			for(ZESize I = 0; I < Methods.GetCount(); I++)
				Methods[I]->ID = I;

			//Our target class is the first item in array
			const char* CurrentClassName = MetaData->Declarations[0]->Name;
			//Target class' parent class is one item after it.
			const char* ParentClassName = MetaData->Declarations[1]->Name;

			ZEMCClass* ClassData = (ZEMCClass*)MetaData->Declarations[0];

			bool HasPublicConstructor = ClassData->HasPublicConstructor;
			bool HasPublicCopyConstructor = ClassData->HasPublicCopyConstructor;
			bool HasCreateInstanceMethod = ClassData->HasCreateInstanceMethod;
			bool IsAbstract = ClassData->IsAbstract;

			PrepareClassDependencies(File, CurrentClassName, Context->ForwardDeclarations);

			CreateScriptBaseClass(File, CurrentClassName, Methods, IsAbstract, HasPublicConstructor);

			CreateGetParentClassMethod(File, CurrentClassName, ParentClassName);
			CreateGetNameMethod(File, CurrentClassName);

			CreateGetAttributesMethod(File, CurrentClassName, Attributes);
			CreateGetAttributeCountMethod(File, CurrentClassName, Attributes.GetCount());

			CreateGetPropertiesMethod(File, IsBuiltInClass, CurrentClassName, Properties);
			CreateGetPropertyCountMethod(File, CurrentClassName, Properties.GetCount());

			CreateGetPropertyOffsetMethod(File, CurrentClassName, Properties);
			CreateGetPropertyOffsetMethod(File, CurrentClassName);

			CreateMethodWrappers(File, ClassData, Methods);
			CreateGetMethodsMethod(File, ClassData, Methods);

			ZESize MethodCountAddition = 0;

			if(ClassData->HasPublicConstructor && !IsAbstract)
				MethodCountAddition += 2;
			
			if(!ClassData->HasPublicCopyConstructor)
				MethodCountAddition++;

			CreateGetMethodCountMethod(File, CurrentClassName, Methods.GetCount() + MethodCountAddition);

			CreateSetPropertyMethod(File, CurrentClassName, Properties);
			CreateSetPropertyMethod(File, CurrentClassName);

			CreateGetPropertyMethod(File, CurrentClassName, Properties);
			CreateGetPropertyMethod(File, CurrentClassName);

			CreateSetPropertyItemMethod(File, CurrentClassName, Properties);
			CreateSetPropertyItemMethod(File, CurrentClassName);

			CreateGetPropertyItemMethod(File, CurrentClassName, Properties);
			CreateGetPropertyItemMethod(File, CurrentClassName);

			CreateAddItemToPropertyMethod(File, CurrentClassName, Properties);
			CreateAddItemToPropertyMethod(File, CurrentClassName);

			CreateAddItemToPropertyWithIndexMethod(File, CurrentClassName, Properties);
			CreateAddItemToPropertyWithIndexMethod(File, CurrentClassName);

			CreateRemoveItemFromPropertyWithIndexMethod(File, CurrentClassName, Properties);
			CreateRemoveItemFromPropertyWithIndexMethod(File, CurrentClassName);

			CreateGetPropertyItemCountMethod(File, CurrentClassName, Properties);
			CreateGetPropertyItemCountMethod(File, CurrentClassName);

			CreateCallWrapperMethodMethod(File, ClassData, Methods);

			CreateCallMethodMethod(File, ClassData, Methods);
			CreateCallMethodMethod(File, CurrentClassName);

			CreateZEClassImplementation(File, ClassData, Methods);

			Properties.Sort(SortPropertiesByHash);
			Methods.Sort(SortMethodsByHash);

			CreateGetPropertyIdMethod(File, CurrentClassName, Properties);
			CreateGetMethodIdMethod(File, CurrentClassName, Methods);
		}
	}
	else if((ZEMCClass*)Context->TargetDeclarations.GetCount() > 0)//If last item in array is builtin class type we compile all classes to seperate files in found header
	{
		fprintf(File, "#include \"ZEMath/%s\"\n", ZEFileInfo::GetFileName(Options->InputFileName).ToCString());
		fprintf(File, "#include \"ZEPrimitiveTypes.h\"\n");
		fprintf(File, "#include \"ZEDS/ZEVariant.h\"\n");
		fprintf(File, "#include \"ZEMeta/ZEReference.h\"\n");
		fprintf(File, "#include <stddef.h>\n\n");

		for(ZESize TargetIndex = 0; TargetIndex < Context->TargetDeclarations.GetCount(); TargetIndex++)
		{
			ZEMCClass* CurrentClassData = (ZEMCClass*)Context->TargetDeclarations[TargetIndex];
			const char* CurrentClassName = CurrentClassData->Name;

			bool HasPublicConstructor = CurrentClassData->HasPublicConstructor;
			bool HasPublicCopyConstructor = CurrentClassData->HasPublicCopyConstructor;
			bool HasCreateInstanceMethod = CurrentClassData->HasCreateInstanceMethod;
			bool IsAbstract = CurrentClassData->IsAbstract;

			ZEArray<ZEMCMethod*> Methods = CurrentClassData->Methods;
			ZEArray<ZEMCProperty*> Properties = CurrentClassData->Properties;

			for(ZESize I = 0; I < Properties.GetCount(); I++)
			{
				Properties[I]->ID = I;
				Properties[I]->MemberOf = CurrentClassName;
			}

			for(ZESize I = 0; I < Methods.GetCount(); I++)
			{
				Methods[I]->ID = I;
				Methods[I]->MemberOf = CurrentClassName;
			}

			CreateGetNameMethod(File, CurrentClassName);

			CreateGetPropertiesMethod(File, IsBuiltInClass, CurrentClassName, Properties);
			CreateGetPropertyCountMethod(File, CurrentClassName, Properties.GetCount());

			CreateGetPropertyOffsetMethod(File, CurrentClassName, Properties);
			CreateGetPropertyOffsetMethod(File, CurrentClassName);

			CreateMethodWrappers(File, CurrentClassData, Methods);

			CreateGetMethodsMethod(File, CurrentClassData, Methods);

			if(!CurrentClassData->HasPublicCopyConstructor && !CurrentClassData->HasCreateInstanceMethod)
				CreateGetMethodCountMethod(File, CurrentClassName, Methods.GetCount() + 1);
			else
				CreateGetMethodCountMethod(File, CurrentClassName, Methods.GetCount());

			CreateSetPropertyMethod(File, CurrentClassName, Properties);
			CreateSetPropertyMethod(File, CurrentClassName);

			CreateGetPropertyMethod(File, CurrentClassName, Properties);
			CreateGetPropertyMethod(File, CurrentClassName);

			CreateCallMethodMethod(File, CurrentClassData, Methods);
			CreateCallMethodMethod(File, CurrentClassName);

			Properties.Sort(SortPropertiesByHash);
			Methods.Sort(SortMethodsByHash);

			CreateGetPropertyIdMethod(File, CurrentClassName, Properties);
			CreateGetMethodIdMethod(File, CurrentClassName, Methods);

			CreateBuiltInClassImplementation(File, CurrentClassName);

			fprintf(File,
				"bool %sClass::CallWrapperMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
				"{\n"
				"\treturn false;\n"
				"}\n\n", CurrentClassName);

			fprintf(File,
				"ZESize %sClass::GetSizeOfClass()\n"
				"{\n"
				"\treturn sizeof(%s);\n"
				"}\n\n", CurrentClassName, CurrentClassName);

			fprintf(File,
				"ZESize %sClass::GetSizeOfScriptBaseClass()\n"
				"{\n"
				"\treturn 0;\n"
				"}\n\n", CurrentClassName);

			fprintf(File,
				"ZEClass* %sClass::Class()\n"
				"{\n"
				"\tstatic %sClass Class;\n"
				"\treturn &Class;\n"
				"}\n\n", CurrentClassName, CurrentClassName);

			fprintf(File,
				"ZEClass* %sClass::GetClass() const\n"
				"{\n"
				"\treturn %sClass::Class();\n"
				"}\n\n", CurrentClassName, CurrentClassName);
		}
	}

	fclose(File);

	return true;
}
