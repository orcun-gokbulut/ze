//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEventImp.h
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

#include "ZEMacro/ZEMacroRepeater.h"
#include "ZEMacro/ZEMacroLogic.h"
#include "ZEMacro/ZEMacroFlowControl.h"

#define ZE_ARGUMENT_SEPERATOR ZE_MACRO_IF_COMMA(ZE_MACRO_BOOL(ZE_MACRO_INCLUDE_INDEX))
#define ZE_TEMPLATE_ARGUMENT_DEFINITION_MACRO(Index, Parameter)  ZE_MACRO_IF_COMMA(ZE_MACRO_BOOL(Index)) typename TArg##Index
#define ZE_TEMPLATE_ARGUMENT_DEFINITIONS ZE_MACRO_REPEATER(ZE_MACRO_INCLUDE_INDEX, ZE_TEMPLATE_ARGUMENT_DEFINITION_MACRO, 0)
#define ZE_TEMPLATE_ARGUMENT_MACRO(Index, Parameter) ZE_MACRO_IF_COMMA(ZE_MACRO_BOOL(Index)) TArg##Index
#define ZE_TEMPLATE_ARGUMENTS ZE_MACRO_REPEATER(ZE_MACRO_INCLUDE_INDEX, ZE_TEMPLATE_ARGUMENT_MACRO, 0)
#define ZE_ARGUMENT_DEFINITION_MACRO(Index, Parameter) ZE_MACRO_IF_COMMA(ZE_MACRO_BOOL(Index)) TArg##Index Arg##Index
#define ZE_ARGUMENT_DEFINITIONS ZE_MACRO_REPEATER(ZE_MACRO_INCLUDE_INDEX, ZE_ARGUMENT_DEFINITION_MACRO, 0)
#define ZE_ARGUMENT_MACRO(Index, Parameter) ZE_MACRO_IF_COMMA(ZE_MACRO_BOOL(Index)) Arg##Index
#define ZE_ARGUMENTS ZE_MACRO_REPEATER(ZE_MACRO_INCLUDE_INDEX, ZE_ARGUMENT_MACRO, 0)

template<ZE_TEMPLATE_ARGUMENT_DEFINITIONS>
class ZEEvent<void (ZE_TEMPLATE_ARGUMENTS)>
{
	private:
		ZEMethodSignatureGenerator<void (ZE_TEMPLATE_ARGUMENTS)> SignatureGenerator;
		ZEArray<ZEEventHandler<void (ZE_TEMPLATE_ARGUMENTS)> > Handlers;

	public:
		void Call(ZE_ARGUMENT_DEFINITIONS)
		{
			for (ZESize I = 0; I < Handlers.GetCount(); I++)
				Handlers[I].Call(ZE_ARGUMENTS);
		}

		void operator()(ZE_ARGUMENT_DEFINITIONS)
		{
			Call(ZE_ARGUMENTS);
		}

		bool AddEventHandler(ZEEventHandlerBase* Handler)
		{
			if (!ZEMethodSignature::Compare(SignatureGenerator.GetSignature(), Handler->GetSignature()))
				return false;

			Handlers.Add(*(ZEEventHandler<void (ZE_TEMPLATE_ARGUMENTS)>*)Handler);

			return true;
		}

		bool RemoveEventHandler(ZEEventHandlerBase* Handler)
		{
			if (!ZEMethodSignature::Compare(SignatureGenerator.GetSignature(), Handler->GetSignature()))
				return false;

			Handlers.Add(*(ZEEventHandler<void (ZE_TEMPLATE_ARGUMENTS)>*)Handler);

			return true;
		}

		void AddEventHandler(ZEEventHandler<void (ZE_TEMPLATE_ARGUMENTS)>& Handler)
		{
			Handlers.Add(Handler);
		}

		void RemoveEventHandler(ZEEventHandler<void (ZE_TEMPLATE_ARGUMENTS)>& Handler)
		{
			Handlers.DeleteValue(Handler);
		}
};

template<typename TReturn ZE_ARGUMENT_SEPERATOR ZE_TEMPLATE_ARGUMENT_DEFINITIONS>
class ZEEvent<TReturn (ZE_TEMPLATE_ARGUMENTS)>
{
	private:
		ZEMethodSignatureGenerator<TReturn (ZE_TEMPLATE_ARGUMENTS)> SignatureGenerator;
		ZEArray<ZEEventHandler<TReturn (ZE_TEMPLATE_ARGUMENTS)> > Handlers;

	public:
		TReturn Call(ZE_ARGUMENT_DEFINITIONS)
		{
			TReturn ReturnValue;
			for (ZESize I = 0; I < Handlers.GetCount(); I++)
				ReturnValue = Handlers[I].Call(ZE_ARGUMENTS);

			return ReturnValue;
		}

		TReturn operator()(ZE_ARGUMENT_DEFINITIONS)
		{
			return Call(ZE_ARGUMENTS);
		}

		bool AddEventHandler(ZEEventHandlerBase* Handler)
		{
			if (!ZEMethodSignature::Compare(SignatureGenerator.GetSignature(), Handler->GetSignature()))
				return false;

			Handlers.Add(*(ZEEventHandler<TReturn (ZE_TEMPLATE_ARGUMENTS)>*)Handler);

			return true;
		}

		bool RemoveEventHandler(ZEEventHandlerBase* Handler)
		{
			if (!ZEMethodSignature::Compare(SignatureGenerator.GetSignature(), Handler->GetSignature()))
				return false;

			Handlers.Add(*(ZEEventHandler<TReturn (ZE_TEMPLATE_ARGUMENTS)>*)Handler);

			return true;
		}

		void AddEventHandler(ZEEventHandler<TReturn (ZE_TEMPLATE_ARGUMENTS)>& Handler)
		{
			Handlers.Add(Handler);
		}

		void RemoveEventHandler(ZEEventHandler<TReturn (ZE_TEMPLATE_ARGUMENTS)>& Handler)
		{
			Handlers.DeleteValue(Handler);
		}
};

#undef ZE_ARGUMENT_SEPERATOR
#undef ZE_TEMPLATE_ARGUMENT_DEFINITION_MACRO
#undef ZE_TEMPLATE_ARGUMENT_DEFINITIONS
#undef ZE_TEMPLATE_ARGUMENT_MACRO
#undef ZE_TEMPLATE_ARGUMENTS
#undef ZE_ARGUMENT_DEFINITION_MACRO
#undef ZE_ARGUMENT_DEFINITIONS
#undef ZE_ARGUMENT_MACRO
#undef ZE_ARGUMENTS
