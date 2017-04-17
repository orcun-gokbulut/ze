//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOptionManager.h
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
#ifndef	__ZE_OPTION_MANAGER_H__
#define __ZE_OPTION_MANAGER_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEValue.h"
#include "ZEOption.h"
#include "ZEOptionSection.h"
#include "ZECommand.h"
#include "ZECommandSection.h"
#include "ZEExport.ZEEngine.h"

class ZE_EXPORT_ZEENGINE ZEOptionManager
{
	friend class ZECore;
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

									ZEOptionManager();
									~ZEOptionManager();

	public:
		bool						RegisterSection(ZEOptionSection* Ref);
		bool						UnregisterSection(ZEOptionSection* Ref);
		
		ZESize						GetNumberOfSections();
		
		ZEOptionSection*			GetSection(const ZEString& Name);
		ZEOptionSection*			GetSection(ZESize Index);

		ZEOption*					GetOption(const ZEString& SectionName, const ZEString& Name);

		void						Save(const ZEString& FileName);
		void						Load(const ZEString& FileName);
		void						ParseParameters(const ZEString& Parameters);
		
		void						CommitChanges();
		void						ResetChanges();

		static ZEOptionManager*		GetInstance();
};

#endif
