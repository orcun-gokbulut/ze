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
#define ZE_TEMPLATE_KEYWORD template<typename TReturn ZE_ARGUMENT_SEPERATOR ZE_TEMPLATE_ARGUMENT_DEFINITIONS>
#define ZE_TEMPLATE_SPECIALIZATION TReturn (ZE_TEMPLATE_ARGUMENTS)

ZE_TEMPLATE_KEYWORD
class ZEEvent<ZE_TEMPLATE_SPECIALIZATION> : public ZEEventBase
{
	private:
		ZEMethodSignatureGenerator<void (ZE_TEMPLATE_ARGUMENTS)> SignatureGenerator;
		ZEArray<ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION> > Delegates;

		void								Clone(const ZEEvent& Event);
		virtual void						CloneConnections(ZEObject* SourceObject, ZEObject* NewObject);

	public:
		virtual const ZEMethodSignature&	GetSignature() const;

		bool								AddDelegate(const ZEEventDelegateBase* Delegate);
		void								AddDelegate(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate);

		bool								RemoveDelegate(const ZEEventDelegateBase* Delegate);
		void								RemoveDelegate(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate);

		virtual void						DisconnectObject(ZEObject* Object);

		void								Clear();

		void								Call(ZE_ARGUMENT_DEFINITIONS) const;

		ZEEvent&							operator=(const ZEEvent& Event);
		ZEEvent&							operator+=(const ZEEventDelegateBase* Delegate);
		ZEEvent&							operator+=(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate);
		ZEEvent&							operator-=(const ZEEventDelegateBase* Delegate);
		ZEEvent&							operator-=(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate);

		void								operator()(ZE_ARGUMENT_DEFINITIONS) const;

											ZEEvent();
											ZEEvent(const ZEEvent& Event);
											~ZEEvent();
};

ZE_TEMPLATE_KEYWORD
const ZEMethodSignature& ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::GetSignature() const
{
	return SignatureGenerator.GetSignature();
}

ZE_TEMPLATE_KEYWORD
void ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::Clone(const ZEEvent& Other)
{
	Delegates.LockWrite();
	{
		Other.Delegates.LockRead();
		{
			Delegates = Other.Delegates;
		}
		Other.Delegates.UnlockRead();

		for (ZESize I = 0; I < Delegates.GetCount(); I++)
			Delegates[I].GetObject()->AddEventConnection(this);
	}
	Delegates.UnlockWrite();
}

ZE_TEMPLATE_KEYWORD
void ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::CloneConnections(ZEObject* SourceObject, ZEObject* NewObject)
{
	Delegates.LockWrite();
	{
		ZESize DelegateCount = Delegates.GetCount();

		for (ZESize I = 0; I < DelegateCount; I++)
		{
			if (Delegates[I].GetObject() != SourceObject)
				continue;

			ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION> DelegateClone = Delegates[I];
			DelegateClone.Object = NewObject;
			Delegates.Add(DelegateClone);
		}
	}
	Delegates.UnlockWrite();
}

ZE_TEMPLATE_KEYWORD
void ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::DisconnectObject(ZEObject* Object)
{
	Delegates.LockWrite();
	{
		for (ZESize I = 0; I < Delegates.GetCount(); I++)
		{
			if (Delegates[I].GetObject() == Object)
			{
				Object->RemoveEventConnection(this);
				Delegates.Remove(I);
				I--;
			}
		}
	}
	Delegates.UnlockWrite();
}

ZE_TEMPLATE_KEYWORD
bool ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::AddDelegate(const ZEEventDelegateBase* Delegate)
{
	zeCheckError(Delegate == NULL, false, "Cannot add delegate. Delegate is NULL.");
	zeCheckError(!ZEMethodSignature::Compare(SignatureGenerator.GetSignature(), Delegate->GetSignature()), false, 
		"Cannot add delegate. ZEEventDelegate signature does not match with the event. "
		"Event Signature: \"%s\", "
		"Delegate Object Name: \"%s\", "
		"Delegate Signature: \"%s\".",
		GetSignature().ToString().ToCString(),
		Delegate->GetObject()->GetClass()->GetName(),
		Delegate->GetSignature().ToString().ToCString());


	Delegates.LockWrite();
	{
		Delegates.Add(*static_cast<const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>*>(Delegate));
		const_cast<ZEObject*>(Delegate->GetObject())->AddEventConnection(this);
	}
	Delegates.UnlockWrite();

	return true;
}

ZE_TEMPLATE_KEYWORD
void ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::AddDelegate(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate)
{
	Delegates.LockWrite();
	{
		Delegates.Add(Delegate);
		const_cast<ZEObject*>(Delegate.GetObject())->AddEventConnection(this);
	}
	Delegates.UnlockWrite();
}

ZE_TEMPLATE_KEYWORD
bool ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::RemoveDelegate(const ZEEventDelegateBase* Delegate)
{
	zeCheckError(Delegate == NULL, false, "Cannot add delegate. Delegate is NULL.");
	zeCheckError(!ZEMethodSignature::Compare(SignatureGenerator.GetSignature(), Delegate->GetSignature()), false, 
		"Cannot add delegate. ZEEventDelegate signature does not match with the event. Delegate Object Name: \"%s\".", Delegate->GetObject()->GetClass()->GetName());

	Delegates.LockWrite();
	{
		Delegates.Add(*static_cast<const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>*>(Delegate));
		const_cast<ZEObject*>(Delegate->GetObject())->AddEventConnection(this);
	}
	Delegates.UnlockWrite();

	return true;
}

ZE_TEMPLATE_KEYWORD
void ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::RemoveDelegate(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate)
{
	Delegates.LockWrite();
	{
		ZESSize Index = Delegates.FindIndex(Delegate);
		if (Index == -1)
		{
			Delegates.UnlockWrite();
			return;
		}

		Delegate.GetObject()->RemoveEventConnection(this);
		Delegates.Remove(Index);
	}
	Delegates.UnlockWrite();
}

ZE_TEMPLATE_KEYWORD
void ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::Call(ZE_ARGUMENT_DEFINITIONS) const
{
	if (GetSuppressed())
		return;

	Delegates.LockRead();
	{
		for (ZESize I = 0; I < Delegates.GetCount(); I++)
			Delegates[I].Call(ZE_ARGUMENTS);
	}
	Delegates.UnlockRead();
}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>& ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::operator=(const ZEEvent<ZE_TEMPLATE_SPECIALIZATION>& Other)
{
	Clone(Other);
	return *this;
}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>& ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::operator+=(const ZEEventDelegateBase* Delegate)
{
	AddDelegate(Delegate);
	return *this;
}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>& ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::operator+=(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate)
{
	AddDelegate(Delegate);
	return *this;
}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>& ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::operator-=(const ZEEventDelegateBase* Delegate)
{
	RemoveDelegate(Delegate);
	return *this;
}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>& ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::operator-=(const ZEEventDelegate<ZE_TEMPLATE_SPECIALIZATION>& Delegate)
{
	RemoveDelegate(Delegate);
	return *this;
}

ZE_TEMPLATE_KEYWORD
void ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::operator()(ZE_ARGUMENT_DEFINITIONS) const
{
	Call(ZE_ARGUMENTS);
}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::ZEEvent()
{

}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::ZEEvent(const ZEEvent<ZE_TEMPLATE_SPECIALIZATION>& Event)
{
	Clone(Event);
}

ZE_TEMPLATE_KEYWORD
ZEEvent<ZE_TEMPLATE_SPECIALIZATION>::~ZEEvent()
{
	Delegates.LockWrite();
	
	for (ZESize I = 0; I < Delegates.GetCount(); I++)
		const_cast<ZEObject*>(Delegates[I].GetObject())->RemoveEventConnection(this);

	Delegates.UnlockWrite();
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
