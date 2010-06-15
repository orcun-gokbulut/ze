//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECommand.h
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
#ifndef	__ZE_COMMAND_H__
#define __ZE_COMMAND_H__

#include "ZEDS\ZENamed.h"
#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEVariant.h"
#include "ZEFastDelegate.h"

#define zeCommands ZECommandManager::GetInstance()
class ZECommand;

typedef ZEArray<ZEVariant> ZECommandParameterList;
typedef fastdelegate::FastDelegate2<ZECommand*, const ZECommandParameterList*, bool> ZECommandCallback;

class ZECommand: public ZENamed
{
	private:
		int							AccessLevel;
		ZECommandCallback			Callback;

	public:

		int							GetAccessLevel();
		void						SetAccessLevel(int AccessLevel);
		
		void						SetCallback(ZECommandCallback Callback);

		bool						Execute(ZECommandParameterList* ParamList);

									ZECommand(const char* Name, ZECommandCallback Callback, int AccessLevel = 0);
};



class ZECommandSection : public ZENamed
{
	private:
		ZEArray<ZECommand*>			Commands;

	public:
		size_t						GetNumberOfCommands();
		
		bool						AddCommand(ZECommand* Command);
		void						DeleteCommand(size_t Index);

		ZECommand*					GetCommand(const char* Name);
		ZECommand*					GetCommand(size_t Index);

		bool						ExecuteCommand(const char* Name, ZEArray<ZEVariant>* Paramlist);

									ZECommandSection();
									ZECommandSection(const char* Name);
									~ZECommandSection();
};



class ZECommandManager 
{
	private:
		ZEArray<ZECommandSection*>	Sections;
		
		bool						Callback_ListSections(ZECommand* Command, const ZECommandParameterList* Params);
		bool						Callback_ListCommands(ZECommand* Command, const ZECommandParameterList* Params);

		ZECommandSection			Commands;
	public:
		size_t						GetNumberOfSections();

		ZECommandSection*			GetCommandSection(const char* Name);
		ZECommand*					GetCommand(const char* Section, const char* Name);

		bool						ExecuteCommand(const char* Section, const char* Name, ZEArray<ZEVariant>* ParamList);
				
		bool						RegisterSection(ZECommandSection* Section);
		bool						UnregisterSection(ZECommandSection* Section);
		
		static ZECommandManager*	GetInstance();

									ZECommandManager();
									~ZECommandManager();
};
#endif



