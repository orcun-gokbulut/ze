//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCGenerator.h
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

#pragma once
#ifndef __ZEMC_GENERATOR_H__
#define __ZEMC_GENERATOR_H__

#include "ZEMCOptions.h"
#include "ZEMCContext.h"

class ZEMCGenerator
{
	private:
		ZEMCContext* Context;
		ZEMCOptions* Options;
		FILE* OutputFile;

		// IO
		bool OpenFile();
		void WriteToFile(const char* ormat, ...);
		void CloseFile();
		
		// Utilities
		const char* ConvertOperatorTypeToName(ZEMCMetaOperatorType OperatorType);
		const char* ConvertOperatorTypeToString(ZEMCMetaOperatorType OperatorType);

		const char* ConvertTypeQualifierToEnum(ZEMCTypeQualifier TypeQualifier);
		const char* ConvertContainerTypeToEnum(ZEMCContainerType ContainerType);
		const char* ConvertBaseTypeToEnum(ZEMCBaseType BaseType);
		ZEString ConvertBaseTypeToName(const ZEMCType& Type);

		ZEString GenerateVariantPostfix(const ZEMCType& baseType, ZEString& CastOutput);
		ZEString GenerateTypeSignature(const ZEMCType& Type);
		ZEString GenerateTypeConstructor(const ZEMCType& Type);
		ZEString GenerateMethodPointerCast(ZEMCMethod* CurrentMethod, ZEMCClass* CurrentClass);

		// File
		void GenerateHeading();
		void GenerateEnding();
		
		// Generic
		void GenerateCastedObject(ZEMCClass* CurrentClass);

		// Enumurator
		void GenerateEnumeratorMacros(ZEMCEnumerator* Enumerator);
		void GenerateEnumeratorGetName(ZEMCEnumerator* Enumerator);
		void GenerateEnumeratorGetGUID(ZEMCEnumerator* Enumerator);
		void GenerateEnumeratorGetItems(ZEMCEnumerator* Enumerator);
		void GenerateEnumeratorGetItemCount(ZEMCEnumerator* Enumerator);
		void GenerateEnumerator(ZEMCEnumerator* Enumerator);
		
		// Class
		void GenerateClassMacros(ZEMCClass* CurrentClass);
		void GenerateClass(ZEMCClass* CurrentClass);
		void GenerateClassForwardDeclarations(ZEMCClass* CurrentClass);
		void GenerateClassGetParentClass(ZEMCClass* CurrentClass);
		void GenerateClassGetName(ZEMCClass* CurrentClass);
		void GenerateClassGetGUID(ZEMCClass* CurrentClass);
		void GenerateClassGetSizeOfClass(ZEMCClass* CurrentClass);
		void GenerateClassGetAttributes(ZEMCClass* CurrentClass);
		void GenerateClassGetAttributeCount(ZEMCClass* CurrentClass);
		void GenerateClassCreateInstance(ZEMCClass* CurrentClass);

		// Properties
		bool HasContainerProperty(ZEMCClass* CurrentClass);
		void GenerateClassPropertyIdRangeCheck(ZEMCClass* CurrentClass);
		void GenerateClassProperties(ZEMCClass* CurrentClass);
		void GenerateClassGetProperties(ZEMCClass* CurrentClass);
		void GenerateClassGetProperties_Attributes(ZEMCClass* CurrentClass);
		void GenerateClassGetProperties_Properties(ZEMCClass* CurrentClass);
		void GenerateClassGetPropertyCount(ZEMCClass* CurrentClass);
		void GenerateClassGetPropertyId(ZEMCClass* CurrentClass);
		void GenerateClassSetProperty(ZEMCClass* CurrentClass);
		void GenerateClassGetProperty(ZEMCClass* CurrentClass);
		void GenerateClassSetPropertyItem(ZEMCClass* CurrentClass);
		void GenerateClassGetPropertyItem(ZEMCClass* CurrentClass);
		void GenerateClassAddItemToProperty(ZEMCClass* CurrentClass);
		void GenerateClassRemoveItemFromProperty(ZEMCClass* CurrentClass);
		void GenerateClassGetPropertyItemCount(ZEMCClass* CurrentClass);

		// Methods
		bool HasEventMethod(ZEMCClass* CurrentClass);
		void GenerateClassCallMethodParameterCheck(ZEMCClass* CurrentClass);
		void GenerateClassMethods(ZEMCClass* CurrentClass);
		ZEString GenerateClassMethodParameterSignature(ZEMCMethod* Method, bool GenerateNames);
		void GenerateClassMethodIdRangeCheck(ZEMCClass* CurrentClass);
		void GenerateClassGetMethods(ZEMCClass* CurrentClass);
		void GenerateClassGetMethods_Attributes(ZEMCClass* CurrentClass);
		void GenerateClassGetMethods_Parameters(ZEMCClass* CurrentClass);
		void GenerateClassGetMethods_Methods(ZEMCClass* CurrentClass);
		void GenerateClassGetMethodCount(ZEMCClass* CurrentClass);
		void GenerateClassGetMethodId(ZEMCClass* CurrentClass);
		void GenerateClassAddEventHandler(ZEMCClass* CurrentClass);
		void GenerateClassRemoveEventHandler(ZEMCClass* CurrentClass);
		void GenerateClassCallMethod(ZEMCClass* CurrentClass);
		void GenerateClassWrapperMethods(ZEMCClass* CurrentClass);

		// Scripting
		void GenerateClassScriptingObject(ZEMCClass* CurrentClass);
		void GenerateClassGetSizeOfScriptingClass(ZEMCClass* CurrentClass);
		void GenerateClassCreateScriptingInstance(ZEMCClass* CurrentClass);



	public:
		void SetMetaContext(ZEMCContext* context);
		void SetOptions(ZEMCOptions* options);

		bool GenerateOld();
		bool Generate();
};

#endif
