//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMacroFlowControl.h
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
#ifndef __ZE_MACRO_FLOW_CONTROL_H__
#define __ZE_MACRO_FLOW_CONTROL_H__

#include "ZEMacro.h"

#define ZE_MACRO_IF(Expression, TrueStatement, FalseStatement) ZE_MACRO_IF_INTERNAL(ZE_MACROL_BOOL(Expression), TrueStatement, FalseStatement)
#define ZE_MACRO_IF_INTERNAL(Expression, TrueStatement, FalseStatement) ZE_MACRO_IF_INTERNAL_#Expression(TrueStatement, FalseStatement)
#define ZE_MACRO_IF_INTERNAL_0(TrueStatement, FalseStatement) FalseStatement
#define ZE_MACRO_IF_INTERNAL_1(TrueStatement, FalseStatement) TrueStatement

#define ZE_MACRO_IF_COMMA(Expression) ZE_MACRO_IF_COMMA_INTERNAL(Expression)
#define ZE_MACRO_IF_COMMA_INTERNAL(Expression) ZE_MACRO_IF_COMMA_INTERNAL_##Expression
#define ZE_MACRO_IF_COMMA_INTERNAL_0
#define ZE_MACRO_IF_COMMA_INTERNAL_1 ,

#define ZE_MACRO_WHILE(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_0(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_0(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_1(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_1(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_2(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_2(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_3(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_3(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_4(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_4(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_5(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_5(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_6(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_6(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_7(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_7(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_8(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_8(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_9(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_9(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_10(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_10(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_11(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_11(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_12(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_12(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_13(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_13(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_14(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_14(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_15(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_15(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_16(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_16(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_17(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_17(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_18(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_18(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_19(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_19(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_20(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_20(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_21(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_21(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_22(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_22(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_23(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_23(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_24(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_24(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_25(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_25(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_26(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_26(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_27(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_27(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_28(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_28(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_29(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_29(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_30(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_30(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_31(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_31(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_32(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_32(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_33(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_33(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_34(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_34(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_35(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_35(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_36(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_36(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_37(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_37(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_38(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_38(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_39(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_39(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_40(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_40(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_41(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_41(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_42(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_42(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_43(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_43(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_44(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_44(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_45(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_45(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_46(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_46(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_47(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_47(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_48(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_48(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_49(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_49(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_50(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_50(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_51(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_51(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_52(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_52(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_53(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_53(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_54(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_54(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_55(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_55(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_56(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_56(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_57(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_57(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_58(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_58(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_59(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_59(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_60(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_60(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_61(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_61(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_62(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_62(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_63(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_63(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_64(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_64(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_65(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_65(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_66(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_66(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_67(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_67(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_68(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_68(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_69(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_69(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_70(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_70(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_71(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_71(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_72(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_72(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_73(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_73(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_74(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_74(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_75(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_75(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_76(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_76(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_77(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_77(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_78(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_78(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_79(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_79(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_80(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_80(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_81(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_81(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_82(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_82(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_83(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_83(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_84(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_84(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_85(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_85(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_86(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_86(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_87(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_87(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_88(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_88(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_89(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_89(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_90(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_90(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_91(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_91(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_92(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_92(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_93(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_93(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_94(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_94(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_95(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_95(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_96(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_96(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_97(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_97(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_98(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_98(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_99(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())
#define ZE_MACRO_WHILE_INTERNAL_99(Expression, InnerStatement) ZE_MACRO_IF(Expression, ZE_MACRO_CONCAT(ZE_MACRO_WHILE_INTERNAL_100(Expression, InnerStatement) InnerStatement), ZE_MACRO_NULL())

#endif
