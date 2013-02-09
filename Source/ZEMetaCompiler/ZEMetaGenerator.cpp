//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaGenerator.cpp
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

#include <stdio.h>
#include "ZEMetaData.h"
#include "ZEMetaGenerator.h"
#include "ZEMeta/ZEMetaAttribute.h"
#include "ZEDS/ZEList.h"
#include "ZEMeta/ZEType.h"
#include "ZEMeta/ZEClass.h"

static ZEInt SortPropertiesByHash(ZEPropertyData* const* PropertyDataA, ZEPropertyData* const* PropertyDataB)
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

static ZEInt SortMethodsByHash(ZEMethodData* const* MethodDataA, ZEMethodData* const* MethodDataB)
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

static const char* GetTypeString(ZEMetaTypeType ParameterType)
{
	switch(ParameterType)
	{
		default:
		case ZE_MTT_UNDEFINED:
			return "ZE_TT_UNDEFINED";

		case ZE_MTT_NULL:
			return "ZE_TT_NULL";

		case ZE_MTT_INTEGER_8:
			return "ZE_TT_INTEGER_8";

		case ZE_MTT_INTEGER_16:
			return "ZE_TT_INTEGER_16";

		case ZE_MTT_INTEGER_32:
			return "ZE_TT_INTEGER_32";

		case ZE_MTT_UNSIGNED_INTEGER_8:
			return "ZE_TT_UNSIGNED_INTEGER_8";

		case ZE_MTT_UNSIGNED_INTEGER_16:
			return "ZE_TT_UNSIGNED_INTEGER_16";

		case ZE_MTT_UNSIGNED_INTEGER_32:
			return "ZE_TT_UNSIGNED_INTEGER_32";

		case ZE_MTT_INTEGER_64:
			return "ZE_TT_INTEGER_64";

		case ZE_MTT_UNSIGNED_INTEGER_64:
			return "ZE_TT_UNSIGNED_INTEGER_64";

		case ZE_MTT_FLOAT:
			return "ZE_TT_FLOAT";

		case ZE_MTT_DOUBLE:
			return "ZE_TT_DOUBLE";

		case ZE_MTT_BOOLEAN:
			return "ZE_TT_BOOLEAN";

		case ZE_MTT_STRING:
			return "ZE_TT_STRING";

		case ZE_MTT_QUATERNION:
			return "ZE_TT_QUATERNION";

		case ZE_MTT_VECTOR2:
			return "ZE_TT_VECTOR2";

		case ZE_MTT_VECTOR3:
			return "ZE_TT_VECTOR3";

		case ZE_MTT_VECTOR4:
			return "ZE_TT_VECTOR4";

		case ZE_MTT_MATRIX3X3:
			return "ZE_TT_MATRIX3X3";

		case ZE_MTT_MATRIX4X4:
			return "ZE_TT_MATRIX4X4";

		case ZE_MTT_CLASS:
			return "ZE_TT_CLASS";

		case ZE_MTT_OBJECT:
			return "ZE_TT_OBJECT";

		case ZE_MTT_OBJECT_PTR:
			return "ZE_TT_OBJECT_PTR";

		case ZE_MTT_ARRAY:
			return "ZE_TT_ARRAY";

		case ZE_MTT_LIST:
			return "ZE_TT_LIST";

		case ZE_MTT_ENUMERATOR:
			return "ZE_TT_ENUMERATOR";
	}
}

static const char* GetTypeQualifierString(ZEMetaTypeQualifier TypeQualifier)
{
	switch(TypeQualifier)
	{
		default:
		case ZE_MTQ_VALUE:
			return "ZE_TQ_VALUE";

		case ZE_MTQ_REFERENCE:
			return "ZE_TQ_REFERENCE";

		case ZE_MTQ_CONST_REFERENCE:
			return "ZE_TQ_CONST_REFERENCE";
	}
}

static const char* ReturnVariantType(ZEMetaTypeType VariantType)
{
	switch(VariantType)
	{
		default:
			return "NULL";

		case ZE_MTT_INTEGER_8:
			return "Int8";

		case ZE_MTT_INTEGER_16:
			return "Int16";

		case ZE_MTT_INTEGER_32:
			return "Int32";

		case ZE_MTT_UNSIGNED_INTEGER_8:
			return "UInt8";

		case ZE_MTT_UNSIGNED_INTEGER_16:
			return "UInt16";

		case ZE_MTT_UNSIGNED_INTEGER_32:
			return "UInt32";

		case ZE_MTT_INTEGER_64:
			return "Int64";

		case ZE_MTT_UNSIGNED_INTEGER_64:
			return "UInt64";

		case ZE_MTT_FLOAT:
			return "Float";

		case ZE_MTT_DOUBLE:
			return "Double";

		case ZE_MTT_BOOLEAN:
			return "Bool";

		case ZE_MTT_ENUMERATOR:
			return "Enum";

		case ZE_MTT_STRING:
			return "String";

		case ZE_MTT_QUATERNION:
			return "Quaternion";

		case ZE_MTT_VECTOR2:
			return "Vector2";

		case ZE_MTT_VECTOR3:
			return "Vector3";

		case ZE_MTT_VECTOR4:
			return "Vector4";

		case ZE_MTT_MATRIX3X3:
			return "Matrix3X3";

		case ZE_MTT_MATRIX4X4:
			return "Matrix4X4";

		case ZE_MTT_ARRAY:
			return "Array";

		case ZE_MTT_OBJECT:
			return "Object";

		case ZE_MTT_OBJECT_PTR:
			return "ObjectPtr";

		case ZE_MTT_CLASS:
			return "Class";
	}
}

static const char* ReturnTypeCast(ZEMetaTypeType VariantType)
{
	switch(VariantType)
	{
		default:
			return "NULL";

		case ZE_MTT_INTEGER_8:
			return "ZEInt8";

		case ZE_MTT_INTEGER_16:
			return "ZEInt16";

		case ZE_MTT_INTEGER_32:
			return "ZEInt32";

		case ZE_MTT_UNSIGNED_INTEGER_8:
			return "ZEUInt8";

		case ZE_MTT_UNSIGNED_INTEGER_16:
			return "ZEUInt16";

		case ZE_MTT_UNSIGNED_INTEGER_32:
			return "ZEUInt32";

		case ZE_MTT_INTEGER_64:
			return "ZEInt64";

		case ZE_MTT_UNSIGNED_INTEGER_64:
			return "ZEUInt64";

		case ZE_MTT_FLOAT:
			return "float";

		case ZE_MTT_DOUBLE:
			return "double";

		case ZE_MTT_BOOLEAN:
			return "bool";

		case ZE_MTT_STRING:
			return "ZEString";

		case ZE_MTT_QUATERNION:
			return "ZEQuaternion";

		case ZE_MTT_VECTOR2:
			return "ZEVector2";

		case ZE_MTT_VECTOR3:
			return "ZEVector3";

		case ZE_MTT_VECTOR4:
			return "ZEVector4";

		case ZE_MTT_MATRIX3X3:
			return "ZEMatrix3X3";

		case ZE_MTT_MATRIX4X4:
			return "ZEMatrix4X4";

		case ZE_MTT_ARRAY:
			return "ZEArray";

		case ZE_MTT_CLASS:
			return "ZEClass";
	}
}

static void PrepareClassDependencies(FILE* File, const char* ClassName, ZEArray<ZEForwardDeclared*> ForwardDeclaredClasses)
{
	fprintf(File, "#include \"%s.h\"\n", ClassName);

	for(ZESize I = 0; I < ForwardDeclaredClasses.GetCount(); I++)
	{
		if(ForwardDeclaredClasses[I]->HeaderFileDeclaredIn == ClassName)
			fprintf(File, "#include \"%s\"\n", ForwardDeclaredClasses[I]->HeaderName.ToCString());
	}

	fprintf(File, "\n");
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

static void CreateGetAttributesMethod(FILE* File, const char* ClassName, ZEArray<ZEAttributeData*> Attributes)
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

static void CreateGetPropertiesMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
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
		if(Properties[I]->Type.Type == ZE_MTT_ENUMERATOR)
		{
			fprintf(File,
				"\tstatic ZEEnumParameter Property%dEnumParameters[%d] =\n"
				"\t{\n", I, Properties[I]->EnumData->Parameters.GetCount());

			for(ZESize J = 0; J < Properties[I]->EnumData->Parameters.GetCount(); J++)
			{
				fprintf(File,
					"\t\t{\"%s\", %d}%s\n",
					Properties[I]->EnumData->Parameters[J]->Name.ToCString(),
					Properties[I]->EnumData->Parameters[J]->Value,
					J < Properties[I]->EnumData->Parameters.GetCount() - 1 ? "," : "");
			}

			fprintf(File,
				"\t};\n\n");

			fprintf(File,
				"\tstatic ZEEnum Property%dEnumerator[1] =\n"
				"\t{\n"
				"\t\t{\"%s\", %#x, Property%dEnumParameters, %d}\n"
				"\t};\n\n",
				I, Properties[I]->EnumData->Name.ToCString(), Properties[I]->EnumData->Hash, I, Properties[I]->EnumData->Parameters.GetCount());
		}
	}

	//Creating Properties
	if(Properties.GetCount() > 0)
	{
		fprintf(File,
			"\tstatic ZEProperty Properties[%d] =\n\t"
			"{\n", Properties.GetCount());

		fprintf(File, "\t\t//{ID, MemberOf*, PropertyName, Hash, Type, IsContainer, IsStatic, Enum*, EnumParameterCount, Attributes*, AttributeCount}\n");
	}
	
	for(ZESize I = 0; I < Properties.GetCount(); I++)
	{
		fprintf(File, "\t\t{%d, %s::Class(), \"%s\", %#x, ZEType(%s, %s, %s, %s), %s, %s, ",
			Properties[I]->ID, Properties[I]->MemberOf.ToCString(),
			Properties[I]->Name.ToCString(), Properties[I]->Hash,
			GetTypeString(Properties[I]->Type.Type), GetTypeQualifierString(Properties[I]->Type.TypeQualifier),
			GetTypeString(Properties[I]->Type.SubType), GetTypeQualifierString(Properties[I]->Type.SubTypeQualifier),
			Properties[I]->IsContainer ? "true" : "false",
			Properties[I]->IsStatic ? "true" : "false");

		if(Properties[I]->Type.Type == ZE_MTT_ENUMERATOR)
			fprintf(File, "Property%dEnumerator, %d, ", I, Properties[I]->EnumData->Parameters.GetCount());
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

static void CreateGetPropertyCountMethod(FILE* File, const char* ClassName, ZESize PropertyCount)
{
	fprintf(File, 
		"ZESize %sClass::GetPropertyCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		ClassName, PropertyCount);
}

static void CreateGetMethodsMethod(FILE* File, const char* ClassName, ZEArray<ZEMethodData*> Methods)
{
	fprintf(File, 
		"const ZEMethod* %sClass::GetMethods()\n"
		"{\n",ClassName);


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
			if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_ENUMERATOR)
			{
				fprintf(File,
					"\tstatic ZEEnumParameter Method%dParameter%dEnumParameters[%d] =\n"
					"\t{\n", I, J, Methods[I]->Parameters[J]->EnumData->Parameters.GetCount());

				for(ZESize K = 0; K < Methods[I]->Parameters[J]->EnumData->Parameters.GetCount(); K++)
				{
					fprintf(File,
						"\t\t{\"%s\", %d}%s\n",
						Methods[I]->Parameters[J]->EnumData->Parameters[K]->Name.ToCString(),
						Methods[I]->Parameters[J]->EnumData->Parameters[K]->Value,
						K < Methods[I]->Parameters[J]->EnumData->Parameters.GetCount() - 1 ? "," : "");
				}

				fprintf(File,
					"\t};\n\n");

				fprintf(File,
					"\tstatic ZEEnum Method%dParameter%dEnumerator[1] =\n"
					"\t{\n"
					"\t\t{\"%s\", %#x, Method%dParameter%dEnumParameters, %d}\n"
					"\t};\n\n",
					I, J, 
					Methods[I]->Parameters[J]->EnumData->Name.ToCString(), 
					Methods[I]->Parameters[J]->EnumData->Hash, 
					I, J,
					Methods[I]->Parameters[J]->EnumData->Parameters.GetCount());
			}
		}
	}

	//Creating method parameters
	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->Parameters.GetCount() > 0)
		{
			fprintf(File,
				"\tstatic ZEMethodParameter Method%dParameters[%d] ="
				"\n\t{\n", I, Methods[I]->Parameters.GetCount());

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				fprintf(File,
					"\t\t{\"%s\", ZEType(%s, %s, %s, %s), ",
					Methods[I]->Parameters[J]->Name.ToCString(),
					GetTypeString(Methods[I]->Parameters[J]->Type.Type), GetTypeQualifierString(Methods[I]->Parameters[J]->Type.TypeQualifier),
					GetTypeString(Methods[I]->Parameters[J]->Type.SubType), GetTypeQualifierString(Methods[I]->Parameters[J]->Type.SubTypeQualifier));


				if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_ENUMERATOR)
					fprintf(File, "Method%dParameter%dEnumerator, %d}",	I, J, Methods[I]->Parameters[J]->EnumData->Parameters.GetCount());
				else
					fprintf(File, "NULL, 0}");

				fprintf(File, "%s\n", J != Methods[I]->Parameters.GetCount() - 1 ? "," : "");
			}

			fprintf(File, "\t};\n\n");
		}
	}

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->ReturnParameter.Type.Type == ZE_MTT_ENUMERATOR)
		{
			fprintf(File,
				"\tstatic ZEEnumParameter Method%dReturnValueEnumParameters[%d] =\n"
				"\t{\n", I, Methods[I]->ReturnParameter.EnumData->Parameters.GetCount());

			for(ZESize J = 0; J < Methods[I]->ReturnParameter.EnumData->Parameters.GetCount(); J++)
			{
				fprintf(File,
					"\t\t{\"%s\", %d}%s\n",
					Methods[I]->ReturnParameter.EnumData->Parameters[J]->Name.ToCString(),
					Methods[I]->ReturnParameter.EnumData->Parameters[J]->Value,
					J < Methods[I]->ReturnParameter.EnumData->Parameters.GetCount() - 1 ? "," : "");
			}

			fprintf(File,
				"\t};\n\n");

			fprintf(File,
				"\tstatic ZEEnum Method%dReturnValueEnum[1] =\n"
				"\t{\n"
				"\t\t{\"%s\", %#x, Method%dReturnValueEnumParameters, %d}\n"
				"\t};\n\n",
				I, Methods[I]->ReturnParameter.EnumData->Name.ToCString(), 
				Methods[I]->ReturnParameter.EnumData->Hash, 
				I, Methods[I]->ReturnParameter.EnumData->Parameters.GetCount());
		}
	}

	if(Methods.GetCount() > 0)
	{
		fprintf(File,
			"\tstatic ZEMethod Methods[%d] =\n\t"
			"{\n", Methods.GetCount());

		fprintf(File, "\t\t//{ID, MemberOf, MethodName, Hash, IsEvent, IsStatic, ReturnType, EnumReturnType*, EnumReturnParameterCount, Parameters*, ParameterCount, Attributes*, AttributeCount}\n");
	}
	
	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		fprintf(File,
			"\t\t{%d, %s::Class(), \"%s\", %#x, %s, %s, ZEType(%s, %s, %s, %s), ",
			Methods[I]->ID, Methods[I]->MemberOf.ToCString(),
			Methods[I]->Name.ToCString(), Methods[I]->Hash,
			Methods[I]->IsEvent ? "true" : "false",
			Methods[I]->IsStatic ? "true" : "false",
			GetTypeString(Methods[I]->ReturnParameter.Type.Type), GetTypeQualifierString(Methods[I]->ReturnParameter.Type.TypeQualifier),
			GetTypeString(Methods[I]->ReturnParameter.Type.SubType), GetTypeQualifierString(Methods[I]->ReturnParameter.Type.SubTypeQualifier));

		if(Methods[I]->ReturnParameter.Type.Type == ZE_MTT_ENUMERATOR)
			fprintf(File, "Method%dReturnValueEnum, %d, ", I, Methods[I]->ReturnParameter.EnumData->Parameters.GetCount());
		else
			fprintf(File, "NULL, 0, ");

		if(Methods[I]->Parameters.GetCount() > 0)
			fprintf(File, "Method%dParameters, %d, ", I, Methods[I]->Parameters.GetCount());
		else
			fprintf(File, "NULL, 0, ");

		if(Methods[I]->Attributes.GetCount() > 0)
			fprintf(File, "Method%dAttributes, %d", I, Methods[I]->Attributes.GetCount());
		else
			fprintf(File, "NULL, 0");

		fprintf(File, "}%s\n", I != Methods.GetCount() - 1 ? "," : "");
	}

	if(Methods.GetCount() > 0)
	{
		fprintf(File, 
			"\t};\n\n"
			"\treturn Methods;\n"
			"}\n\n"
			);
	}
	else
	{
		fprintf(File, 
			"\n"
			"\treturn NULL;\n"
			"}\n\n"
			);
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

static void CreateGetPropertyIdMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File,
		"ZESize %sClass::GetPropertyId(ZEString PropertyName)\n"
		"{\n"
		"\tstruct ZESortedPropertyData\n"
		"\t{\n"
		"\t\tZEUInt32\t\tHash;\n"
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
			"\tZEUInt32 Hash = PropertyName.Hash();\n\n"
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

static void CreateGetMethodIdMethod(FILE* File, const char* ClassName, ZEArray<ZEMethodData*> Methods)
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
			"\tZEUInt32 Hash = MethodName.Hash();\n\n"
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

static void CreateSetPropertyMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File,
		"bool %sClass::SetProperty(ZEObject* Object, ZESize PropertyId, const ZENewVariant& Value)\n"
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
		ZEString VariantType = ReturnVariantType(Properties[I]->Type.Type);

		if(Properties[I]->IsStatic)
		{
			if(Properties[I]->Type.Type == ZE_MTT_CLASS)
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
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.ClassData->Name.ToCString());
				}
			}
			else if(Properties[I]->Type.Type == ZE_MTT_ENUMERATOR)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\t%s::%s = (%s)Value.Get%s();\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(),
					Properties[I]->Type.Type == ZE_MTT_ENUMERATOR ? Properties[I]->EnumData->Name.ToCString() : "",
					VariantType.ToCString());
			}
			else if(Properties[I]->Type.Type == ZE_MTT_ARRAY)
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
			if(Properties[I]->Type.Type == ZE_MTT_CLASS)
			{
				if(Properties[I]->IsContainer)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else if(Properties[I]->IsGeneratedByMetaCompiler)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->Set%s((%s*)Value.GetObjectPtr());\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.ClassData->Name.ToCString());
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s = (%s*)Value.GetObjectPtr();\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.ClassData->Name.ToCString());
				}
			}
			else if(Properties[I]->Type.Type == ZE_MTT_ENUMERATOR)
			{
				if(Properties[I]->IsGeneratedByMetaCompiler)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->Set%s((%s)Value.Get%s());\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(),
						Properties[I]->Type.Type == ZE_MTT_ENUMERATOR ? Properties[I]->EnumData->Name.ToCString() : "",
						VariantType.ToCString());
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s = (%s)Value.Get%s();\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(),
						Properties[I]->Type.Type == ZE_MTT_ENUMERATOR ? Properties[I]->EnumData->Name.ToCString() : "",
						VariantType.ToCString());
				}
			}
			else if(Properties[I]->Type.Type == ZE_MTT_ARRAY)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				if(Properties[I]->IsGeneratedByMetaCompiler)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->Set%s(Value.Get%s());\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(),
						VariantType.ToCString());
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\t((%s*)Object)->%s = Value.Get%s();\n"
						"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(),
						VariantType.ToCString());
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
		"bool %sClass::SetProperty(ZEObject* Object, ZEString PropertyName, const ZENewVariant& Value)\n"
		"{\n"
		"\treturn SetProperty(Object, GetPropertyId(PropertyName), Value);\n"
		"}\n\n", ClassName);
}

static void CreateGetPropertyMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File,
		"bool %sClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZENewVariant& Value)\n"
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
		ZEString VariantType = ReturnVariantType(Properties[I]->Type.Type);

		if(Properties[I]->IsStatic)
		{
			if(Properties[I]->Type.Type == ZE_MTT_CLASS)
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
			else if(Properties[I]->Type.Type == ZE_MTT_ARRAY)
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
			if(Properties[I]->Type.Type == ZE_MTT_CLASS)
			{
				if(Properties[I]->IsContainer)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n", I);
				}
				else
				{
					if(Properties[I]->IsGeneratedByMetaCompiler)
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\tValue.SetObjectPtr((ZEObject*)(((%s*)Object)->Get%s());\n"
							"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
					}
					else
					{
						fprintf(File,
							"\t\tcase %d:\n"
							"\t\t\tValue.SetObjectPtr((ZEObject*)(((%s*)Object)->Get%s());\n"
							"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString());
					}
				}
			}
			else if(Properties[I]->Type.Type == ZE_MTT_ARRAY)
			{
				fprintf(File,
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				if(Properties[I]->IsGeneratedByMetaCompiler)
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\tValue.Set%s(((%s*)Object)->Get%s());\n"
						"\t\t\treturn true;\n", I, VariantType.ToCString(), ClassName, Properties[I]->Name.ToCString());
				}
				else
				{
					fprintf(File,
						"\t\tcase %d:\n"
						"\t\t\tValue.Set%s(((%s*)Object)->%s);\n"
						"\t\t\treturn true;\n", I, VariantType.ToCString(), ClassName, Properties[I]->Name.ToCString());
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
		"bool %sClass::GetProperty(ZEObject* Object, ZEString PropertyName, ZENewVariant& Value)\n"
		"{\n"
		"\treturn GetProperty(Object, GetPropertyId(PropertyName), Value);\n"
		"}\n\n", ClassName);
}

static void CreateSetPropertyItemMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File, 
		"bool %sClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZENewVariant& Value)\n"
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
		if(Properties[I]->Type.Type == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
		{
			if(Properties[I]->Type.SubType != ZE_TT_CLASS)
			{
				ZEString VariantType = ReturnVariantType(Properties[I]->Type.SubType);

				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s[Index] = Value.Get%s%s();\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), VariantType.ToCString(),
					Properties[I]->Type.SubTypeQualifier == ZE_MTQ_CONST_REFERENCE ? "Const" : "");
			}
			else
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s[Index] = ((%s*)Value.GetObjectPtr());\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.SubTypeClassName.ToCString());
			}
		}
		else if(Properties[I]->IsContainer)
		{
			if(Properties[I]->Type.SubType != ZE_TT_CLASS)
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
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.SubTypeClassName.ToCString());
			}
		}
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
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

static void CreateSetPropertyItemMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::SetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZENewVariant& Value)\n"
		"{\n"
		"\treturn SetPropertyItem(Object, GetPropertyId(PropertyName), Index, Value);\n"
		"}\n\n", ClassName);
}

static void CreateGetPropertyItemMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZENewVariant& Value)\n"
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
		if(Properties[I]->Type.Type == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
		{
			if(Properties[I]->Type.SubTypeQualifier == ZE_TQ_REFERENCE)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\tValue = ((%s*)((%s*)Object)->%s[Index]);\n"
					"\t\t\treturn true;\n", I, Properties[I]->Type.SubTypeClassName.ToCString(), ClassName, Properties[I]->Name.ToCString());
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
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
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

static void CreateGetPropertyItemMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItem(ZEObject* Object, ZEString PropertyName, ZESize Index, ZENewVariant& Value)\n"
		"{\n"
		"\treturn GetPropertyItem(Object, GetPropertyId(PropertyName), Index, Value);\n"
		"}\n\n", ClassName);
}

static void CreateAddItemToPropertyMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZENewVariant& Value)\n"
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
		if(Properties[I]->Type.Type == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
		{
			if(Properties[I]->Type.SubType != ZE_TT_CLASS)
			{
				ZEString VariantType = ReturnVariantType(Properties[I]->Type.SubType);

				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s.Add(Value.Get%s%s());\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), VariantType.ToCString(),
					Properties[I]->Type.SubTypeQualifier == ZE_MTQ_CONST_REFERENCE ? "Const" : "");
			}
			else
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s.Add(((%s*)Value.GetObjectPtr()));\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.SubTypeClassName.ToCString());
			}
		}
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
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

static void CreateAddItemToPropertyMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZENewVariant& Value)\n"
		"{\n"
		"\treturn AddItemToProperty(Object, GetPropertyId(PropertyName), Value);\n"
		"}\n\n", ClassName);
}

static void CreateAddItemToPropertyWithIndexMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZENewVariant& Value)\n"
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
		if(Properties[I]->IsContainer)
		{
			if(Properties[I]->Type.SubType != ZE_TT_CLASS)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\treturn false;\n", I);
			}
			else
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s->Insert(Index, ((%s*)Value.GetObjectPtr()));\n"
					"\t\t\treturn true;\n", I, ClassName, Properties[I]->Name.ToCString(), Properties[I]->Type.SubTypeClassName.ToCString());
			}
		}
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
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

static void CreateAddItemToPropertyWithIndexMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZEString PropertyName, ZESize Index, ZENewVariant& Value)\n"
		"{\n"
		"\treturn AddItemToProperty(Object, GetPropertyId(PropertyName), Index, Value);\n"
		"}\n\n", ClassName);
}

static void CreateRemoveItemFromPropertyWithIndexMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File, 
		"bool %sClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index)\n"
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
		if(Properties[I]->Type.Type == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
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
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
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

static void CreateRemoveItemFromPropertyWithIndexMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::RemoveItemFromProperty(ZEObject* Object, ZEString PropertyName, ZESize Index)\n"
		"{\n"
		"\treturn RemoveItemFromProperty(Object, GetPropertyId(PropertyName), Index);\n"
		"}\n\n", ClassName);
}

static void CreateGetPropertyItemCountMethod(FILE* File, const char* ClassName, ZEArray<ZEPropertyData*> Properties)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count)\n"
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
		if(Properties[I]->Type.Type == ZE_TT_ARRAY && Properties[I]->Type.SubType != ZE_TT_UNDEFINED)
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
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
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

static void CreateGetPropertyItemCountMethod(FILE* File, const char* ClassName)
{
	fprintf(File, 
		"bool %sClass::GetPropertyItemCount(ZEObject* Object, ZEString PropertyName, ZESize& Count)\n"
		"{\n"
		"\treturn GetPropertyItemCount(Object, GetPropertyId(PropertyName), Count);\n"
		"}\n\n", ClassName);
}

static void CreateCallMethodMethod(FILE* File, const char* ClassName, ZEArray<ZEMethodData*> Methods)
{
	fprintf(File,
		"bool %sClass::CallMethod(ZEObject* Object, ZESize MethodId, ZENewVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
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
		if(Methods[I]->ReturnParameter.Type.Type != ZE_TT_NULL)
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
			else
			{
				if(Methods[I]->ReturnParameter.Type.Type == ZE_MTT_ENUMERATOR)
				{
					if(Methods[I]->IsStatic)
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\tReturnValue = (ZEInt32)(%s::%s(", I, ClassName, Methods[I]->Name.ToCString());
					}
					else
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\tReturnValue = (ZEInt32)(((%s*)Object)->%s(", I, ClassName, Methods[I]->Name.ToCString());
					}

					for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
					{
						ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.Type);

						if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_ENUMERATOR)
						{
							fprintf(File,
								"((%s)(Parameters[%d]->GetInt32ConstRef()))%s",
								Methods[I]->Parameters[J]->EnumData->Name.ToCString(), J,
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
						}
						else if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_OBJECT_PTR)
						{
							fprintf(File,
								"(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
								Methods[I]->Parameters[J]->Type.ClassData->Name.ToCString(), J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : "));\n");
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
				else if(Methods[I]->ReturnParameter.Type.Type == ZE_MTT_ARRAY)
				{
					fprintf(File, 
						"\t\tcase %d:\n"
						"\t\t\treturn false;\n");
				}
				else
				{
					if(Methods[I]->IsStatic)
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\tReturnValue = %s::%s(", I, ClassName, Methods[I]->Name.ToCString());
					}
					else
					{
						fprintf(File, 
							"\t\tcase %d:\n"
							"\t\t\tReturnValue = ((%s*)Object)->%s(", I, ClassName, Methods[I]->Name.ToCString());
					}

					for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
					{
						ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.Type);

						if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_ENUMERATOR)
						{
							fprintf(File,
								"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
								Methods[I]->Parameters[J]->EnumData->Name.ToCString(), J,
								J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
						}
						else if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_OBJECT_PTR)
						{
							fprintf(File,
								"(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
								Methods[I]->Parameters[J]->Type.ClassData->Name.ToCString(), J,
								Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
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
			if(Methods[I]->IsStatic)
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t%s::%s(", I, ClassName, Methods[I]->Name.ToCString());
			}
			else
			{
				fprintf(File, 
					"\t\tcase %d:\n"
					"\t\t\t((%s*)Object)->%s(", I, ClassName, Methods[I]->Name.ToCString());
			}

			for(ZESize J = 0; J < Methods[I]->Parameters.GetCount(); J++)
			{
				ZEString VariantType = ReturnVariantType(Methods[I]->Parameters[J]->Type.Type);

				if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_ENUMERATOR)
				{
					fprintf(File,
						"((%s)(Parameters[%d]->GetInt32ConstRef()))%s", 
						Methods[I]->Parameters[J]->EnumData->Name.ToCString(), J,
						J != Methods[I]->Parameters.GetCount() - 1 ? ", " : ");\n");
				}
				else if(Methods[I]->Parameters[J]->Type.Type == ZE_MTT_OBJECT_PTR)
				{
					fprintf(File,
						"(%s*)Parameters[%d]->GetObjectPtr%sRef()%s", 
						Methods[I]->Parameters[J]->Type.ClassData->Name.ToCString(), J,
						Methods[I]->Parameters[J]->Type.TypeQualifier == ZE_TQ_CONST_REFERENCE ? "Const" : "",
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
		"bool %sClass::CallMethod(ZEObject* Object, ZEString MethodName, ZENewVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
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
		"\t\t\t\treturn -1; //Reached bottom limit.Method could not found.Break\n"
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
		"\t\t}\n"
		"\t}\n\n"
		"\treturn CallMethod(Object, RealId, ReturnValue, Parameters, ParameterCount);\n"
		"}\n\n", ClassName);
}

static void CreateZEClassImplementation(FILE* File, const char* ClassName, bool IsAbstract, bool HasPublicConstructor, ZEArray<ZEMethodData*> Methods)
{
	fprintf(File,
	"ZEGUID %sClass::GetGUID()\n"
	"{\n"
	"\treturn ZEGUID();\n"
	"}\n\n", ClassName);

	fprintf(File,
	"bool %sClass::AddEventHandler(ZEObject* Target, ZESize EventId, ZEEventHandlerBase* Handler)\n"
	"{\n"
	"\tconst ZEMethod Method = GetMethods()[EventId];\n"
	"\tif(Method.Name == \"\" || Method.Name == NULL)\n"
	"\t\treturn false;\n\n"
	"\tswitch(EventId)\n"
	"\t{\n", ClassName);

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsEvent)
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn ((%s*)Target)->%s.AddEventHandler(Handler) ? true : false;\n", I, ClassName, Methods[I]->Name.ToCString());
		}
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
		}
	}

	fprintf(File,
	"\t}\n"
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
		"\t\treturn false;\n\n"
		"\tswitch(EventId)\n"
		"\t{\n", ClassName);

	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->IsEvent)
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn ((%s*)Target)->%s.RemoveEventHandler(Handler) ? true : false;\n", I, ClassName, Methods[I]->Name.ToCString());
		}
		else
		{
			fprintf(File, 
				"\t\tcase %d:\n"
				"\t\t\treturn false;\n", I);
		}
	}

	fprintf(File,
		"\t}\n"
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

	bool CreateInstanceMethodFound = false;
	for(ZESize I = 0; I < Methods.GetCount(); I++)
	{
		if(Methods[I]->Name == "CreateInstance")
			CreateInstanceMethodFound = true;
	}

	if(CreateInstanceMethodFound)
	{
		fprintf(File,
			"ZEObject* %sClass::CreateInstance()\n"
			"{\n"
			"\treturn %s::CreateInstance();\n"
			"}\n\n", ClassName, ClassName);
	}
	else if(HasPublicConstructor && !IsAbstract)
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
}

bool ZEMetaGenerator::Generate(const ZEMetaCompilerOptions& Options, ZEMetaData* MetaData)
{	
	ZEArray<ZEAttributeData*> Attributes;
	ZEArray<ZEPropertyData*> Properties;
	ZEArray<ZEMethodData*> Methods;

	for(ZESize Index = 1; Index < MetaData->HeaderTypes.GetCount(); Index++)
	{
		ZEClassData* CurrentClassData = (ZEClassData*)MetaData->HeaderTypes[Index];
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

	if(MetaData->HeaderTypes.GetCount() < 2)
		return false;
	else
	{
		const char* ParentClassName = MetaData->HeaderTypes[MetaData->HeaderTypes.GetCount() - 2]->Name;
		const char* CurrentClassName = MetaData->HeaderTypes[MetaData->HeaderTypes.GetCount() - 1]->Name;

		ZEClassData* ClassData = (ZEClassData*)MetaData->HeaderTypes.GetLastItem();

		bool HasPublicConstructor = ClassData->HasPublicConstructor;
		bool IsAbstract = ClassData->IsAbstract;

		ZEString FilePath = "C:\\Users\\Hakan.Candemir\\Desktop\\";
		FilePath.Append(CurrentClassName);
		FilePath.Append(".h.ZEMeta.cpp");

		FILE* File;
		File = fopen(FilePath.ToCString(), "w");

		PrepareClassDependencies(File, CurrentClassName, MetaData->ForwardDeclaredClasses);

		CreateGetParentClassMethod(File, CurrentClassName, ParentClassName);
		CreateGetNameMethod(File, CurrentClassName);
		CreateGetAttributesMethod(File, CurrentClassName, Attributes);
		CreateGetAttributeCountMethod(File, CurrentClassName, Attributes.GetCount());
		CreateGetPropertiesMethod(File, CurrentClassName, Properties);
		CreateGetPropertyCountMethod(File, CurrentClassName, Properties.GetCount());
		CreateGetMethodsMethod(File, CurrentClassName, Methods);
		CreateGetMethodCountMethod(File, CurrentClassName, Methods.GetCount());

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

		CreateCallMethodMethod(File, CurrentClassName, Methods);
		CreateCallMethodMethod(File, CurrentClassName);

		CreateZEClassImplementation(File, CurrentClassName, IsAbstract, HasPublicConstructor, Methods);

		Properties.Sort(SortPropertiesByHash);
		Methods.Sort(SortMethodsByHash);

		CreateGetPropertyIdMethod(File, CurrentClassName, Properties);
		CreateGetMethodIdMethod(File, CurrentClassName, Methods);

		fclose(File);
	}

	return true;
}
