//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGeneratorClassProperty.cpp
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

void ZEMCGenerator::GenerateClassProperties(ZEMCClass* CurrentClass)
{
	GenerateClassGetProperties(CurrentClass);
	GenerateClassGetPropertyId(CurrentClass);
	GenerateClassGetPropertyCount(CurrentClass);
	GenerateClassSetProperty(CurrentClass);
	GenerateClassGetProperty(CurrentClass);
	GenerateClassSetPropertyItem(CurrentClass);
	GenerateClassGetPropertyItem(CurrentClass);
	GenerateClassAddItemToProperty(CurrentClass);
	GenerateClassRemoveItemFromProperty(CurrentClass);
	GenerateClassGetPropertyItemCount(CurrentClass);
}

static ZEInt PropertySortOperator(ZEMCProperty* const* PropertyDataA, ZEMCProperty* const* PropertyDataB)
{
	ZEUInt32 HashA = (**PropertyDataA).Hash;
	ZEUInt32 HashB = (**PropertyDataB).Hash;

	if (HashA > HashB)
		return 1;
	else if (HashA == HashB)
		return 0;
	else
		return -1;
}

bool ZEMCGenerator::HasContainerProperty(ZEMCClass* CurrentClass)
{
	for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
	{
		if (CurrentClass->Properties[I]->IsContainer)
		{
			return true;
		}
	}

	return false;
}

void ZEMCGenerator::GenerateClassPropertyIdRangeCheck(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"\tif (PropertyId >= %d)\n"
		"\t\treturn false;\n\n", 
		CurrentClass->Properties.GetCount());
}

void ZEMCGenerator::GenerateClassGetProperties_Attributes(ZEMCClass* CurrentClass)
{
	//Creating Attributes
	for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
	{
		ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];
		if(CurrentProperty->Attributes.GetCount() > 0)
		{
			for (ZESize J = 0; J < CurrentProperty->Attributes.GetCount(); J++)
			{
				ZEMCAttribute* CurrentAttribute = &CurrentProperty->Attributes[J];
				if(CurrentAttribute->Values.GetCount() == 0)
					continue;

				WriteToFile(
					"\tstatic const char* Property%dAttribute%dParameters[%d] = {", 
					I, J, 
					CurrentAttribute->Values.GetCount());

				for (ZESize K = 0; K < CurrentAttribute->Values.GetCount(); K++)
				{
					WriteToFile("\"%s\"%s", 
						CurrentAttribute->Values[K].ToCString(), 
						K != CurrentAttribute->Values.GetCount() - 1 ? ", " : "");
				}

				WriteToFile("};\n\n");
			}

			WriteToFile(
				"\tstatic ZEMTAttribute Property%dAttributes[%d] =\n"
				"\t{\n", 
				I, CurrentProperty->Attributes.GetCount());

			for (ZESize J = 0; J < CurrentProperty->Attributes.GetCount(); J++)
			{
				ZEMCAttribute* CurrentAttribute = &CurrentProperty->Attributes[J];
				if(CurrentAttribute->Values.GetCount() > 0)
				{
					WriteToFile(
						"\t\t{\"%s\", Property%dAttribute%dParameters, %d}", 
						CurrentAttribute->Name.ToCString(),
						I, J, 
						CurrentAttribute->Values.GetCount());
				}
				else
				{
					WriteToFile("\t\t{\"%s\", NULL, 0}", CurrentAttribute->Name.ToCString());
				}

				WriteToFile("%s\n", J < CurrentProperty->Attributes.GetCount() - 1 ? "," : "");
			}

			WriteToFile("\t};\n\n");
		}
	}
}

void ZEMCGenerator::GenerateClassGetProperties_Properties(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"\tstatic ZEMTProperty Properties[%d] =\n\t"
		"{\n", CurrentClass->Properties.GetCount());

	for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
	{
		ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];

		WriteToFile("\t\t{");

		WriteToFile("%d, ", CurrentProperty->ID);
		if (CurrentProperty->Class != NULL)
			WriteToFile("%s::Class(), ", CurrentProperty->Class->Name.ToCString());
		else
			WriteToFile("NULL, ");
		WriteToFile("\"%s\", ",	CurrentProperty->Name.ToCString());
		WriteToFile("%#x, ", CurrentProperty->Name.Hash());

		if (CurrentProperty->HasAccessors)
		{
			WriteToFile("NULL, ");
		}
		else
		{
			if (CurrentProperty->IsStatic)
			{
				WriteToFile("&%s::%s, ",
					CurrentClass->Name.ToCString(),
					CurrentProperty->Name.ToCString());
			}
			else
			{
				WriteToFile("(void*)offsetof(%s, %s), ",
					CurrentClass->Name.ToCString(),
					CurrentProperty->Name.ToCString());
			}
		}

		// Type
		WriteToFile("%s, ", GenerateTypeConstructor(CurrentProperty->Type).ToCString());

		// Access
		switch (CurrentProperty->Access)
		{
			default:
			case ZEMC_PA_NONE:
				WriteToFile("ZEMT_PA_NONE, ");
				break;

			case ZEMC_PA_READ:
				WriteToFile("ZEMT_PA_READ, ");
				break;

			case ZEMC_PA_WRITE:
				WriteToFile("ZEMT_PA_WRITE, ");
				break;

			case ZEMC_PA_READ_WRITE:
				WriteToFile("ZEMT_PA_READ_WRITE, ");
				break;
		}

		// Is
		WriteToFile("%s, ",	CurrentProperty->HasAccessors ? "true" : "false");
		WriteToFile("%s, ",	CurrentProperty->IsContainer ? "true" : "false");
		WriteToFile("%s, ",	CurrentProperty->IsStatic ? "true" : "false");
		
		// Attributes
		if (CurrentProperty->Attributes.GetCount() > 0)
			WriteToFile("Property%dAttributes, %d}", I, CurrentProperty->Attributes.GetCount());
		else
			WriteToFile("NULL, 0}");

		WriteToFile("%s\n", I != CurrentClass->Properties.GetCount() - 1 ? "," : "");
	}

	WriteToFile("\t};\n\n");
}

void ZEMCGenerator::GenerateClassGetProperties(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"const ZEMTProperty* %sClass::GetProperties()\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	if (CurrentClass->Properties.GetCount() == 0)
	{
		WriteToFile("\treturn NULL;\n");
	}
	else
	{
		GenerateClassGetProperties_Attributes(CurrentClass);
		GenerateClassGetProperties_Properties(CurrentClass);
		WriteToFile("\treturn Properties;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassGetPropertyId(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"ZESize %sClass::GetPropertyId(const ZEString& PropertyName)\n"
		"{\n",
		CurrentClass->Name.ToCString());

	if (CurrentClass->Properties.GetCount() != 0)
	{
		ZEArray<ZEMCProperty*> sortedProperties = CurrentClass->Properties;
		sortedProperties.Sort(PropertySortOperator);

		WriteToFile(
			"\tstatic ZEClassSortedData SortedProperties[%d] = \n"
			"\t{\n"
			"\t\t//{Hash, ID, PropertyName}\n", 
			sortedProperties.GetCount());
		
		for (ZESize I = 0; I < sortedProperties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = sortedProperties[I];
			WriteToFile( 
				"\t\t{%#x, %d, \"%s\"}%s\n",
				CurrentProperty->Hash, 
				CurrentProperty->ID, 
				CurrentProperty->Name.ToCString(),
				I < sortedProperties.GetCount() - 1 ? "," : "");
		}

		WriteToFile("\t};\n\n");

		WriteToFile("\treturn Search(SortedProperties, %d, PropertyName);\n", CurrentClass->Properties.GetCount());
	}
	else
	{
		WriteToFile("\treturn -1;\n");
	}

	WriteToFile("}\n\n");
}

void ZEMCGenerator::GenerateClassGetPropertyCount(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"ZESize %sClass::GetPropertyCount()\n"
		"{\n"
		"\treturn %d;\n"
		"}\n\n",
		CurrentClass->Name.ToCString(), 
		CurrentClass->Properties.GetCount());
}

void ZEMCGenerator::GenerateClassSetProperty(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::SetProperty(ZEObject* Object, ZESize PropertyId, const ZEVariant& Value)\n"
		"{\n",
		CurrentClass->Name.ToCString());

	bool AccessibleProperties = false;
	for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
	{
		if ((CurrentClass->Properties[I]->Access & ZEMC_PA_WRITE) != 0)
		{
			AccessibleProperties = true;
			break;
		}
	}

	if (AccessibleProperties)
	{
		GenerateClassPropertyIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);

		WriteToFile(
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];

			if (CurrentProperty->IsContainer)
				continue;

			if ((CurrentProperty->Access & ZEMC_PA_WRITE) == 0)
				continue;

			if (CurrentProperty->Type.BaseType == ZEMC_BT_OBJECT) // Not Supported Right Now
				continue;
		
			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);

			if (CurrentProperty->IsStatic)
			{
				if (CurrentProperty->Setter != NULL)
				{
					WriteToFile("\t\t\t%s::%s(", 
						CurrentClass->Name.ToCString(),
						CurrentProperty->Setter->Name.ToCString());
				}
				else
				{
					WriteToFile("\t\t\t%s::%s = ",
						CurrentClass->Name.ToCString(),
						CurrentProperty->Name.ToCString());
				}
			}
			else
			{
				if (CurrentProperty->Setter != NULL)
				{
					WriteToFile("\t\t\tCastedObject->%s(",
						CurrentProperty->Setter->Name.ToCString());
				}
				else
				{
					WriteToFile("\t\t\tCastedObject->%s = ",
						CurrentProperty->Name.ToCString());
				}
			}

			ZEString VariantCast;
			ZEString VariantPostfix = GenerateVariantPostfix(CurrentProperty->Type, VariantCast, ZEString());
			WriteToFile("%sValue.Get%s()", VariantCast.ToCString(), VariantPostfix.ToCString());
		
			if (CurrentProperty->HasAccessors)
			{
				WriteToFile(");\n");
			}
			else
			{
				WriteToFile(";\n");
			}

			WriteToFile("\t\t\treturn true;\n");
		}

		WriteToFile("\t}\n");
	}

	WriteToFile("\treturn false;\n"
		"}\n\n");
}

void ZEMCGenerator::GenerateClassGetProperty(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::GetProperty(ZEObject* Object, ZESize PropertyId, ZEVariant& Value)\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	bool AccessibleProperties = false;
	for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
	{
		if ((CurrentClass->Properties[I]->Access & ZEMC_PA_READ) != 0)
		{
			AccessibleProperties = true;
			break;
		}
	}

	if (AccessibleProperties != 0)
	{
		GenerateClassPropertyIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);

		WriteToFile(
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];

			if (CurrentProperty->IsContainer)
				continue;

			if ((CurrentProperty->Access & ZEMC_PA_READ) == 0)
				continue;

			if (CurrentProperty->Type.BaseType == ZEMC_BT_OBJECT) // Not Supported Right Now
				continue;

			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);

			ZEString VariantCast;
			if (CurrentProperty->Type.ContainerType == ZEMC_CT_NONE && CurrentProperty->Type.BaseType == ZEMC_BT_OBJECT_PTR && CurrentProperty->Type.Class->IsForwardDeclared)
				VariantCast = "(ZEObject*)";

			ZEString VariantSetterParam;
			ZEString VariantPostFix = GenerateVariantPostfix(CurrentProperty->Type, ZEString(), VariantSetterParam);

			WriteToFile("\t\t\tValue.Set%s(%s", VariantPostFix.ToCString(), VariantCast.ToCString());

			if (CurrentProperty->Type.Enumurator != NULL)
				WriteToFile("(ZEInt32)");

			if (CurrentProperty->IsStatic)
			{
				if (CurrentProperty->Getter != NULL)
				{
					WriteToFile("%s::%s()",
						CurrentClass->Name.ToCString(),
						CurrentProperty->Getter->Name.ToCString());
				}
				else
				{
					WriteToFile("%s::%s",
						CurrentClass->Name.ToCString(),
						CurrentProperty->Name.ToCString());
				}
			}
			else
			{
				if (CurrentProperty->Getter != NULL)
				{
					WriteToFile("CastedObject->%s()",
						CurrentProperty->Getter->Name.ToCString());
				}
				else
				{
					WriteToFile("CastedObject->%s",
						CurrentProperty->Name.ToCString());
				}
			}
			WriteToFile("%s);\n", VariantSetterParam.ToCString());
			WriteToFile("\t\t\treturn true;\n");
		}

		WriteToFile("\t}\n");
	}

	WriteToFile(
		"\treturn false;\n"
		"}\n\n");
}

void ZEMCGenerator::GenerateClassSetPropertyItem(ZEMCClass* CurrentClass)
{
	if (CurrentClass->IsFundamental)
		return;

	WriteToFile(
		"bool %sClass::SetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n", 
		CurrentClass->Name.ToCString());

	if (HasContainerProperty(CurrentClass))
	{
		GenerateClassPropertyIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);

		WriteToFile(
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];
			if (!CurrentProperty->IsContainer)
				continue;;

			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);
			if(CurrentProperty->IsStatic)
			{
				WriteToFile(
					"\t\t\t%s::%s.SetItem(Index, ",
					CurrentClass->Name.ToCString(),
					CurrentProperty->Name.ToCString());
			}
			else
			{
				WriteToFile(
					"\t\t\tCastedObject->%s.SetItem(Index, ",
					CurrentProperty->Name.ToCString());
			}
	
			if (CurrentProperty->Type.BaseType == ZEMC_BT_ENUMERATOR)
				WriteToFile("(%s)", CurrentProperty->Type.Enumurator->Name.ToCString());

			ZEMCType ContainerItemType = CurrentProperty->Type;
			ContainerItemType.ContainerType = ZEMC_CT_NONE;
			ContainerItemType.TypeQualifier = ZEMC_TQ_VALUE;

			ZEString VariantCast;
			ZEString VariantPostfix = GenerateVariantPostfix(ContainerItemType, VariantCast, ZEString());
			WriteToFile("%sValue.Get%s()", VariantCast.ToCString(), VariantPostfix.ToCString());

			WriteToFile(
				");\n"
				"\t\t\t return true;\n");
		}

		WriteToFile("\t}\n");
	}

	WriteToFile(
		"\treturn false;\n"
		"}\n\n");
}

void ZEMCGenerator::GenerateClassGetPropertyItem(ZEMCClass* CurrentClass)
{
	WriteToFile( 
		"bool %sClass::GetPropertyItem(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n", CurrentClass->Name.ToCString());

	if (HasContainerProperty(CurrentClass))
	{
		GenerateClassPropertyIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);

		WriteToFile(
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];
			if (!CurrentProperty->IsContainer)
				continue;;

			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);
			if(CurrentProperty->IsStatic)
			{
				WriteToFile(
					"\t\t\tValue = %s::%s.GetItem(Index);\n",
					CurrentClass->Name.ToCString(),
					CurrentProperty->Name.ToCString());
			}
			else
			{
				WriteToFile(
					"\t\t\tValue = CastedObject->%s.GetItem(Index);\n",
					CurrentProperty->Name.ToCString());
			}
		}

		WriteToFile("\t}\n");
	}

	WriteToFile(
		"\treturn false;\n"
		"}\n\n");

}

void ZEMCGenerator::GenerateClassAddItemToProperty(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::AddItemToProperty(ZEObject* Object, ZESize PropertyId, ZESize Index, ZEVariant& Value)\n"
		"{\n", CurrentClass->Name.ToCString());

	if (HasContainerProperty(CurrentClass))
	{
		GenerateClassPropertyIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);

		WriteToFile(
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];
			if (!CurrentProperty->IsContainer)
				continue;

			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);
			if (CurrentProperty->IsStatic)
			{
				WriteToFile(
					"\t\t\t%s::%s.Insert(Index, ",
					CurrentClass->Name.ToCString(),
					CurrentProperty->Name.ToCString());
			}
			else
			{
				WriteToFile(
					"\t\t\tCastedObject->%s.Insert(Index, ",
					CurrentProperty->Name.ToCString());
			}

			ZEMCType ContainerItemType = CurrentProperty->Type;
			ContainerItemType.ContainerType = ZEMC_CT_NONE;
			ContainerItemType.TypeQualifier = ZEMC_TQ_VALUE;

			ZEString VariantCast;
			ZEString VariantPostfix = GenerateVariantPostfix(ContainerItemType, VariantCast, ZEString());
			WriteToFile("%sValue.Get%s()", VariantCast.ToCString(), VariantPostfix.ToCString());

			WriteToFile(
				");\n"
				"\t\t\t return true;\n");
		}


		WriteToFile("\t}\n");
	}

	WriteToFile(
		"\treturn false;\n"
		"}\n\n");
}

void ZEMCGenerator::GenerateClassRemoveItemFromProperty(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::RemoveItemFromProperty(ZEObject* Object, ZESize PropertyId, ZESize Index)\n"
		"{\n", CurrentClass->Name.ToCString());

	if (HasContainerProperty(CurrentClass))
	{
		GenerateClassPropertyIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);

		WriteToFile(
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];
			if (!CurrentProperty->IsContainer)
				continue;;

			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);
			if (CurrentProperty->IsStatic)
			{
				WriteToFile(
					"\t\t\t%s::%s.Remove(Index);\n",
					CurrentClass->Name.ToCString(),
					CurrentProperty->Name.ToCString());
			}
			else
			{
				WriteToFile(
					"\t\t\tCastedObject->%s.Remove(Index);\n",
					CurrentProperty->Name.ToCString());
			}

			WriteToFile("\t\t\treturn true;\n");
		}


		WriteToFile("\t}\n");
	}

	WriteToFile(
		"\treturn false;\n"
		"}\n\n");
}

void ZEMCGenerator::GenerateClassGetPropertyItemCount(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"bool %sClass::GetPropertyItemCount(ZEObject* Object, ZESize PropertyId, ZESize& Count)\n"
		"{\n", CurrentClass->Name.ToCString());

	if (HasContainerProperty(CurrentClass))
	{
		GenerateClassPropertyIdRangeCheck(CurrentClass);
		GenerateCastedObject(CurrentClass);

		WriteToFile(
			"\tswitch(PropertyId)\n"
			"\t{\n");

		for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];
			if (!CurrentProperty->IsContainer)
				continue;;

			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);
			if(CurrentProperty->IsStatic)
			{
				WriteToFile(
					"\t\t\tCount = %s::%s.GetCount();\n",
					CurrentClass->Name.ToCString(),
					CurrentProperty->Name.ToCString());
			}
			else
			{
				WriteToFile(
					"\t\t\tCount = CastedObject->%s.GetCount();\n",
					CurrentProperty->Name.ToCString());
			}
		}

		WriteToFile("\t}\n");
	}

	WriteToFile(
		"\treturn false;\n"
		"}\n\n");
}
