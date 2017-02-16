//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEventDelegateImp.h
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
#define ZE_TEMPLATE_KEYWORD template<typename TReturn ZE_ARGUMENT_SEPERATOR ZE_TEMPLATE_ARGUMENT_DEFINITIONS>
#define ZE_TEMPLATE_SPECIALIZATION TReturn (ZE_TEMPLATE_ARGUMENTS)

ZE_TEMPLATE_KEYWORD
class ZEEventDelegate<TReturn (ZE_TEMPLATE_ARGUMENTS)> : public ZEEventDelegateBase
{
	private:
		ZEMTMethodSignatureGenerator<void (ZE_TEMPLATE_ARGUMENTS)> SignatureGenerator;
		void (*Wrapper)(ZEObject* ZE_ARGUMENT_SEPERATOR ZE_TEMPLATE_ARGUMENTS);

		template<typename TClass, TReturn (TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS)>
		static void WrapperImpMember(ZEObject* Object ZE_ARGUMENT_SEPERATOR ZE_ARGUMENT_DEFINITIONS);
		
		template<typename TClass, TReturn (TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS) const>
		static void WrapperImpConst(ZEObject* Object ZE_ARGUMENT_SEPERATOR ZE_ARGUMENT_DEFINITIONS);

	public:
		virtual const ZEMTMethodSignature& GetSignature() const;

		void Call(ZE_ARGUMENT_DEFINITIONS) const;

		bool operator==(const ZEEventDelegate& Other) const;
		void operator()(ZE_ARGUMENT_DEFINITIONS) const;

		template<typename TClass, void(TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS)>
		static ZEEventDelegate Create(TClass* Object);

		template<typename TClass, void (TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS) const>
		static ZEEventDelegate CreateConst(const TClass* Object);

		ZEEventDelegate();
};


// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

ZE_TEMPLATE_KEYWORD
ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>::ZEEventDelegate()
{
	Object = NULL;
	Wrapper = NULL;
}

ZE_TEMPLATE_KEYWORD
template<typename TClass, TReturn (TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS)>
void ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>::WrapperImpMember(ZEObject* Object ZE_ARGUMENT_SEPERATOR ZE_ARGUMENT_DEFINITIONS)
{
	TClass* ObjectPointer = static_cast<TClass*>(Object);
	(ObjectPointer->*TMethod)(ZE_ARGUMENTS);
}

ZE_TEMPLATE_KEYWORD
template<typename TClass, TReturn (TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS) const>
void ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>::WrapperImpConst(ZEObject* Object ZE_ARGUMENT_SEPERATOR ZE_ARGUMENT_DEFINITIONS)
{
	const TClass* ObjectPointer = static_cast<const TClass*>(Object);
	(ObjectPointer->*TMethod)(ZE_ARGUMENTS);
}

ZE_TEMPLATE_KEYWORD
const ZEMTMethodSignature& ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>::GetSignature() const
{
	return SignatureGenerator.GetSignature();
}

ZE_TEMPLATE_KEYWORD
void ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>::Call(ZE_ARGUMENT_DEFINITIONS) const
{
	(*(void (*)(void* ZE_ARGUMENT_SEPERATOR ZE_TEMPLATE_ARGUMENTS))Wrapper)(Object ZE_ARGUMENT_SEPERATOR ZE_ARGUMENTS);
}

ZE_TEMPLATE_KEYWORD
bool ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>::operator==(const ZEEventDelegate& Other) const
{
	return (Object == Other.Object && Wrapper == Other.Wrapper);
}

ZE_TEMPLATE_KEYWORD
void ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>::operator()(ZE_ARGUMENT_DEFINITIONS) const
{
	return (*(void (*)(void* ZE_ARGUMENT_SEPERATOR ZE_TEMPLATE_ARGUMENTS))Wrapper)(Object ZE_ARGUMENT_SEPERATOR ZE_ARGUMENTS);
}

ZE_TEMPLATE_KEYWORD
template<typename TClass, void (TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS)>
ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION> ZEEventDelegate<TReturn (ZE_TEMPLATE_ARGUMENTS)>::Create(TClass* Object)
{
	ZEEventDelegate<void (ZE_TEMPLATE_ARGUMENTS)> Delegate;
	Delegate.Object = Object;
	Delegate.Wrapper = &ZEEventDelegate::WrapperImpMember<TClass, TMethod>;
	return Delegate;
}

ZE_TEMPLATE_KEYWORD
template<typename TClass, void (TClass::*TMethod)(ZE_TEMPLATE_ARGUMENTS) const>
ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION> ZEEventDelegate<TReturn (ZE_TEMPLATE_ARGUMENTS)>::CreateConst(const TClass* Object)
{
	ZEEventDelegate<void (ZE_TEMPLATE_ARGUMENTS)> Delegate;
	Delegate.Object = const_cast<TClass*>(Object);
	Delegate.Wrapper = &ZEDelegate::WrapperImpConst<TClass, TMethod>;
	return Delegate;
}

#undef ZE_TEMPLATE_SPECIALIZATION
#undef ZE_TEMPLATE_KEYWORD
#undef ZE_ARGUMENT_SEPERATOR
#undef ZE_TEMPLATE_ARGUMENT_DEFINITION_MACRO
#undef ZE_TEMPLATE_ARGUMENT_DEFINITIONS
#undef ZE_TEMPLATE_ARGUMENT_MACRO
#undef ZE_TEMPLATE_ARGUMENTS
#undef ZE_ARGUMENT_DEFINITION_MACRO
#undef ZE_ARGUMENT_DEFINITIONS
#undef ZE_ARGUMENT_MACRO
#undef ZE_ARGUMENTS
