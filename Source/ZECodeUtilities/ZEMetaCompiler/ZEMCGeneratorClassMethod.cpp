//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGeneratorClassMethod.cpp
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

void ZEMCGenerator::GenerateClassMethods(ZEMCClass* CurrentClass)
{
	GenerateClassWrapperMethods(CurrentClass);
	GenerateClassGetMethods(CurrentClass);
	GenerateClassGetMethodCount(CurrentClass);
	GenerateClassGetMethodId(CurrentClass);
	GenerateClassAddEventHandler(CurrentClass);
	GenerateClassRemoveEventHandler(CurrentClass);
	GenerateClassCallMethod(CurrentClass);
}

void ZEMCGenerator::GenerateClassMethodIdRangeCheck(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"\tif (MethodId >= %d)\n"
		"\t\treturn false;\n\n", 
		CurrentClass->Methods.GetCount());
}

bool ZEMCGenerator::HasEventMethod(ZEMCClass* CurrentClass)
{
	for (ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
		if (CurrentClass->Methods[I]->IsEvent)
			return true;

	return false;
}


ZEString ZEMCGenerator::GenerateClassMethodParameterSignature(ZEMCMethod* CurrentMethod, bool GenerateNames)
{
	ZEString Output;
	for (int I = 0; I < CurrentMethod->Parameters.GetCount(); I++)
	{
		ZEMCMethodParameter* CurrentParameter = &CurrentMethod->Parameters[I];

		Output.Append(GenerateTypeSignature(CurrentParameter->Type));
		if (GenerateNames)
			Output.Append(ZEFormat::Format(" Arg{0}", I));

		if (I + 1 < CurrentMethod->Parameters.GetCount())
			Output.Append(", ");
	}

	return Output;
}

void ZEMCGenerator::GenerateClassGetMethods_Attributes(ZEMCClass* CurrentClass)
{
	// Attribute Parameters
	for (ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
	{
		ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];
		if(CurrentMethod->Attributes.GetCount() == 0)
			continue;

		for (ZESize J = 0; J < CurrentMethod->Attributes.GetCount(); J++)
		{
			ZEMCAttribute* CurrentAttribute = &CurrentMethod->Attributes[J];
			if(CurrentAttribute->Parameters.GetCount() == 0)
				continue;

			WriteToFile("\tstatic const char* Method%dAttribute%dParameters[%d] = {", 
				I, J, 
				CurrentAttribute->Parameters.GetCount());

			for(ZESize K = 0; K < CurrentAttribute->Parameters.GetCount(); K++)
			{
				WriteToFile("\"%s\"%s", 
					CurrentAttribute->Parameters[K].ToCString(), 
					K != CurrentAttribute->Parameters.GetCount() - 1 ? ", " : "");
			}

			WriteToFile("};\n\n");
			
		}

		// Virtual to Non Virtual

		// Attributes
		WriteToFile(
			"\tstatic ZEMetaAttribute Method%dAttributes[%d] =\n"
			"\t{\n", I, CurrentMethod->Attributes.GetCount());

		for (ZESize J = 0; J < CurrentMethod->Attributes.GetCount(); J++)
		{
			ZEMCAttribute* CurrentAttribute = &CurrentMethod->Attributes[J];
			if(CurrentAttribute->Parameters.GetCount() > 0)
			{
				WriteToFile(
					"\t\t{\"%s\", Method%dAttribute%dParameters, %d", 
					CurrentAttribute->Name.ToCString(),
					I, J, 
					CurrentAttribute->Parameters.GetCount());
			}
			else
			{
				WriteToFile("\t\t{\"%s\", NULL, 0", CurrentAttribute->Name.ToCString());
			}

			WriteToFile("}%s\n", J < CurrentMethod->Attributes.GetCount() - 1 ? "," : "");
		}

		WriteToFile("\t};\n\n");
	}
}

void ZEMCGenerator::GenerateClassGetMethods_Parameters(ZEMCClass* CurrentClass)
{
	// Method Parameters
	for (ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
	{
		ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];
		if(CurrentMethod->Parameters.GetCount() == 0)
			continue;

		WriteToFile(
			"\tstatic ZEMethodParameter Method%dParameters[%d] ="
			"\n\t{\n", I, CurrentMethod->Parameters.GetCount());

		for (ZESize J = 0; J < CurrentMethod->Parameters.GetCount(); J++)
		{
			ZEMCMethodParameter* CurrentParameter = &CurrentMethod->Parameters[J];
			WriteToFile("\t\t{");
			WriteToFile("\"%s\", ", CurrentParameter->Name.ToCString());
			WriteToFile("%s", GenerateTypeConstructor(CurrentParameter->Type).ToCString());
			WriteToFile("}%s\n", J != CurrentMethod->Parameters.GetCount() - 1 ? "," : "");
		}
		WriteToFile("\t};\n\n");
	}
}

ZEString ZEMCGenerator::GenerateMethodPointerCast(ZEMCMethod* CurrentMethod, ZEMCClass* CurrentClass)
{
	return ZEFormat::Format("({0} (*)({1}{2}{3}{4}))",
		CurrentMethod->ReturnValue.BaseType == ZEMC_BT_VOID ? "void" : GenerateTypeSignature(CurrentMethod->ReturnValue),
		CurrentClass != NULL ? CurrentClass->Name : "",
		CurrentClass != NULL ? "*" : "",
		CurrentMethod->Parameters.GetCount() != 0 ? ", " : "",
		GenerateClassMethodParameterSignature(CurrentMethod, false));
}

void ZEMCGenerator::GenerateClassGetMethods_Methods(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"\tstatic ZEMethod Methods[%d] =\n\t"
		"{\n", CurrentClass->Methods.GetCount());

	for (ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
	{
		ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];
		WriteToFile("\t\t{");
		WriteToFile("%d, ", CurrentMethod->ID);
		WriteToFile("\"%s\", ", CurrentMethod->Name.ToCString());
		WriteToFile("%#x, ", CurrentMethod->Name.Hash());
		WriteToFile("%s%s, ", CurrentClass->Name.ToCString(), "::Class()");

		if (CurrentMethod->IsStatic)
			WriteToFile("%s&%s::%s, ", GenerateMethodPointerCast(CurrentMethod, NULL).ToCString(), CurrentClass->Name.ToCString(), CurrentMethod->Name.ToCString());
		else
			WriteToFile("%s&MethodWrapper%d, ", GenerateMethodPointerCast(CurrentMethod, CurrentClass).ToCString(), CurrentMethod->ID);

		WriteToFile("%s, ",	CurrentMethod->IsConst ? "true" : "false");
		WriteToFile("%s, ",	CurrentMethod->IsEvent ? "true" : "false");
		WriteToFile("%s, ", CurrentMethod->IsVirtual ? "true" : "false");
		WriteToFile("%s, ", CurrentMethod->IsStatic ? "true" : "false");
		WriteToFile("%s, ", CurrentMethod->IsOperator ? "true" : "false");
		WriteToFile("%s, ", ConvertOperatorTypeToString(CurrentMethod->OperatorType));
		WriteToFile("%s, ", GenerateTypeConstructor(CurrentMethod->ReturnValue).ToCString());

		if(CurrentMethod->Parameters.GetCount() > 0)
			WriteToFile("Method%dParameters, %d, ", I, CurrentMethod->Parameters.GetCount());
		else
			WriteToFile("NULL, 0, ");

		if(CurrentMethod->Attributes.GetCount() > 0)
			WriteToFile("Method%dAttributes, %d", I, CurrentMethod->Attributes.GetCount());
		else
			WriteToFile("NULL, 0");

		WriteToFile("},\n");
	}

	if (CurrentClass->Methods.GetCount() != 0)
	{
		WriteToFile(
			"\t};\n\n"
			"\treturn Methods;\n");
	}
	else
	{
		WriteToFile("\treturn NULL;\n");
	}
}

void ZEMCGenerator::GenerateClassGetMethods(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"const ZEMethod* %sClass::GetMethods()\n"
		"{\n", CurrentClass->Name.ToCString());

	if (CurrentClass->Methods.GetCount() == 0)
	{
		WriteToFile("\treturn NULL;\n");
	}
	else
	{
		GenerateClassGetMethods_Attributes(CurrentClass);
		GenerateClassGetMethods_Parameters(CurrentClass);
		GenerateClassGetMethods_Methods(CurrentClass);
	}

	WriteToFile("}\n\n");

}

void ZEMCGenerator::GenerateClassGetMethodCount(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZESize %sClass::GetMethodCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		CurrentClass->Name.ToCString(), 
		CurrentClass->Methods.GetCount());
}

void ZEMCGenerator::GenerateClassGetMethodId(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZESize %sClass::GetMethodId(ZEString MethodName, ZESize OverloadIndex)\n"
		"{\n", CurrentClass->Name.ToCString());

	if (CurrentClass->Methods.GetCount() == 0)
	{
		WriteToFile("\treturn -1;\n");
	}
	else
	{
		WriteToFile(
			"\tstruct ZESortedMethodData\n"
			"\t{\n"
			"\t\tZEUInt32\t\tHash;\n"
			"\t\tZESize\t\t\tID;\n"
			"\t\tconst char*\t\tMethodName;\n"
			"\t};\n\n", 
			CurrentClass->Name);

		if (CurrentClass->Methods.GetCount() > 0)
		{
			WriteToFile(
				"\tstatic ZESortedMethodData SortedMethods[%d] = \n"
				"\t{\n"
				"\t\t//{Hash, ID, MethodName}\n", 
				CurrentClass->Methods.GetCount());
		}

		for (ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
		{
			ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];
			WriteToFile(
				"\t\t{%#x, %d, \"%s\"}%s\n",
				CurrentMethod->Hash, 
				CurrentMethod->ID, 
				CurrentMethod->Name.ToCString(),
				I < CurrentClass->Methods.GetCount() - 1 ? "," : "");
		}

		WriteToFile("\t};\n\n");

		WriteToFile(
			"\tZESize Hash = MethodName.Hash();\n\n"
			"\tZESize LeftmostIndex = 0, RightmostIndex = %d, MiddleIndex;\n\n"
			"\t//Binary Search Algorithm\n"
			"\twhile (RightmostIndex >= LeftmostIndex)\n"
			"\t{\n"
			"\t\tMiddleIndex = (LeftmostIndex + RightmostIndex) / 2;\n"
			"\t\tif (SortedMethods[MiddleIndex].Hash < Hash)\n"
			"\t\t\tLeftmostIndex  = MiddleIndex + 1;\n"
			"\t\telse if (SortedMethods[MiddleIndex].Hash > Hash)\n"
			"\t\t\tRightmostIndex = MiddleIndex - 1;\n"
			"\t\telse\n"
			"\t\t\tbreak;\n"
			"\t}\n\n"
			"\tif (MethodName == SortedMethods[MiddleIndex].MethodName)\n"
			"\t\treturn SortedMethods[MiddleIndex].ID;\n\n"
			"\treturn -1;\n", CurrentClass->Methods.GetCount() - 1);
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassAddEventHandler(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::AddEventHandler(ZEObject* Object, ZESize MethodId, ZEEventHandlerBase* Handler)\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	if (HasEventMethod(CurrentClass))
	{
		GenerateCastedObject(CurrentClass);
		GenerateClassMethodIdRangeCheck(CurrentClass);
		
		WriteToFile(
			"\tswitch(EventId)\n"
			"\t{\n", CurrentClass->Name.ToCString());

		for(ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
		{
			ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];
			if(CurrentMethod->IsEvent)
			{
				WriteToFile(
					"\t\tcase %d:\n"
					"\t\t\treturn ");

				if (!CurrentMethod->IsStatic)
					WriteToFile("CastedObject->");
				else
					WriteToFile("%s::", CurrentClass->Name.ToCString());

				WriteToFile("%s.AddEventHandler(Handler);\n", I, CurrentMethod->Name.ToCString());
			}
		}

		WriteToFile("\t}\n");
	}

	WriteToFile("\treturn false;\n");
	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassRemoveEventHandler(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::RemoveEventHandler(ZEObject* Object, ZESize MethodId, ZEEventHandlerBase* Handler)\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	if (HasEventMethod(CurrentClass))
	{
		GenerateCastedObject(CurrentClass);
		GenerateClassMethodIdRangeCheck(CurrentClass);

		WriteToFile(
			"\tswitch(EventId)\n"
			"\t{\n", CurrentClass->Name.ToCString());

		for(ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
		{
			ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];
			if(CurrentMethod->IsEvent)
			{
				WriteToFile(
					"\t\tcase %d:\n"
					"\t\t\treturn ");

				if (!CurrentMethod->IsStatic)
					WriteToFile("CastedObject->");
				else
					WriteToFile("%s::", CurrentClass->Name.ToCString());

				WriteToFile("%s.RemoveEventHandler(Handler);\n", I, CurrentMethod->Name.ToCString());
			}
		}

		WriteToFile("\t}\n");
	}

	WriteToFile("\treturn false;\n");
	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassCallMethod(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::CallMethod(ZEObject* Object, ZESize MethodId, ZEVariant& ReturnValue, const ZEReference** Parameters, ZESize ParameterCount)\n"
		"{\n", 
		CurrentClass->Name.ToCString());
	

	if (CurrentClass->Methods.GetCount() != 0)
	{
		GenerateClassMethodIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);
		//GenerateCallMethodParameterCheck(CurrentClass);

		WriteToFile(
			"\tswitch(MethodId)\n"
			"\t{\n");


		for (ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
		{
			ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];
			WriteToFile("\t\tcase %d:\n", CurrentMethod->ID);
			
			WriteToFile("\t\t\t");
			if (CurrentMethod->ReturnValue.BaseType != ZEMC_BT_VOID)
				WriteToFile("ReturnValue.Set%s(", GenerateVariantPostfix(CurrentMethod->ReturnValue, ZEString()).ToCString());


			if (CurrentMethod->IsStatic)
				WriteToFile("%s::%s(", CurrentClass->Name.ToCString(), CurrentMethod->Name.ToCString());
			else
				WriteToFile("CastedObject->%s(", CurrentMethod->Name.ToCString());

			for (ZESize N = 0; N < CurrentMethod->Parameters.GetCount(); N++)
			{
				ZEMCMethodParameter* CurrentParameter = &CurrentMethod->Parameters[N];
				if (CurrentParameter->Type.BaseType == ZEMC_BT_ENUMERATOR)
					WriteToFile("(%s)", CurrentParameter->Type.Enumurator->Name.ToCString());

				ZEMCType ModifiedParameterType = CurrentParameter->Type;
				if (CurrentParameter->Type.TypeQualifier == ZEMC_TQ_VALUE)
					ModifiedParameterType.TypeQualifier = ZEMC_TQ_REFERENCE;
				else if (CurrentParameter->Type.TypeQualifier == ZEMC_TQ_CONST_VALUE)
					ModifiedParameterType.TypeQualifier = ZEMC_TQ_CONST_REFERENCE;

				ZEString VariantCast;
				ZEString VariantPostfix = GenerateVariantPostfix(ModifiedParameterType, VariantCast);
				WriteToFile("%sParameters[%d]->Get%s()%s", 
					VariantCast.ToCString(),
					N, 
					VariantPostfix.ToCString(),
					N + 1 >= CurrentMethod->Parameters.GetCount() ? "" : ", ");
			}
			
			WriteToFile("));\n");
			WriteToFile("\t\t\treturn true;\n");
		}

		WriteToFile(
			"\t\tdefault:\n"
			"\t\t\treturn false;\n"
			"\t}\n");
	}

	WriteToFile(
		"\treturn false;\n"
		"}\n\n");
}

void ZEMCGenerator::GenerateClassWrapperMethods(ZEMCClass* CurrentClass)
{
	for (ZESize I = 0; I < CurrentClass->Methods.GetCount(); I++)
	{
		ZEMCMethod* CurrentMethod = CurrentClass->Methods[I];

		if (CurrentMethod->IsStatic)
			continue;
		
		WriteToFile("static %s MethodWrapper%d(%s* Object%s%s)\n"
			"{\n", 
			GenerateTypeSignature(CurrentMethod->ReturnValue).ToCString(),
			CurrentMethod->ID,
			CurrentClass->Name.ToCString(),
			CurrentMethod->Parameters.GetCount() != 0 ? ", " : "",
			GenerateClassMethodParameterSignature(CurrentMethod, true).ToCString());
		
		WriteToFile("\t");
		if (CurrentMethod->ReturnValue.BaseType != ZEMC_BT_VOID)
		{
			WriteToFile("return ");
		}
		
		WriteToFile("Object->%s(", CurrentMethod->Name.ToCString());
		for (ZESize N = 0; N < CurrentMethod->Parameters.GetCount(); N++)
		{
			WriteToFile("Arg%d%s",
				N,
				N + 1 != CurrentMethod->Parameters.GetCount() ? ", " : "");
		}
		WriteToFile(
			");\n"
			"}\n\n");
	}
}
