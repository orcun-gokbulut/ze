//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDelegateImp.h
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

template<typename TReturn ArgumentSeperator TemplateArgumentDefinitions>
class ZEDelegate<TReturn (TemplateArguments)>
{
	private:
		void* Object;
		TReturn (*Wrapper)(void* ArgumentSeperator TemplateArguments);

		template<typename TClass, TReturn(TClass::*TMethod)(TemplateArguments)>
		static TReturn WrapperImpMember(void* Object ArgumentSeperator ArgumentDefinitions)
		{
			TClass* ObjectPointer = static_cast<TClass*>(Object);
			return (ObjectPointer->*TMethod)(Arguments);
		}

		template<typename TClass, TReturn(TClass::*TMethod)(TemplateArguments) const>
		static TReturn WrapperImpConst(void* Object ArgumentSeperator ArgumentDefinitions)
		{
			const TClass* ObjectPointer = static_cast<const TClass*>(Object);
			return (ObjectPointer->*TMethod)(Arguments);
		}

		template <TReturn (*TMethod)(TemplateArguments)>
		static TReturn WrapperImpGlobal(void* ArgumentSeperator ArgumentDefinitions)
		{
			return (TMethod)(Arguments);
		}

	public:
		template<class TClass, TReturn(TClass::*TMethod)(TemplateArguments)>
		static ZEDelegate Create(TClass* Object)
		{
			ZEDelegate Delegate;
			Delegate.Object = Object;
			Delegate.Wrapper = &ZEDelegate::WrapperImpMember<TClass, TMethod>;
			return Delegate;
		}

		template<class TClass, TReturn (TClass::*TMethod)(TemplateArguments) const>
		static ZEDelegate CreateConst(const TClass* Object)
		{
			ZEDelegate Delegate;
			Delegate.Object = const_cast<TClass*>(Object);
			Delegate.Wrapper = &ZEDelegate::WrapperImpConst<TClass, TMethod>;
			return Delegate;
		}

		template<TReturn (*TMethod)(TemplateArguments)>
		static ZEDelegate Create()
		{
			ZEDelegate Delegate;
			Delegate.Object = 0;
			Delegate.Wrapper = &ZEDelegate::WrapperImpGlobal<TMethod>;
			return Delegate;
		}

		void Clear()
		{
			Object = 0;
			Wrapper = 0;
		}

		bool IsNull() const
		{
			return Wrapper == 0;
		}

		operator bool() const
		{
			return !IsNull();
		}

		bool operator!() const
		{
			return IsNull();
		}

		TReturn operator()(ArgumentDefinitions) const
		{
			return (*Wrapper)(Object ArgumentSeperator Arguments);
		}

		ZEDelegate()
		{
			Clear();
		}
};
