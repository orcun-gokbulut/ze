//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIterator.h
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

#define ze_for_each(Variable, Container) for (auto Variable = (Container).GetIterator(); Variable.IsValid(); ++Variable)
#define ze_for_each_reverse(Variable, Container) for (auto Variable = (Container).GetIteratorEnd(); Variable.IsValid(); --Variable)

#define ze_for_each_iterator(Variable, Iterator) for (auto Variable = (Iterator); Variable.IsValid(); ++Variable)
#define ze_for_each_iterator_reverse(Variable, Iterator) for (auto Variable = (Iterator); Variable.IsValid(); --Variable)

template<typename ZEItemType>
class ZEIterator
{
	public:
		bool					IsValid() const;

		ZEItemType&				GetItem() const;
		void					Prev();
		void					Next();

		bool					operator==(const ZEIterator& Iterator) const;

		ZEIterator&				operator--();
		ZEIterator&				operator++();

		ZEItemType&				operator*() const;
		ZEItemType&				operator->() const;
};

template<typename ZEItemType>
class ZEIteratorConst
{
	public:
		bool					IsValid() const;

		const ZEItemType&		GetItem() const;
		void					Prev();
		void					Next();

		bool					operator==(const ZEIteratorConst& Iterator) const;

		ZEIteratorConst&		operator--();
		ZEIteratorConst&		operator++();

		const ZEItemType&		operator*() const;
		const ZEItemType*		operator->() const;
};
