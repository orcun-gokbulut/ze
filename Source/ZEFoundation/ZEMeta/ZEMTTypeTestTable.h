//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTTypeTestTable.h
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

//#define ZEMT_TYPE_TEST(Declaration, FunctionPostfix, Value, Type)
//#define ZEMT_TYPE_TEST_INVALID(Declaration)

#ifdef ZEMT_TYPE_TEST_INITIALIZE_VALUES



#else

#ifndef ZEMT_TYPE_TEST
#define ZEMT_TYPE_TEST(Declaration, BaseType, FunctionPostfix, TestType, Type)
#endif

#ifndef ZEMT_TYPE_TEST_INVALID
#define ZEMT_TYPE_TEST_INVALID(Declaration)
#endif

#ifndef ZEMT_TYPE_TEST_VALUE
#define ZEMT_TYPE_TEST_VALUE ZEMT_TYPE_TEST
#endif

#ifndef ZEMT_TYPE_TEST_CONST_VALUE
#define ZEMT_TYPE_TEST_CONST_VALUE ZEMT_TYPE_TEST
#endif

#ifndef ZEMT_TYPE_TEST_REFERENCE
#define ZEMT_TYPE_TEST_REFERENCE ZEMT_TYPE_TEST
#endif

#ifndef ZEMT_TYPE_TEST_CONST_REFERENCE
#define ZEMT_TYPE_TEST_CONST_REFERENCE ZEMT_TYPE_TEST
#endif

//										Declaration										Base Type										FunctionPostfix						InitialValue														Type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ZEMT_TYPE_TEST_NO_ORDINARY_TYPE
ZEMT_TYPE_TEST_VALUE(					ZEInt8,											ZEInt8,											Int8,								ZEInt8 InitialValue = -1,											ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEInt8,									ZEInt8,											Int8,								ZEInt8 InitialValue = -1,											ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEInt8&,										ZEInt8,											Int8,								ZEInt8 InitialValue = -1,											ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEInt8&,									ZEInt8,											Int8,								ZEInt8 InitialValue = -1,											ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEInt16,										ZEInt16,										Int16,								ZEInt16 InitialValue = -16500,										ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEInt16,									ZEInt16,										Int16,								ZEInt16 InitialValue = -16500,										ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEInt16&,										ZEInt16,										Int16,								ZEInt16 InitialValue = -16500,										ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEInt16&,									ZEInt16,										Int16,								ZEInt16 InitialValue = -16500,										ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEInt32,										ZEInt32,										Int32,								ZEInt32 InitialValue = -32566,										ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEInt32,									ZEInt32,										Int32,								ZEInt32 InitialValue = -32566,										ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEInt32&,										ZEInt32,										Int32,								ZEInt32 InitialValue = -32566,										ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEInt32&,									ZEInt32,										Int32,								ZEInt32 InitialValue = -32566,										ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEInt64,										ZEInt64,										Int64,								ZEInt64 InitialValue = -6465464646464654ll,							ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEInt64,									ZEInt64,										Int64,								ZEInt64 InitialValue = -6465464646464654ll,							ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEInt64&,										ZEInt64,										Int64,								ZEInt64 InitialValue = -6465464665464654ll,							ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEInt64&,									ZEInt64,										Int64,								ZEInt64 InitialValue = -6465464664654654ll,							ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEUInt8,										ZEUInt8,										UInt8,								ZEUInt8 InitialValue = 0xCE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEUInt8,									ZEUInt8,										UInt8,								ZEUInt8 InitialValue = 0xCE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEUInt8&,										ZEUInt8,										UInt8,								ZEUInt8 InitialValue = 0xCE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEUInt8&,									ZEUInt8,										UInt8,								ZEUInt8 InitialValue = 0xCE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEUInt16,										ZEUInt16,										UInt16,								ZEUInt16 InitialValue = 0xCECE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEUInt16,									ZEUInt16,										UInt16,								ZEUInt16 InitialValue = 0xCECE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEUInt16&,										ZEUInt16,										UInt16,								ZEUInt16 InitialValue = 0xCECE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEUInt16&,								ZEUInt16,										UInt16,								ZEUInt16 InitialValue = 0xCECE,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEUInt32,										ZEUInt32,										UInt32,								ZEUInt32 InitialValue = 0xCECECECE,									ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEUInt32,									ZEUInt32,										UInt32,								ZEUInt32 InitialValue = 0xCECECECE,									ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEUInt32&,										ZEUInt32,										UInt32,								ZEUInt32 InitialValue = 0xCECECECE,									ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEUInt32&,								ZEUInt32,										UInt32,								ZEUInt32 InitialValue = 0xCECECECE,									ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEUInt64,										ZEUInt64,										UInt64,								ZEUInt64 InitialValue = 0xCECECECEADADADAD,							ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEUInt64,									ZEUInt64,										UInt64,								ZEUInt64 InitialValue = 0xCECECECEADADADAD,							ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEUInt64&,										ZEUInt64,										UInt64,								ZEUInt64 InitialValue = 0xCECECECEADADADAD,							ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEUInt64&,								ZEUInt64,										UInt64,								ZEUInt64 InitialValue = 0xCECECECEADADADAD,							ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					float,											float,											Float,								float InitialValue = 0.5f,											ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const float,									float,											Float,								float InitialValue = 0.5f,											ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				float&,											float,											Float,								float InitialValue = 0.5f,											ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const float&,									float,											Float,								float InitialValue = 0.5f,											ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					double,											double,											Double,								double InitialValue = 0.5,											ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const double,									double,											Double,								double InitialValue = 0.5,											ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				double&,										double,											Double,								double InitialValue = 0.5,											ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const double&,									double,											Double,								double InitialValue = 0.5,											ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					bool,											bool,											Bool,								bool InitialValue = true,											ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const bool,										bool,											Bool,								bool InitialValue = true,											ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				bool&,											bool,											Bool,								bool InitialValue = true,											ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const bool&,									bool,											Bool,								bool InitialValue = true,											ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEString,										ZEString,										String,								ZEString InitialValue = "TestString",								ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEString,									ZEString,										String,								ZEString InitialValue = "TestString",								ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEString&,										ZEString,										String,								ZEString InitialValue = "TestString",								ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEString&,								ZEString,										String,								ZEString InitialValue = "TestString",								ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEVector2,										ZEVector2,										Vector2,							ZEVector2 InitialValue = ZEVector2::One,							ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEVector2,								ZEVector2,										Vector2,							ZEVector2 InitialValue = ZEVector2::One,							ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEVector2&,										ZEVector2,										Vector2,							ZEVector2 InitialValue = ZEVector2::One,							ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEVector2&,								ZEVector2,										Vector2,							ZEVector2 InitialValue = ZEVector2::One,							ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEVector2d,										ZEVector2d,										Vector2d,							ZEVector2d InitialValue = ZEVector2d::One,							ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEVector2d,								ZEVector2d,										Vector2d,							ZEVector2d InitialValue = ZEVector2d::One,							ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEVector2d&,									ZEVector2d,										Vector2d,							ZEVector2d InitialValue = ZEVector2d::One,							ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEVector2d&,								ZEVector2d,										Vector2d,							ZEVector2d InitialValue = ZEVector2d::One,							ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEVector3,										ZEVector3,										Vector3,							ZEVector3 InitialValue = ZEVector3::One,							ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEVector3,								ZEVector3,										Vector3,							ZEVector3 InitialValue = ZEVector3::One,							ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEVector3&,										ZEVector3,										Vector3,							ZEVector3 InitialValue = ZEVector3::One,							ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEVector3&,								ZEVector3,										Vector3,							ZEVector3 InitialValue = ZEVector3::One,							ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEVector3d,										ZEVector3d,										Vector3d,							ZEVector3d InitialValue = ZEVector3d::One,							ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEVector3d,								ZEVector3d,										Vector3d,							ZEVector3d InitialValue = ZEVector3d::One,							ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEVector3d&,									ZEVector3d,										Vector3d,							ZEVector3d InitialValue = ZEVector3d::One,							ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEVector3d&,								ZEVector3d,										Vector3d,							ZEVector3d InitialValue = ZEVector3d::One,							ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEVector4,										ZEVector4,										Vector4,							ZEVector4 InitialValue = ZEVector4::One,							ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEVector4,								ZEVector4,										Vector4,							ZEVector4 InitialValue = ZEVector4::One,							ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEVector4&,										ZEVector4,										Vector4,							ZEVector4 InitialValue = ZEVector4::One,							ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEVector4&,								ZEVector4,										Vector4,							ZEVector4 InitialValue = ZEVector4::One,							ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEVector4d,										ZEVector4d,										Vector4d,							ZEVector4d InitialValue = ZEVector4d::One,							ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEVector4d,								ZEVector4d,										Vector4d,							ZEVector4d InitialValue = ZEVector4d::One,							ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEVector4d&,									ZEVector4d,										Vector4d,							ZEVector4d InitialValue = ZEVector4d::One,							ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEVector4d&,								ZEVector4d,										Vector4d,							ZEVector4d InitialValue = ZEVector4d::One,							ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEQuaternion,									ZEQuaternion,									Quaternion,							ZEQuaternion InitialValue = ZEQuaternion::Identity,					ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEQuaternion,								ZEQuaternion,									Quaternion,							ZEQuaternion InitialValue = ZEQuaternion::Identity,					ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEQuaternion&,									ZEQuaternion,									Quaternion,							ZEQuaternion InitialValue = ZEQuaternion::Identity,					ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEQuaternion&,							ZEQuaternion,									Quaternion,							ZEQuaternion InitialValue = ZEQuaternion::Identity,					ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEMatrix3x3,									ZEMatrix3x3,									Matrix3x3,							ZEMatrix3x3 InitialValue = ZEMatrix3x3::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEMatrix3x3,								ZEMatrix3x3,									Matrix3x3,							ZEMatrix3x3 InitialValue = ZEMatrix3x3::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEMatrix3x3&,									ZEMatrix3x3,									Matrix3x3,							ZEMatrix3x3 InitialValue = ZEMatrix3x3::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEMatrix3x3&,								ZEMatrix3x3,									Matrix3x3,							ZEMatrix3x3 InitialValue = ZEMatrix3x3::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEMatrix3x3d,									ZEMatrix3x3d,									Matrix3x3d,							ZEMatrix3x3d InitialValue = ZEMatrix3x3d::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEMatrix3x3d,								ZEMatrix3x3d,									Matrix3x3d,							ZEMatrix3x3d InitialValue = ZEMatrix3x3d::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEMatrix3x3d&,									ZEMatrix3x3d,									Matrix3x3d,							ZEMatrix3x3d InitialValue = ZEMatrix3x3d::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEMatrix3x3d&,							ZEMatrix3x3d,									Matrix3x3d,							ZEMatrix3x3d InitialValue = ZEMatrix3x3d::Identity,					ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEMatrix4x4,									ZEMatrix4x4,									Matrix4x4,							ZEMatrix4x4 InitialValue = ZEMatrix4x4::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEMatrix4x4,								ZEMatrix4x4,									Matrix4x4,							ZEMatrix4x4 InitialValue = ZEMatrix4x4::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEMatrix4x4&,									ZEMatrix4x4,									Matrix4x4,							ZEMatrix4x4 InitialValue = ZEMatrix4x4::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEMatrix4x4&,								ZEMatrix4x4,									Matrix4x4,							ZEMatrix4x4 InitialValue = ZEMatrix4x4::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEMatrix4x4d,									ZEMatrix4x4d,									Matrix4x4d,							ZEMatrix4x4d InitialValue = ZEMatrix4x4d::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEMatrix4x4d,								ZEMatrix4x4d,									Matrix4x4d,							ZEMatrix4x4d InitialValue = ZEMatrix4x4d::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEMatrix4x4d&,									ZEMatrix4x4d,									Matrix4x4d,							ZEMatrix4x4d InitialValue = ZEMatrix4x4d::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEMatrix4x4d&,							ZEMatrix4x4d,									Matrix4x4d,							ZEMatrix4x4d InitialValue = ZEMatrix4x4d::Identity,					ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, NULL))
#endif

#ifndef ZEMT_TYPE_TEST_NO_IRREGULAR_TYPE
ZEMT_TYPE_TEST_VALUE(					ZETestObject,									ZETestObject,									Object,								ZEObject InitialValue = ZETestObject::Instance,						ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZETestObject,								ZETestObject,									Object,								ZEObject InitialValue = ZETestObject::Instance,						ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZETestObject&,									ZETestObject,									Object,								ZEObject InitialValue = ZETestObject::Instance,						ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZETestObject&,							ZETestObject,									Object,								ZEObject InitialValue = ZETestObject::Instance,						ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_VALUE(					ZETestObject*,									ZETestObject*,									ObjectPtr,							ZEObject* InitialValue = &ZETestObject::Instance,					ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZETestObject*,							const ZETestObject*,							ObjectPtr,							const ZEObject* InitialValue = &ZETestObject::Instance,				ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZETestObject*&,									ZETestObject*,									ObjectPtr,							ZEObject* InitialValue = &ZETestObject::Instance,					ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZETestObject*&,							const ZETestObject*,							ObjectPtr,							const ZEObject* InitialValue = &ZETestObject::Instance,				ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_VALUE(					ZEHolder<ZETestObject>,							ZEHolder<ZETestObject>,							ObjectHolder,						ZEHolder<ZEObject> InitialValue = &ZETestObject::Instance,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				ZEHolder<const ZETestObject>,					ZEHolder<const ZETestObject>,					ObjectHolder,						ZEHolder<ZEObject> InitialValue = &ZETestObject::Instance,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_CONST_VALUE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZEHolder<ZETestObject>&,						ZEHolder<ZETestObject>,							ObjectHolder,						ZEHolder<ZEObject> InitialValue = &ZETestObject::Instance,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			ZEHolder<const ZETestObject>&,					ZEHolder<const ZETestObject>,					ObjectHolder,						ZEHolder<ZEObject> InitialValue = &ZETestObject::Instance,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_CONST_REFERENCE, ZEMT_CT_NONE, ZEMT_TQ_VALUE, ZETestObject::Class()))
#endif

#ifndef ZEMT_TYPE_TEST_NO_COLLECTION_TYPE
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEInt8>,								ZEArray<ZEInt8>,								Collection,							ZEArray<ZEInt8> InitialValue = ZETestObject::ValuesInt8,											ZEMTType(ZEMT_BT_INTEGER_8, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEInt16>,								ZEArray<ZEInt16>,								Collection,							ZEArray<ZEInt16> InitialValue = ZETestObject::ValuesInt16,											ZEMTType(ZEMT_BT_INTEGER_16, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEInt32>,								ZEArray<ZEInt32>,								Collection,							ZEArray<ZEInt32> InitialValue = ZETestObject::ValuesInt32,											ZEMTType(ZEMT_BT_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEInt64>,								ZEArray<ZEInt64>,								Collection,							ZEArray<ZEInt64> InitialValue = ZETestObject::ValuesInt64,											ZEMTType(ZEMT_BT_INTEGER_64, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZEUInt8>,							ZEArray<ZEUInt8>,								Collection,							ZEArray<ZEUInt8> InitialValue = ZETestObject::ValuesUInt8,											ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_8, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZEUInt16>,						ZEArray<ZEUInt16>,								Collection,							ZEArray<ZEUInt16> InitialValue = ZETestObject::ValuesUInt16,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_16, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZEUInt32>,						ZEArray<ZEUInt32>,								Collection,							ZEArray<ZEUInt32> InitialValue = ZETestObject::ValuesUInt32,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_32, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, NULL))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZEUInt64>,						ZEArray<ZEUInt64>,								Collection,							ZEArray<ZEUInt64> InitialValue = ZETestObject::ValuesUInt64,										ZEMTType(ZEMT_BT_UNSIGNED_INTEGER_64, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<float>&,								ZEArray<float>,									Collection,							ZEArray<float> InitialValue = ZETestObject::ValuesFloat,											ZEMTType(ZEMT_BT_FLOAT, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<double>&,								ZEArray<double>,								Collection,							ZEArray<double> InitialValue = ZETestObject::ValuesDouble,											ZEMTType(ZEMT_BT_DOUBLE, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<bool>&,									ZEArray<bool>,									Collection,							ZEArray<bool> InitialValue = ZETestObject::ValuesBool,												ZEMTType(ZEMT_BT_BOOLEAN, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, NULL))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<ZEString>&,								ZEArray<ZEString>,								Collection,							ZEArray<ZEString> InitialValue = ZETestObject::ValuesString,										ZEMTType(ZEMT_BT_STRING, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZEVector2>&,						ZEArray<ZEVector2>,								Collection,							ZEArray<ZEVector2> InitialValue = ZETestObject::ValuesVector2,										ZEMTType(ZEMT_BT_VECTOR2, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZEVector2d>&,						ZEArray<ZEVector2d>,							Collection,							ZEArray<ZEVector2d> InitialValue = ZETestObject::ValuesVector2d,									ZEMTType(ZEMT_BT_VECTOR2D, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZEVector3>&,						ZEArray<ZEVector3>,								Collection,							ZEArray<ZEVector3> InitialValue = ZETestObject::ValuesVector3,										ZEMTType(ZEMT_BT_VECTOR3, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, NULL))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZEVector3d>&,						ZEArray<ZEVector3d>,							Collection,							ZEArray<ZEVector3d> InitialValue = ZETestObject::ValuesVector3d,									ZEMTType(ZEMT_BT_VECTOR3D, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEVector4>,								ZEArray<ZEVector4>,								Collection,							ZEArray<ZEVector4> InitialValue = ZETestObject::ValuesVector4,										ZEMTType(ZEMT_BT_VECTOR4, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEVector4d>,							ZEArray<ZEVector4d>,							Collection,							ZEArray<ZEVector4d> InitialValue = ZETestObject::ValuesVector4d,									ZEMTType(ZEMT_BT_VECTOR4D, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEQuaternion>,							ZEArray<ZEQuaternion>,							Collection,							ZEArray<ZEQuaternion> InitialValue = ZETestObject::ValuesQuaternion,								ZEMTType(ZEMT_BT_QUATERNION, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEMatrix3x3>,							ZEArray<ZEMatrix3x3>,							Collection,							ZEArray<ZEMatrix3x3> InitialValue = ZETestObject::ValuesMatrix3x3,									ZEMTType(ZEMT_BT_MATRIX3X3, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEMatrix3x3d>,							ZEArray<ZEMatrix3x3d>,							Collection,							ZEArray<ZEMatrix3x3d> InitialValue = ZETestObject::ValuesMatrix3x3d,								ZEMTType(ZEMT_BT_MATRIX3X3D, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEMatrix4x4>,							ZEArray<ZEMatrix4x4>,							Collection,							ZEArray<ZEMatrix4x4> InitialValue = ZETestObject::ValuesMatrix4x4,									ZEMTType(ZEMT_BT_MATRIX4X4, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEMatrix4x4d>,							ZEArray<ZEMatrix4x4d>,							Collection,							ZEArray<ZEMatrix4x4d> InitialValue = ZETestObject::ValuesMatrix4x4d,								ZEMTType(ZEMT_BT_MATRIX4X4D, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, NULL))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZETestObject>,							ZEArray<ZETestObject>,							Collection,							ZEArray<ZETestObject> InitialValue = ZETestObject::ValuesObject,									ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZETestObject>,					ZEArray<ZETestObject>,							Collection,							ZEArray<ZETestObject> InitialValue = ZETestObject::ValuesObject,									ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<ZETestObject>&,							ZEArray<ZETestObject>,							Collection,							ZEArray<ZETestObject> InitialValue = ZETestObject::ValuesObject,									ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZETestObject>&,					ZEArray<ZETestObject>,							Collection,							ZEArray<ZETestObject> InitialValue = ZETestObject::ValuesObject,									ZEMTType(ZEMT_BT_OBJECT, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZETestObject*>,							ZEArray<ZETestObject*>,							Collection,							ZEArray<ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtr,								ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_VALUE(					ZEArray<const ZETestObject*>,					ZEArray<const ZETestObject*>,					Collection,							ZEArray<const ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtrConst,						ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZETestObject*>,					ZEArray<ZETestObject*>,							Collection,							ZEArray<ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtr,								ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<const ZETestObject*>,				ZEArray<const ZETestObject*>,					Collection,							ZEArray<const ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtrConst,						ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<ZETestObject*>&,						ZEArray<ZETestObject*>,							Collection,							ZEArray<ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtr,								ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<const ZETestObject*>&,					ZEArray<const ZETestObject*>,					Collection,							ZEArray<const ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtrConst,						ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZETestObject*>&,					ZEArray<ZETestObject*>,							Collection,							ZEArray<ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtr,								ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<const ZETestObject*>&,			ZEArray<const ZETestObject*>,					Collection,							ZEArray<const ZETestObject*> InitialValue = ZETestObject::ValuesObjectPtrConst,						ZEMTType(ZEMT_BT_OBJECT_PTR, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEHolder<ZETestObject>>,				ZEArray<ZEHolder<ZETestObject>>,				Collection,							ZEArray<ZEHolder<ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolder,					ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_VALUE(					ZEArray<ZEHolder<const ZETestObject>>,			ZEArray<ZEHolder<const ZETestObject>>,			Collection,							ZEArray<ZEHolder<const ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolderConst,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZEHolder<ZETestObject>>,			ZEArray<ZEHolder<ZETestObject>>,				Collection,							ZEArray<ZEHolder<ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolder,					ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_VALUE(				const ZEArray<ZEHolder<const ZETestObject>>,	ZEArray<ZEHolder<const ZETestObject>>,			Collection,							ZEArray<ZEHolder<const ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolderConst,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_VALUE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<ZEHolder<ZETestObject>>&,				ZEArray<ZEHolder<ZETestObject>>,				Collection,							ZEArray<ZEHolder<ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolder,					ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_REFERENCE(				ZEArray<ZEHolder<const ZETestObject>>&,			ZEArray<ZEHolder<const ZETestObject>>,			Collection,							ZEArray<ZEHolder<const ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolderConst,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZEHolder<ZETestObject>>&,			ZEArray<ZEHolder<ZETestObject>>,				Collection,							ZEArray<ZEHolder<ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolder,					ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, ZETestObject::Class()))
ZEMT_TYPE_TEST_CONST_REFERENCE(			const ZEArray<ZEHolder<const ZETestObject>>&,	ZEArray<ZEHolder<const ZETestObject>>,			Collection,							ZEArray<ZEHolder<const ZETestObject>> InitialValue = ZETestObject::ValuesObjectHolderConst,			ZEMTType(ZEMT_BT_OBJECT_HOLDER, ZEMT_TQ_CONST_VALUE, ZEMT_CT_ARRAY, ZEMT_TQ_CONST_REFERENCE, ZETestObject::Class()))
#endif

/*ZEMT_TYPE_TEST_INVALID(ZEDummyNonObject);
ZEMT_TYPE_TEST_INVALID(const ZEDummyNonObject);
ZEMT_TYPE_TEST_INVALID(ZEDummyNonObject&);
ZEMT_TYPE_TEST_INVALID(const ZEDummyNonObject&);
ZEMT_TYPE_TEST_INVALID(ZEDummyNonObject*);
ZEMT_TYPE_TEST_INVALID(const ZEDummyNonObject*);
ZEMT_TYPE_TEST_INVALID(ZEDummyNonObject*&);
ZEMT_TYPE_TEST_INVALID(const ZEDummyNonObject*&);*/
ZEMT_TYPE_TEST_INVALID(ZEString*);
ZEMT_TYPE_TEST_INVALID(const ZEString*);
ZEMT_TYPE_TEST_INVALID(const ZEString**);
ZEMT_TYPE_TEST_INVALID(ZEString* const*);
ZEMT_TYPE_TEST_INVALID(const ZEString* const*);
ZEMT_TYPE_TEST_INVALID(ZETestObject**);
ZEMT_TYPE_TEST_INVALID(const ZETestObject**);
ZEMT_TYPE_TEST_INVALID(ZETestObject* const*);
ZEMT_TYPE_TEST_INVALID(const ZETestObject* const);
ZEMT_TYPE_TEST_INVALID(ZEHolder<ZEInt8>);
ZEMT_TYPE_TEST_INVALID(ZEHolder<ZEUInt32>);
ZEMT_TYPE_TEST_INVALID(ZEHolder<ZEUInt64>);
ZEMT_TYPE_TEST_INVALID(ZEHolder<ZEString>);
ZEMT_TYPE_TEST_INVALID(ZEHolder<ZEMatrix4x4d>);
ZEMT_TYPE_TEST_INVALID(ZEHolder<ZETestObject>*);
ZEMT_TYPE_TEST_INVALID(ZEHolder<const ZETestObject>*);
ZEMT_TYPE_TEST_INVALID(const ZEHolder<ZETestObject>*);
ZEMT_TYPE_TEST_INVALID(const ZEHolder<const ZETestObject>*);
ZEMT_TYPE_TEST_INVALID(const ZEHolder<ZETestObject>);
ZEMT_TYPE_TEST_INVALID(const ZEHolder<const ZETestObject>);
ZEMT_TYPE_TEST_INVALID(const ZEHolder<ZETestObject>&);
ZEMT_TYPE_TEST_INVALID(const ZEHolder<const ZETestObject>&);
ZEMT_TYPE_TEST_INVALID(ZEArray<ZEUInt32*>);
ZEMT_TYPE_TEST_INVALID(ZEArray<ZEString*>);
ZEMT_TYPE_TEST_INVALID(ZEArray<ZEMatrix4x4*>);
ZEMT_TYPE_TEST_INVALID(ZEArray<bool*>);
ZEMT_TYPE_TEST_INVALID(ZEArray<const float*>);
ZEMT_TYPE_TEST_INVALID(ZEArray<const double*>);
#endif
