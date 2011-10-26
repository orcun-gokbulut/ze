//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDelegate.h
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

#ifndef __ZE_DELEGATE_H__
#define __ZE_DELEGATE_H__

template <typename TSignature> 
class ZEDelegate;

#define ArgumentSeperator

#define DelegateName ZEDelegate0
#define TemplateArgumentDefinitions
#define TemplateArguments
#define ArgumentDefinitions
#define Arguments
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinition
#undef TemplateArguments
#undef ArgumentDefinition
#undef Arguments	

#undef ArgumentSeperator

#define ArgumentSeperator			,

#define DelegateName				ZEDelegate1
#define TemplateArgumentDefinitions	typename TArg0
#define TemplateArguments			TArg0 
#define ArgumentDefinitions			TArg0 Arg0
#define Arguments					Arg0
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments

#define DelegateName				ZEDelegate2
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1
#define TemplateArguments			TArg0, TArg1 
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1
#define Arguments					Arg0, Arg1
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	


#define DelegateName				ZEDelegate3
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2
#define TemplateArguments			TArg0, TArg1, TArg2
#define Arguments					Arg0, Arg1, Arg2
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	


#define DelegateName				ZEDelegate4
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2, typename TArg3
#define TemplateArguments			TArg0, TArg1, TArg2, TArg3
#define Arguments					Arg0, Arg1, Arg2, Arg3
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2, TArg3 Arg3
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	


#define DelegateName				ZEDelegate5
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4
#define TemplateArguments			TArg0, TArg1, TArg2, TArg3, TArg4
#define Arguments					Arg0, Arg1, Arg2, Arg3, Arg4
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2, TArg3 Arg3, TArg4 Arg4
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	


#define DelegateName				ZEDelegate6
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5
#define TemplateArguments			TArg0, TArg1, TArg2, TArg3, TArg4, TArg5
#define Arguments					Arg0, Arg1, Arg2, Arg3, Arg4, Arg5
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2, TArg3 Arg3, TArg4 Arg4, TArg5 Arg5
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	

#define DelegateName				ZEDelegate7
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6
#define TemplateArguments			TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6
#define Arguments					Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2, TArg3 Arg3, TArg4 Arg4, TArg5 Arg5, TArg6 Arg6
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	


#define DelegateName				ZEDelegate8
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7
#define TemplateArguments			TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7
#define Arguments					Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2, TArg3 Arg3, TArg4 Arg4, TArg5 Arg5, TArg6 Arg6, TArg7 Arg7
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	


#define DelegateName				ZEDelegate9
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8
#define TemplateArguments			TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8
#define Arguments					Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2, TArg3 Arg3, TArg4 Arg4, TArg5 Arg5, TArg6 Arg6, TArg7 Arg7, TArg8 Arg8
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	


#define DelegateName				ZEDelegate10
#define TemplateArgumentDefinitions	typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8, typename TArg9
#define TemplateArguments			TArg0, TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8, TArg9
#define Arguments					Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9
#define ArgumentDefinitions			TArg0 Arg0, TArg1 Arg1, TArg2 Arg2, TArg3 Arg3, TArg4 Arg4, TArg5 Arg5, TArg6 Arg6, TArg7 Arg7, TArg8 Arg8, TArg9 Arg9
#include "ZEDelegateImp.h"
#undef DelegateName
#undef TemplateArgumentDefinitions
#undef TemplateArguments
#undef ArgumentDefinitions
#undef Arguments	

#undef ArgumentSeperator

#endif
