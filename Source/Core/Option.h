//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Option.h
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
#ifndef	__ZE_OPTION_H__
#define __ZE_OPTION_H__

#include "ZEDS/Named.h"
#include "ZEDS/Array.h"
#include "ZEDS/TypedVariant.h"
#include "fastdelegate.h"
#include "Command.h"
#include <string.h>

#define zeOptions ZEOptionManager::GetInstance()

enum ZEOptionAttribute
{
	ZEOPTIONATTRIBUTE_NORMAL,			// This option is public.
	ZEOPTIONATTRIBUTE_HIDDEN,			// This option won't be written to option files. Exp : Game::Cheats=true ;)
	ZEOPTIONATTRIBUTE_INTERNAL,	    // This option won't be readen/writen from/to option files.
};

enum ZEOptionType
{
	OPTIONTYPE_NORMAL,
	OPTIONTYPE_ENUMERATOR,
	OPTIONTYPE_RANGED
};

class ZEOption;

typedef fastdelegate::FastDelegate0<void> ZEOptionsChangedEventCallback;

class ZEOptionSection;
class ZEOption : public ZENamed
{
	friend ZEOptionSection;
	protected:
		ZEOptionSection*		Section;
		ZETypedVariant			DefaultValue;
		ZETypedVariant			Value;
		ZEOptionAttribute		Attribute;
		bool					Changed;

	public:

		virtual void			SetValueType(ZEVariantType NewType);
		ZEVariantType			GetValueType();

		virtual void			SetDefaultValue(ZETypedVariant NewDefaultValue);
		ZETypedVariant			GetDefaultValue();
		
		void					SetToDefault();

		virtual void			SetValue(ZETypedVariant NewValue);
		ZETypedVariant			GetValue();

		ZEOptionAttribute 		GetAttribute();
		void					SetAttribute(ZEOptionAttribute NewAttribute);

		virtual ZEOptionType	GetType(); 

		bool					IsChanged();
		void					ChangeCommitted();

								ZEOption();
								ZEOption(const char *InitialName, ZETypedVariant InitialDefaultValue, 
										 ZEOptionAttribute InitialAttribute);
};

class ZEOptionSection : public ZENamed
{
	friend class ZEOption;
	private:
		ZEArray<ZEOption*>				Options;
		bool							Changed;
		ZEOptionsChangedEventCallback	EventHandler;

	public:
		bool							AddOption(ZEOption* Option);
		void							DeleteOption(size_t Index);
		size_t							GetNumberOfOptions();

		ZEOption*						GetOption(const char* OptionName);
		ZEOption*						GetOption(size_t Index);

		void							SetEventHandler (ZEOptionsChangedEventCallback NewEventHandler);
		
		bool							HasChanges();
		void							CommitChanges();
		void							ResetChanges();

										ZEOptionSection();
										ZEOptionSection(const char* Name);
										~ZEOptionSection();
};

class ZEOptionManager
{
	private:
		ZECommandSection			Commands;
		ZEArray<ZEOptionSection*>	Sections;

		bool						MatchSet(char* Line, char* Match);
		void						MatchOption(char* Line, char* MatchName, char* MatchValue);

		bool						LoadCommand(ZECommand* Command, const ZECommandParameterList* Params);
		bool						SaveCommand(ZECommand* Command, const ZECommandParameterList* Params);
		bool						ListSectionsCommand(ZECommand* Command, const ZECommandParameterList* Params);
		bool						ListOptionsCommand(ZECommand* Command, const ZECommandParameterList* Params);
		bool						CommitChangesCommand(ZECommand* Command, const ZECommandParameterList* Params);
		bool						ResetChangesCommand(ZECommand* Command, const ZECommandParameterList* Params);

	public:
		bool						RegisterSection (ZEOptionSection* Ref);
		bool						UnregisterSection (ZEOptionSection* Ref);
		
		size_t						GetNumberOfSections();
		
		ZEOptionSection*			GetSection(const char* Name);
		ZEOptionSection*			GetSection(size_t Index);

		ZEOption*					GetOption(const char* SectionName, const char* Name);

		void						Save(const char *FileName);
		void						Load(const char *FileName);
		void						ParseParameters(const char *Parameters);
		
		void						CommitChanges();
		void						ResetChanges();

		static ZEOptionManager*		GetInstance();

									ZEOptionManager();
									~ZEOptionManager();
};
#endif
