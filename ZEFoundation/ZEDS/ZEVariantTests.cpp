//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVariantTests.cpp
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

#include "ZEVariant.h"
#include "ZETest.h"
#include "ZEString.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEMath\ZEMatrix.h"
#include "ZEMath\ZEVector.h"
#include "ZETypedVariant.h"
#include "ZEFile\ZEFile.h"


ZETestSuite(ZEVariant)
{
	ZETest(SetTypeGetTypeSizeOfGetValue)
	{
		ZEVariant TestItem1;
		ZEMatrix4x4 Matrix;
		Matrix.M11 = 3;
		Matrix.M12 = 3;
		Matrix.M13 = 3;
		Matrix.M14 = 3;
		Matrix.M21 = 3;

		ZEQuaternion Quaternion;
		ZEQuaternion::CreateFromMatrix(Quaternion,Matrix);
		TestItem1.SetQuaternion(Quaternion);
		ZETestCheck(Quaternion == TestItem1.GetQuaternion());
		ZETestCheckEqual(TestItem1.SizeOf(), sizeof(ZEQuaternion));


		/*ZEVariant TestItem2;
		ZEString String1;
		String1 = "cCc";
		TestItem2.SetString("cCc");
		CHECK(String1 == TestItem2.GetString() );
		CHECK(String1.GetSize() == TestItem2.SizeOf());
		TestItem2.GetValue();*/

		ZEVariant TestItem3;
		int integer;
		integer = 3;
		TestItem3.SetInteger(3);
		ZETestCheck(integer == TestItem3.GetInteger());
		ZETestCheckEqual(TestItem3.SizeOf(), sizeof(int));

		ZEVariant TestItem4;
		float Fl;
		Fl = 3.0;
		TestItem4.SetFloat(3.0);
		ZETestCheck(Fl == TestItem4.GetFloat());
		ZETestCheckEqual(TestItem4.SizeOf(), sizeof(float));

		ZEVariant TestItem5;
		ZEVector2 Vector2;
		Vector2.x = 1.0;
		Vector2.y = 2.0;
		TestItem5.SetVector2(Vector2);
		ZETestCheck(Vector2 == TestItem5.GetVector2());
		ZETestCheckEqual(TestItem5.SizeOf(), sizeof(ZEVector2));

		ZEVariant TestItem6;
		ZEVector3 Vector3;
		Vector3.x = 1.0;
		Vector3.y = 2.0;
		Vector3.z = 3.0;
		TestItem6.SetVector3(Vector3);
		ZETestCheck(Vector3 == TestItem6.GetVector3());
		ZETestCheckEqual(TestItem6.SizeOf(), sizeof(ZEVector3));

		ZEVariant TestItem7;
		ZEVector4 Vectors4(1.0,2.0,3.0,1.0);
		TestItem7.SetVector4(Vectors4);
		ZETestCheck(Vectors4 == TestItem7.GetVector4());
		ZETestCheckEqual(TestItem7.SizeOf(), sizeof(ZEVector4));

		ZEVariant TestItem8;
		ZEMatrix3x3 Matrix3;
		Matrix3.M11 = 1;
		Matrix3.M12 = 2;
		Matrix3.M13 = 3;
		Matrix3.M21 = 1;
		Matrix3.M22 = 2;
		Matrix3.M23 = 3;
		TestItem8.SetMatrix3x3(Matrix3);
		ZETestCheck(Matrix3 == TestItem8.GetMatrix3x3());
		ZETestCheckEqual(TestItem8.SizeOf(), sizeof(ZEMatrix3x3));

		ZEVariant TestItem9;
		ZEMatrix4x4 Matrix4;
		Matrix4.M11 = 1;
		Matrix4.M12 = 2;
		Matrix4.M13 = 3;
		Matrix4.M14 = 4;
		Matrix4.M21 = 1;
		Matrix4.M22 = 2;
		Matrix4.M23 = 3;
		TestItem9.SetMatrix4x4(Matrix4);
		ZETestCheck(Matrix4 == TestItem9.GetMatrix4x4());
		ZETestCheckEqual(TestItem9.SizeOf(), sizeof(ZEMatrix4x4));
		
		ZEVariant TestItem11;
		ZETypedVariant Typedvariant;
		Typedvariant.SetInteger(5);
		ZETestCheck(5 == Typedvariant.GetInteger());
		Typedvariant.GetValue();
		

	}

	ZETest(operators)
	{	//ZEString 
		/*ZEVariant TestItem1;
		ZEString String1;
		String1 = "cCc";
		TestItem1.SetString("cCc");
		CHECK(String1 = TestItem1.GetString());
		TestItem1 = "EGEMENBASKAN";*/

		//= int
		ZEVariant TestItem2;
		TestItem2 = 9;
		//= float
		ZEVariant TestItem3;
		TestItem3 = 9.0f;

		//= v3
		ZEVariant TestItem4;
		ZEVector3 Vector3;
		Vector3.x = 1.0;
		Vector3.y = 2.0;
		Vector3.z = 3.0;
		TestItem4 = Vector3;

		//= v4
		ZEVariant TestItem5;
		ZEVector4 Vectors4(1.0,2.0,3.0,1.0);
		TestItem5 = Vectors4;

		//= v2
		ZEVariant TestItem6;
		ZEVector2 Vector2;
		Vector2.x = 1.0;
		Vector2.y = 2.0;
		TestItem6 = Vector2;

		//= quaternion
		ZEVariant TestItem7;
		ZEQuaternion Quternion;
		TestItem7 = Quternion;

		//= matrix4
		ZEVariant TestItem8;
		ZEMatrix4x4 Matrix4;
		TestItem8 = Matrix4;

		//= matrix3
		ZEVariant TestItem9;
		ZEMatrix3x3 Matrix3;
		TestItem9 = Matrix3;
		
	}

	ZETest(SerializeUnserialize)
	{	//Matrix3
		size_t LastItem;
		ZEFile File;
		File.Open("ZEVariantTests.txt", ZE_FM_READ_WRITE, true);
		ZEVariant TestItem1;
		ZEMatrix3x3 Matrix3;
		Matrix3.M11 = 1;
		Matrix3.M12 = 2;
		Matrix3.M13 = 3;
		TestItem1 = Matrix3;
		LastItem = File.Tell();
		TestItem1.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem2;
		
		TestItem2.Unserialize(&File);
		ZETestCheckEqual(TestItem1.GetMatrix3x3(), TestItem2.GetMatrix3x3());
		//Matrix4
		ZEVariant TestItem3;
		ZEMatrix4x4 Matrix4;
		Matrix4.M11 = 1;
		Matrix4.M12 = 2;
		Matrix4.M13 = 3;
		Matrix4.M14 = 4;
		TestItem3 = Matrix4;
		LastItem = File.Tell();
		TestItem3.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem4;
		TestItem4.Unserialize(&File);
		ZETestCheckEqual(TestItem4.GetMatrix4x4(), TestItem3.GetMatrix4x4());
		//Int
		ZEVariant TestItem5;
		int a;
		a = 9;
		TestItem5 = a;
		LastItem = File.Tell();
		TestItem5.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem6;
		TestItem6.Unserialize(&File);
		ZETestCheckEqual(TestItem5.GetInteger(), TestItem6.GetInteger());
		//vector2
		ZEVariant TestItem7;
		ZEVector2 Vector2(1.0, 2.0);
		TestItem7 = Vector2;
		LastItem = File.Tell();
		TestItem7.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem8;
		TestItem8.Unserialize(&File);
		ZETestCheckEqual(TestItem8.GetVector2(), TestItem7.GetVector2());
		//Vector3
		ZEVariant TestItem9;
		ZEVector3 Vector3(1.0, 2.0, 3.0);
		TestItem9 = Vector3;
		LastItem = File.Tell();
		TestItem9.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem10;
		TestItem10.Unserialize(&File);
		ZETestCheckEqual(TestItem10.GetVector3(), TestItem9.GetVector3());
		//Vector4
		ZEVariant TestItem11;
		ZEVector4 Vector4(1.0, 2.0, 3.0, 4.0);
		TestItem11 = Vector4;
		LastItem = File.Tell();
		TestItem11.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem12;
		TestItem12.Unserialize(&File);
		ZETestCheckEqual(TestItem12.GetVector4(), TestItem11.GetVector4());
		//Quaternion
		ZEVariant TestItem13;
		ZEQuaternion Quaternion;
		ZEMatrix4x4 Matrix;
		Matrix.M11 = 3;
		Matrix.M12 = 3;
		Matrix.M13 = 3;
		Matrix.M14 = 3;
		Matrix.M21 = 3;
		ZEQuaternion::CreateFromMatrix(Quaternion,Matrix);
		TestItem13.SetQuaternion(Quaternion);
		LastItem = File.Tell();
		TestItem13.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem14;
		TestItem14.Unserialize(&File);
		ZETestCheckEqual(TestItem14.GetQuaternion(), TestItem13.GetQuaternion());
		//float
		ZEVariant TestItem15;
		float Float;
		Float = 9.0f;
		TestItem15 = Float;
		LastItem = File.Tell();
		TestItem15.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem16;
		TestItem16.Unserialize(&File);
		ZETestCheckEqual(TestItem16.GetFloat(), TestItem15.GetFloat());
		/*ZEString
		ZEVariant TestItem17;
		ZEString String;
		String = "YowYow";
		TestItem17 = String;
		LastItem = File.Tell();
		TestItem17.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem18;
		TestItem18.Unserialize(&File);
		CHECK_EQUAL(TestItem18.GetString(), TestItem17.GetString());*/
		/*//NULL
		ZEVariant TestItem19;
		ZEString String1 = NULL;
		TestItem19 = String1;
		LastItem = File.Tell();
		TestItem19.Serialize(&File);
		File.Seek(LastItem, ZE_SF_BEGINING);
		ZEVariant TestItem20;
		TestItem20.Unserialize(&File);
		CHECK_EQUAL(TestItem20.GetString(), TestItem19.GetString());*/

	}

	
}
