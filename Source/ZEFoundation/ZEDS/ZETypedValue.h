//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETypedValue.h
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

///////////////////////////////////////////////////////////////
//  Orcun's Data Structres Version 2.0 (Zinek Engine)        //
// --------------------------------------------------------- //
//  Copyrights (C) 2005-2007,  Y. Orçun GÖKBULUT			 //
//  All rights reserved.									 //
///////////////////////////////////////////////////////////////

#pragma once
#ifndef __ZE_TYPED_VARIANT_H__
#define __ZE_TYPED_VARIANT_H__

#pragma warning(push)
#pragma warning(disable:4482)

#include "ZEValue.h"
#include "ZETypes.h"

class ZETypedValue : public ZEValue
{
	public:
		void		SetString		(char *NewValue);
		void		SetInteger		(ZEInt NewValue);
		void		SetFloat		(float NewValue);
		void		SetBoolean		(bool NewValue);
		void		SetValue		(const ZEValue& NewValue);
		void		SetTypedValue	(const ZETypedValue& NewValue);

		void		operator=		(ZEValue& NewValue);
		void		operator=		(ZETypedValue& NewValue);
		void		operator=		(char* NewValue);
		void		operator=		(ZEInt NewValue);
		void		operator=		(float NewValue);
		void		operator=		(bool NewValue);


					ZETypedValue	();
					ZETypedValue	(ZETypedValue& InitialType);
					ZETypedValue	(ZEValue& InitialValue);
					ZETypedValue	(char* InitialValue);	
					ZETypedValue	(ZEInt InitialValue);
					ZETypedValue	(float InitialValue);
					ZETypedValue	(bool InitialValue);

					~ZETypedValue();
				
};

#pragma warning(pop)
#endif
