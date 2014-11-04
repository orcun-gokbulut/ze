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
				if(CurrentAttribute->Parameters.GetCount() == 0)
					continue;

				WriteToFile(
					"\tstatic const char* Property%dAttribute%dParameters[%d] = {", 
					I, J, 
					CurrentAttribute->Parameters.GetCount());

				for (ZESize K = 0; K < CurrentAttribute->Parameters.GetCount(); K++)
				{
					WriteToFile("\"%s\"%s", 
						CurrentAttribute->Parameters[K].ToCString(), 
						K != CurrentAttribute->Parameters.GetCount() - 1 ? ", " : "");
				}

				WriteToFile("};\n\n");
			}

			WriteToFile(
				"\tstatic ZEMetaAttribute Property%dAttributes[%d] =\n"
				"\t{\n", 
				I, CurrentProperty->Attributes.GetCount());

			for (ZESize J = 0; J < CurrentProperty->Attributes.GetCount(); J++)
			{
				ZEMCAttribute* currentAttribute = &CurrentProperty->Attributes[I];
				WriteToFile(
					"\t\t{\"%s\", %s, %d}%s\n", 
					currentAttribute->Name.ToCString(),
					(currentAttribute->Parameters.GetCount() == 0 ? "NULL" : "Property%dAttribute%dParameters"),
					currentAttribute->Parameters.GetCount(),
					J < CurrentProperty->Attributes.GetCount() - 1 ? "," : "");
			}

			WriteToFile("\t};\n\n");
		}
	}
}

void ZEMCGenerator::GenerateClassGetProperties_Properties(ZEMCClass* CurrentClass)
{
	WriteToFile(
		"\tstatic ZEProperty Properties[%d] =\n\t"
		"{\n", CurrentClass->Properties.GetCount());

	for (ZESize I = 0; I < CurrentClass->Properties.GetCount(); I++)
	{
		ZEMCProperty* CurrentProperty = CurrentClass->Properties[I];

		WriteToFile("\t\t{");

		WriteToFile("%d, ", CurrentProperty->ID);
		if (!CurrentClass->IsBuiltInClass)
			WriteToFile("%s::Class(), ", CurrentClass->Name.ToCString());
		else
			WriteToFile("NULL, ");

		WriteToFile("\"%s\", ",	CurrentProperty->Name.ToCString());
		WriteToFile("%#x, ", CurrentProperty->Name.Hash());

		if (CurrentProperty->HasSetterGetter)
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

		// Is
		WriteToFile("%s, ",	CurrentProperty->HasSetterGetter ? "true" : "false");
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
		"const ZEProperty* %sClass::GetProperties()\n"
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
		"ZESize %sClass::GetPropertyId(ZEString PropertyName)\n"
		"{\n",
		CurrentClass->Name.ToCString());

	if (CurrentClass->Properties.GetCount() == 0)
	{
		WriteToFile("\treturn -1;\n");
	}
	else
	{
		ZEArray<ZEMCProperty*> sortedProperties = CurrentClass->Properties;
		sortedProperties.Sort(PropertySortOperator);

		WriteToFile(
			"\tstruct ZESortedPropertyData\n"
			"\t{\n"
			"\t\tZESize\t\tHash;\n"
			"\t\tZESize\t\t\tID;\n"
			"\t\tconst char*\t\tPropertyName;\n"
			"\t};\n\n", 
			CurrentClass->Name);

		WriteToFile(
			"\tstatic ZESortedPropertyData SortedProperties[%d] = \n"
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

		WriteToFile(
			"\tZESize Hash = PropertyName.Hash();\n"
			"\tZESize LeftmostIndex = 0, RightmostIndex = %d, MiddleIndex;\n"
			"\twhile (RightmostIndex >= LeftmostIndex)\n"
			"\t{\n"
			"\t\tMiddleIndex = (LeftmostIndex + RightmostIndex) / 2;\n"
			"\t\tif (SortedProperties[MiddleIndex].Hash < Hash)\n"
			"\t\t\tLeftmostIndex  = MiddleIndex + 1;\n"
			"\t\telse if (SortedProperties[MiddleIndex].Hash > Hash)\n"
			"\t\t\tRightmostIndex = MiddleIndex - 1;\n"
			"\t\telse\n"
			"\t\t\tbreak;\n"
			"\t}\n\n"
			"\tif (PropertyName == SortedProperties[MiddleIndex].PropertyName)\n"
			"\t\treturn SortedProperties[MiddleIndex].ID;\n"
			"\n"
			"\treturn -1;\n", 
			sortedProperties.GetCount() - 1);
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

	if (CurrentClass->Properties.GetCount() != 0)
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

			if (CurrentProperty->HasSetterGetter && CurrentProperty->Setter == "")
				continue;

			else if (CurrentProperty->Type.BaseType == ZEMC_BT_OBJECT) // Not Supported Right Now
				continue;
		
			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);

			if (CurrentProperty->IsStatic)
			{
				if (CurrentProperty->HasSetterGetter)
				{
					WriteToFile("\t\t\t%s::%s(", 
						CurrentClass->Name.ToCString(),
						CurrentProperty->Setter.ToCString());
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
				if (CurrentProperty->HasSetterGetter)
				{
					WriteToFile("\t\t\tCastedObject->%s(",
						CurrentProperty->Setter.ToCString());
				}
				else
				{
					WriteToFile("\t\t\tCastedObject->%s = ",
						CurrentProperty->Name.ToCString());
				}
			}

			ZEString VariantCast;
			ZEString VariantPostfix = GenerateVariantPostfix(CurrentProperty->Type, VariantCast);
			WriteToFile("%sValue.Get%s()", VariantCast.ToCString(), VariantPostfix.ToCString());
		
			if (CurrentProperty->HasSetterGetter)
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

	if (CurrentClass->Properties.GetCount() != 0)
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

			if (CurrentProperty->HasSetterGetter && CurrentProperty->Getter == "")
				continue;

			else if (CurrentProperty->Type.BaseType == ZEMC_BT_OBJECT) // Not Supported Right Now
				continue;

			WriteToFile("\t\tcase %d:\n", CurrentProperty->ID);
			WriteToFile("\t\t\tValue = ");

			if (CurrentProperty->IsStatic)
			{
				if (!CurrentProperty->Getter.IsEmpty())
				{
					WriteToFile("%s::%s();\n",
						CurrentClass->Name.ToCString(),
						CurrentProperty->Getter.ToCString());
				}
				else
				{
					WriteToFile("%s::%s;\n",
						CurrentClass->Name.ToCString(),
						CurrentProperty->Name.ToCString());
				}
			}
			else
			{
				if (CurrentProperty->HasSetterGetter)
				{
					WriteToFile("CastedObject->%s();\n",
						CurrentProperty->Setter.ToCString());
				}
				else
				{
					WriteToFile("CastedObject->%s;\n",
						CurrentProperty->Name.ToCString());
				}
			}

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
	if (CurrentClass->IsBuiltInClass)
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
			ZEString VariantPostfix = GenerateVariantPostfix(ContainerItemType, VariantCast);
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
				continue;;

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
			ZEString VariantPostfix = GenerateVariantPostfix(ContainerItemType, VariantCast);
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
