//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECommandEngine.cpp
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

#include "ZECommandEngine.h"

#include "ZERegEx/ZERegEx.h"

const ZEArray<ZECommandVariable>& ZEDCommandEngine::GetVariables()
{
	return Variables;
}

void ZEDCommandEngine::SetVariable(const ZEString& Name, const ZEValue& Value)
{
	for (ZESize I = 0; I < Variables.GetCount(); I++)
	{
		if (Variables[I].Name == Name)
		{
			Variables[I].Value = Value;
			break;
		}
	}

	ZERegEx RegEx("[a-zA-Z_][a-zA-Z0-9_]*");
	ZERegExMatch Match;
	if (!RegEx.Match(Name, Match, ZE_REF_NO_MATCH_STRING | ZE_REF_NO_SUBMATCH))
	{
		zeError("%s is not valid variable name.", Name.ToCString());
		return;
	}

	ZECommandVariable* Variable = Variables.Add();
	Variable->Name = Name;
	Variable->Value = Value;
}

void ZEDCommandEngine::UnsetVariable(const ZEString& Name)
{
	for (ZESize I = 0; I < Variables.GetCount(); I++)
	{
		if (Variables[I].Name == Name)
		{
			Variables.Remove(I);
			return;
		}
	}
}

const ZEArray<ZECommandGlobalObject>& ZEDCommandEngine::GetGlobalObjects()
{
	return GlobalObjects;
}

ZECommandGlobalObject* ZEDCommandEngine::GetGlobalObject(const ZEString& Name)
{
	for (ZESize I = 0; I < Variables.GetCount(); I++)
	{
		if (Variables[I].Name == Name)
			return &GlobalObjects[I];
	}

	return NULL;
}

void ZEDCommandEngine::RegisterGlobalObject(const ZEString& Name, ZEObject* Object)
{
	for (ZESize I = 0; I < Variables.GetCount(); I++)
	{
		if (Variables[I].Name == Name)
		{
			zeError("%s global object already registered.", Name.ToCString());
			return;
		}
	}

	ZERegEx RegEx("[a-zA-Z_][a-zA-Z0-9_]*");
	ZERegExMatch Match;
	if (!RegEx.Match(Name, Match, ZE_REF_NO_MATCH_STRING | ZE_REF_NO_SUBMATCH))
	{
		zeError("%s is not valid global object name.", Name.ToCString());
		return;
	}

	ZECommandGlobalObject* GlobalObject = GlobalObjects.Add();
	GlobalObject->Name = Name;
	GlobalObject->Object = Object;
	GlobalObject->ObjectConst = NULL;
}

void ZEDCommandEngine::RegisterGlobalObject(const ZEString& Name, const ZEObject* Object)
{
	for (ZESize I = 0; I < Variables.GetCount(); I++)
	{
		if (Variables[I].Name == Name)
		{
			zeError("%s global object already registered.", Name.ToCString());
			return;
		}
	}

	ZERegEx RegEx("[a-zA-Z_][a-zA-Z0-9_]*");
	ZERegExMatch Match;
	if (!RegEx.Match(Name, Match, ZE_REF_NO_MATCH_STRING | ZE_REF_NO_SUBMATCH))
	{
		zeError("%s is not valid global object name.", Name.ToCString());
		return;
	}

	ZECommandGlobalObject* GlobalObject = GlobalObjects.Add();
	GlobalObject->Name = Name;
	GlobalObject->Object = NULL;
	GlobalObject->ObjectConst = Object;
}

void ZEDCommandEngine::UnregisterGlobalObject(const ZEString& Name)
{
	for (ZESize I = 0; I < Variables.GetCount(); I++)
	{
		if (Variables[I].Name == Name)
		{
			Variables.Remove(I);
			return;
		}
	}
}

ZEString ZEDCommandEngine::Execute(const ZEString& Input)
{
	return "B�ll�k";
}

ZEDCommandEngine::ZEDCommandEngine()
{

}

ZEDCommandEngine::~ZEDCommandEngine()
{

}
