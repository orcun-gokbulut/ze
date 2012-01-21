//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrix3x3Tests.cpp
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



#include "ZETest.h"
#include <d3dx9.h>
#include <math.h>
#include "ZEMath/ZEAngle.h"
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMath.h"

bool ZETestInternalCheckClose(const ZEMatrix3x3& Actual, const ZEMatrix3x3& Expected, const ZEMatrix3x3& Threshold = ZEMatrix3x3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD))
{	
	return(ZETestInternalCheckClose(Actual.M11, Expected.M11, Threshold.M11) && ZETestInternalCheckClose(Actual.M12, Expected.M12, Threshold.M12) && ZETestInternalCheckClose(Actual.M13, Expected.M13, Threshold.M13) && ZETestInternalCheckClose(Actual.M21, Expected.M21, Threshold.M21) && ZETestInternalCheckClose(Actual.M22, Expected.M22, Threshold.M22) && ZETestInternalCheckClose(Actual.M23, Expected.M23, Threshold.M23) && ZETestInternalCheckClose(Actual.M31, Expected.M31, Threshold.M31) && ZETestInternalCheckClose(Actual.M32, Expected.M32, Threshold.M32) && ZETestInternalCheckClose(Actual.M33, Expected.M33, Threshold.M33));
}

ZETestSuite(ZEMatrix3x3)
{
	ZETest("ZEMatrix3x3::ZEMatrix3x3(float M00, float M01, float M02, float M10, float M11, float M12, float M20, float M21, float M22)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		ZETestCheckEqual(Matrix, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}
	ZETest("ZEMatrix3x3::ZEMatrix3x3(float M[9])")
	{
		float M[9]={1.0f, 3.0f, 13.0f, 1.0f, 5.0f, 21.0f, 2.0f,  8.0f,  34.0f};
		ZEMatrix3x3 Matrix(M);
		ZETestCheckEqual(Matrix, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}
	ZETest("inline void ZEMatrix3x3::Add(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B( 1.0f,  2.0f,  3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Add(Out, A, B);
		ZETestCheckClose(Out, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f));
	}
	ZETest("inline void ZEMatrix3x3::Create(ZEMatrix3x3& Matrix, float M[9])")
	{
		ZEMatrix3x3 Matrix;
		float M[9]={34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f};

		ZEMatrix3x3::Create(Matrix, M);
		ZETestCheckEqual(Matrix, ZEMatrix3x3(34.0f, 8.0f, 2.0f, 21.0f, 5.0f, 1.0f, 13.0f, 3.0f, 1.0f));
	}
	ZETest("inline void ZEMatrix3x3::Create(ZEMatrix3x3 &Matrix, float M11, float M12, float M13, float M21, float M22, float M23, float M31, float M32, float M33)")
	{
		ZEMatrix3x3 Matrix;

		ZEMatrix3x3::Create(Matrix,
			1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);
		ZETestCheckEqual(Matrix, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}
	ZETest("void ZEMatrix3x3::CreateIdentity(ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix;
		
		ZEMatrix3x3::CreateIdentity(Matrix);
		ZETestCheckEqual(Matrix, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}
	ZETest("void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, const ZEQuaternion& Rotation)")
	{
		ZEQuaternion Rotation(ZE_PI_4, ZEVector3::UnitY);
		ZEMatrix3x3 Matrix;

		ZEMatrix3x3::CreateRotation(Matrix, Rotation);
		ZETestCheckClose(Matrix, ZEMatrix3x3(0.70710673f, 0.0f, 0.70710680f, 0.0f, 1.0f, 0.0f, -0.70710680f, 0.0f, 0.70710673f));
	}
	ZETest("void ZEMatrix3x3::CreateRotationX(ZEMatrix3x3& Matrix, float Pitch)")
	{
		ZEMatrix3x3 Matrix;
		float Pitch = ZE_PI_4;
		
		ZEMatrix3x3::CreateRotationX(Matrix, Pitch);
		ZETestCheckClose(Matrix, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 0.70710677f, -0.70710677f, 0.0f, 0.70710677f, 0.70710677f));
	}
	ZETest("void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder)")
	{
		ZEMatrix3x3 Matrix;
		float Pitch = ZE_PI;
		float Yawn = ZE_PI_2;
		float Roll = ZE_PI_4;
		
		ZETestCase("for RotationOrder ZE_RO_XYZ")
		{
			ZERotationOrder RotationOrder = ZE_RO_XYZ;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZETestCheckClose(Matrix, ZEMatrix3x3(0.0f, 0.0f, 1.0f, -0.70710678f, -0.70710678f, 0.0f, 0.70710678f, -0.70710678f, 0.0f));
		}
		ZETestCase("for RotationOrder ZE_RO_XZY")
		{
			ZERotationOrder RotationOrder = ZE_RO_XZY;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZETestCheckClose(Matrix, ZEMatrix3x3(0.0f, -0.70710678f, 0.70710678f, 0.0f, -0.70710678f, -0.70710678f, 1.0f, 0.0f, 0.0f));
		}
		ZETestCase("for RotationOrder ZE_RO_YXZ")
		{
			ZERotationOrder RotationOrder = ZE_RO_YXZ;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZETestCheckClose(Matrix, ZEMatrix3x3(0.0f, 0.0f, -1.0f, -0.70710678f, -0.70710678f, 0.0f, -0.70710678f, 0.70710678f, 0.0f));
		}
		ZETestCase("for RotationOrder ZE_RO_YZX")
		{
			ZERotationOrder RotationOrder = ZE_RO_YZX;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZETestCheckClose(Matrix, ZEMatrix3x3(0.0f, 0.0f, -1.0f, 0.70710678f, -0.70710678f, 0.0f, -0.70710678f, -0.70710678f, 0.0f));
		}
		ZETestCase("for RotationOrder ZE_RO_ZXY")
		{
			ZERotationOrder RotationOrder = ZE_RO_ZXY;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZETestCheckClose(Matrix, ZEMatrix3x3(0.0f, 0.70710678f, 0.70710678f, 0.0f, -0.70710678f, 0.70710678f, 1.0f, 0.0f, 0.0f));
		}
		ZETestCase("for RotationOrder ZE_RO_ZYX")
		{
			ZERotationOrder RotationOrder = ZE_RO_ZYX;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZETestCheckClose(Matrix, ZEMatrix3x3(0.0f, 0.70710678f, -0.70710678f, 0.0f, -0.70710678f, -0.70710678f, -1.0f, 0.0f, 0.0f));
		}
	}
	ZETest("void ZEMatrix3x3::CreateRotationY(ZEMatrix3x3& Matrix, float Yawn)")
	{
		ZEMatrix3x3 Matrix;
		float Yawn = ZE_PI_4;

		ZEMatrix3x3::CreateRotationY(Matrix, Yawn);
		ZETestCheckClose(Matrix, ZEMatrix3x3(0.70710677f, 0.0f, 0.70710677f, 0.0f, 1.0f, 0.0f, -0.70710677f, 0.0f, 0.70710677f));
	}
	ZETest("void ZEMatrix3x3::CreateRotationZ(ZEMatrix3x3& Matrix, float Roll)")
	{
		ZEMatrix3x3 Matrix;
		float Roll = ZE_PI_4;

		ZEMatrix3x3::CreateRotationZ(Matrix, Roll);
		ZETestCheckClose(Matrix, ZEMatrix3x3(0.70710677f, -0.70710677f, 0.0f, 0.70710677f, 0.70710677f, 0.0f, 0.0f, 0.0f, 1.0f));
	}
	ZETest("void ZEMatrix3x3::CreateScale(ZEMatrix3x3& Matrix, float x, float y, float z)")
	{
		ZEMatrix3x3 Matrix;
		
		ZEMatrix3x3::CreateScale(Matrix, 1.0f, 2.0f, 3.0f);
		ZETestCheckEqual(Matrix, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 3.0f));
	}
	ZETest("void ZEMatrix3x3::CreateZero(ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix;
		
		ZEMatrix3x3::CreateZero(Matrix);
		ZETestCheckEqual(Matrix, ZEMatrix3x3(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	ZETest("float ZEMatrix3x3::Determinant(const ZEMatrix3x3 &Matrix)")
	{
		ZEMatrix3x3 Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		
		float Determinant = ZEMatrix3x3::Determinant(Matrix);
		ZETestCheckClose(Determinant, 0.0f);
	}
	ZETest("float ZEMatrix3x3::Determinant() const")
	{
		ZEMatrix3x3 Matrix(1.0f, 2.0f, -5.0f,
			4.0f, 2.0f, 1.0f,
			3.0f, 2.0f, 9.0f);

		float Determinant = Matrix.Determinant();
		ZETestCheckClose(Determinant, -60.0f);
	}
	ZETest("bool ZEMatrix3x3::Inverse(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix)")
	{
		ZEMatrix3x3 Matrix(2.0f, 1.0f, 1.0f,
			3.0f, 2.0f, 1.0f,
			4.0f, -3.0f, 5.0f);

		ZEMatrix3x3 Out;

		bool Result = ZEMatrix3x3::Inverse(Out, Matrix);
		ZETestCheck(Result == true);
		ZETestCheckClose(Out, ZEMatrix3x3(-6.5f, 4.0f, 0.5f, 5.5f, -3.0f, -0.5f, 8.5f, -5.0f, -0.5f));
		
		ZETestCase("for Determinant=0")
		{
			ZEMatrix3x3 Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
			ZEMatrix3x3 Out;

			bool Result = ZEMatrix3x3::Inverse(Out, Matrix);
			ZETestCheck(Result == false);
		}
	}
	ZETest("ZEMatrix3x3 ZEMatrix3x3::Inverse() const")
	{
		ZEMatrix3x3 Matrix;
		ZEMatrix3x3::CreateIdentity(Matrix);
		
		ZEMatrix3x3 Out = Matrix.Inverse();
		ZETestCheckEqual(Out, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}
	ZETest("void ZEMatrix3x3::InverseSelf()")
	{
		ZEMatrix3x3 Matrix(2.0f, 1.0f, 1.0f,
			3.0f, 2.0f, 1.0f,
			4.0f, -3.0f, 5.0f);

		Matrix.InverseSelf();
		ZETestCheckClose(Matrix, ZEMatrix3x3(-6.5f, 4.0f, 0.5f, 5.5f, -3.0f, -0.5f, 8.5f, -5.0f, -0.5f));
	}
	ZETest("inline void ZEMatrix3x3::Multiply(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)")
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 Out;
		
		ZEMatrix3x3::Multiply(Out, A, B);
		ZETestCheckClose(Out, ZEMatrix3x3(312.0f, 486.0f, 798.0f, 159.0f, 251.0f, 410.0f, 166.0f, 264.0f, 430.0f));

		ZETestCase("Multiply by Identity Matrix")
		{
			ZEMatrix3x3 Out;
			ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f,  9.0f,  9.0f, 10.0f);
			ZEMatrix3x3 Identity;
			ZEMatrix3x3::CreateIdentity(Identity);

			ZEMatrix3x3::Multiply(Out, Matrix, Identity);
			ZETestCheckEqual(Out, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f,  9.0f,  9.0f, 10.0f));
		}
	}
	ZETest("bool ZEMatrix3x3::operator != (const ZEMatrix3x3 &M) const")
	{
		ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		bool Result = (A != B);
		ZETestCheck(Result == false);

		ZETestCase("!= WithMatrixArg for Comparison for Different Matrices")
		{
			ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f);

			ZEMatrix3x3 B(5.0f, 1.0f, 3.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 3.0f);

			bool Result = (A != B);
			ZETestCheck(Result == true);
		}
	}
	ZETest("ZEMatrix3x3 ZEMatrix3x3::operator-(const ZEMatrix3x3 &RightOperand) const")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Result;

		Result = A - B;
		ZETestCheckClose(Result, ZEMatrix3x3(33.0f, 19.0f, 10.0f, 4.0f, 0.0f, -3.0f, -5.0f, -7.0f, -8.0f));
	}
	ZETest("ZEMatrix3x3 ZEMatrix3x3::operator*(const ZEMatrix3x3 &RightOperand) const ")
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 Result = A * B;
		ZETestCheckClose(Result, ZEMatrix3x3(312.0f, 486.0f, 798.0f, 159.0f, 251.0f, 410.0f, 166.0f, 264.0f, 430.0f));
	}
	ZETest("ZEVector2 ZEMatrix3x3::operator*(const ZEVector2 &RightOperand) const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector2 Vector(1.0f, 2.0f);

		ZEVector2 Result = Matrix * Vector;
		ZETestCheckEqual(Result, ZEVector2(97.0f, 41.0f));
	}
	ZETest("ZEVector3 ZEMatrix3x3::operator*(const ZEVector3 &RightOperand) const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZEVector3 Result = Matrix * Vector;
		ZETestCheckEqual(Result, ZEVector3(129.0f, 59.0f, 57.0f));
	}
	ZETest("ZEVector4 ZEMatrix3x3::operator*(const ZEVector4 &RightOperand) const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		ZEVector4 Result = Matrix * Vector;
		ZETestCheckEqual(Result, ZEVector4(129.0f, 59.0f, 57.0f, 1.0f));
	}
	ZETest("ZEMatrix3x3 ZEMatrix3x3::operator*(float S)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float S = 2.0f;

		ZEMatrix3x3 Result = Matrix * S;
		ZETestCheckClose(Result, ZEMatrix3x3(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f));
	}
	ZETest("ZEMatrix3x3& ZEMatrix3x3::operator *= (const ZEMatrix3x3 &RightOperand)")
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		A *= B;
		ZETestCheckClose(A, ZEMatrix3x3(312.0f, 486.0f, 798.0f, 159.0f, 251.0f, 410.0f, 166.0f, 264.0f, 430.0f));
	}
	ZETest("ZEMatrix3x3& ZEMatrix3x3::operator *= (float S)")
	{
		ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float S = 2.0f;

		A *= S;
		ZETestCheckClose(A, ZEMatrix3x3(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f));
	}
	ZETest("float& ZEMatrix3x3::operator[](ZESize Index)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float M11 = Matrix[0];
		float M21 = Matrix[1];
		float M31 = Matrix[2];
		ZETestCheckEqual(M11, 1.0f);
		ZETestCheckEqual(M21, 3.0f);
		ZETestCheckEqual(M31, 13.0f);
	}
	ZETest("float ZEMatrix3x3::operator[](ZESize Index) const")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float M12 = Matrix[3];
		float M21 = Matrix[1];
		float M32 = Matrix[5];
		ZETestCheckEqual(M12, 1.0f);
		ZETestCheckEqual(M21, 3.0f);
		ZETestCheckEqual(M32, 21.0f);
	}
	ZETest("ZEMatrix3x3 ZEMatrix3x3::operator+(const ZEMatrix3x3 &RightOperand) const")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f,  2.0f,  3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Result = A + B;
		ZETestCheckClose(Result, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f));	
	}
	ZETest("ZEMatrix3x3& ZEMatrix3x3::operator += (const ZEMatrix3x3 &RightOperand)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		A += B;
		ZETestCheckClose(A, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f));
	}
	ZETest("ZEMatrix3x3& ZEMatrix3x3::operator -= (const ZEMatrix3x3 &RightOperand)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		A -= B;
		ZETestCheckClose(A, ZEMatrix3x3(33.0f, 19.0f, 10.0f, 4.0f, 0.0f, -3.0f, -5.0f, -7.0f, -8.0f));
	}
	ZETest("bool ZEMatrix3x3::operator == (const ZEMatrix3x3 &M) const")
	{
		ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		bool Result = (A == B);
		ZETestCheck(Result == true);

		ZETestCase("== WithMatrixArg for Comparison for Different Matrices")
		{
			ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f);

			ZEMatrix3x3 B(5.0f, 1.0f, 3.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 3.0f);

			bool Result = (A == B);
			ZETestCheck(Result == false);
		}
	}
	ZETest("inline void ZEMatrix3x3::Scale(ZEMatrix3x3 &Out, const ZEMatrix3x3& A, float s)")
	{
		ZEMatrix3x3 Matrix(1.0f, 0.0f, 1.5f,
			3.0f, 5.0f, 8.0f,
			13.0f, 7.0f, 14.0f);

		ZEMatrix3x3 Out;
		float s = 2.0f;

		ZEMatrix3x3::Scale(Out, Matrix, s);
		ZETestCheckClose(Out, ZEMatrix3x3(2.0f, 0.0f, 3.0f, 6.0f, 10.0f, 16.0f, 26.0f, 14.0f, 28.0f));
	}
	ZETest("inline void ZEMatrix3x3::Sub(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f,  2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Sub(Out, A, B);
		ZETestCheckClose(Out, ZEMatrix3x3(33.0f, 19.0f, 10.0f, 4.0f, 0.0f, -3.0f, -5.0f, -7.0f, -8.0f));
	}
	ZETest("void ZEMatrix3x3::Transform(ZEVector2& Out, const ZEMatrix3x3& Matrix, const ZEVector2& Vector)")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector2 Out;
		ZEVector2 Vector(1.0f, 2.0f);

		ZEMatrix3x3::Transform(Out, Matrix, Vector);
		ZETestCheckEqual(Out, ZEVector2(97.0f, 41.0f));
	}
	ZETest("void ZEMatrix3x3::Transform(ZEVector3& Out, const ZEMatrix3x3& Matrix, const ZEVector3& Vector)")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f,  9.0f, 10.0f);

		ZEVector3 Out;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZEMatrix3x3::Transform(Out, Matrix, Vector);
		ZETestCheckEqual(Out, ZEVector3(129.0f, 59.0f, 57.0f));
	}
	ZETest("void ZEMatrix3x3::Transform(ZEVector4& Out, const ZEMatrix3x3& Matrix, const ZEVector4& Vector)")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector4 Out;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		ZEMatrix3x3::Transform(Out, Matrix, Vector);
		ZETestCheckEqual(Out, ZEVector4(129.0f, 59.0f, 57.0f, 1.0f));
	}
	ZETest("void ZEMatrix3x3::Transpose(ZEMatrix3x3& Out, const ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix(1.2f, 1.2f, 2.4f,
			3.6f, 6.0f, 9.6f,
			15.6f, 25.2f, 40.8f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Transpose(Out, Matrix);
		ZETestCheckEqual(Out, ZEMatrix3x3(1.2f, 3.6f, 15.6f, 1.2f, 6.0f, 25.2f, 2.4f, 9.6f, 40.8f));
	}
	ZETest("ZEMatrix3x3 ZEMatrix3x3::Transpose() const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Transpose(Out, Matrix);
		ZETestCheckEqual(Out, ZEMatrix3x3(35.0f, 12.0f, 9.0f, 23.0f, 10.0f, 9.0f, 16.0f, 9.0f, 10.0f));
	}
	ZETest("void ZEMatrix3x3::TransposeSelf()")
	{
		ZEMatrix3x3 Matrix(1.2f, 1.2f, 2.4f,
			3.6f, 6.0f, 9.6f,
			15.6f, 25.2f, 40.8f);

		Matrix.TransposeSelf();
		ZETestCheckEqual(Matrix, ZEMatrix3x3(1.2f, 3.6f, 15.6f, 1.2f, 6.0f, 25.2f, 2.4f, 9.6f, 40.8f));
	}
}

