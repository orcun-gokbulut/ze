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



#include "ZETest/ZETest.h"
#include "ZEMath/ZEMath.h"
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

ZE_TEST(ZEMatrix3x3)
{
	ZE_TEST_ITEM("ZEMatrix3x3::ZEMatrix3x3(float M00, float M01, float M02, float M10, float M11, float M12, float M20, float M21, float M22)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}

	ZE_TEST_ITEM("ZEMatrix3x3::ZEMatrix3x3(float M[9])")
	{
		float M[9]={1.0f, 3.0f, 13.0f, 1.0f, 5.0f, 21.0f, 2.0f,  8.0f,  34.0f};
		ZEMatrix3x3 Matrix(M);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}

	ZE_TEST_ITEM("inline void ZEMatrix3x3::Add(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B( 1.0f,  2.0f,  3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Add(Out, A, B);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f));
	}

	ZE_TEST_ITEM("inline void ZEMatrix3x3::Create(ZEMatrix3x3& Matrix, float M[9])")
	{
		ZEMatrix3x3 Matrix;
		float M[9]={34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f};

		ZEMatrix3x3::Create(Matrix, M);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(34.0f, 8.0f, 2.0f, 21.0f, 5.0f, 1.0f, 13.0f, 3.0f, 1.0f));
	}

	ZE_TEST_ITEM("inline void ZEMatrix3x3::Create(ZEMatrix3x3 &Matrix, float M11, float M12, float M13, float M21, float M22, float M23, float M31, float M32, float M33)")
	{
		ZEMatrix3x3 Matrix;

		ZEMatrix3x3::Create(Matrix,
			1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateIdentity(ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix;
		
		ZEMatrix3x3::CreateIdentity(Matrix);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateOrientation2D(ZEMatrix3x3& Matrix, const ZEVector2& Translation, float Angle, const ZEVector2& Scale)")
	{
		ZEMatrix3x3 TransformationMatrix;
		ZEMatrix3x3::CreateOrientation2D(TransformationMatrix, ZEVector2(5.0f, 5.0f), ZE_PI_2, ZEVector2(2.0f, 2.0f));

		ZEVector2 SampleVector(5.0f, 0.0f);

		ZEVector2 ResultingVector = TransformationMatrix * SampleVector;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultingVector.x, 5.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultingVector.y, -5.0f);
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, const ZEQuaternion& Rotation)")
	{
		ZEQuaternion Rotation(ZE_PI_4, ZEVector3::UnitY);
		ZEMatrix3x3 Matrix;

		ZEMatrix3x3::CreateRotation(Matrix, Rotation);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.70710673f, 0.0f, 0.70710680f, 0.0f, 1.0f, 0.0f, -0.70710680f, 0.0f, 0.70710673f));
	}

	ZE_TEST_ITEM("static void ZEMatrix3x3::CreateRotation2D(ZEMatrix3x3& Matrix, float Angle)")
	{
		ZEMatrix3x3 RotationMatrix;
		ZEMatrix3x3::CreateRotation2D(RotationMatrix, ZE_PI_2);

		ZEVector2 SampleVector(5.0f, 0.0f);

		ZEVector2 ResultingVector = RotationMatrix * SampleVector;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultingVector.x, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultingVector.y, -5.0f);
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateRotationX(ZEMatrix3x3& Matrix, float Pitch)")
	{
		ZEMatrix3x3 Matrix;
		float Pitch = ZE_PI_4;
		
		ZEMatrix3x3::CreateRotationX(Matrix, Pitch);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 0.70710677f, -0.70710677f, 0.0f, 0.70710677f, 0.70710677f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder)")
	{
		ZEMatrix3x3 Matrix;
		float Pitch = ZE_PI;
		float Yawn = ZE_PI_2;
		float Roll = ZE_PI_4;
		
		ZE_TEST_CASE("for RotationOrder ZE_RO_XYZ")
		{
			ZERotationOrder RotationOrder = ZE_RO_XYZ;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.0f, 0.0f, 1.0f, -0.70710678f, -0.70710678f, 0.0f, 0.70710678f, -0.70710678f, 0.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_XZY")
		{
			ZERotationOrder RotationOrder = ZE_RO_XZY;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.0f, -0.70710678f, 0.70710678f, 0.0f, -0.70710678f, -0.70710678f, 1.0f, 0.0f, 0.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_YXZ")
		{
			ZERotationOrder RotationOrder = ZE_RO_YXZ;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.0f, 0.0f, -1.0f, -0.70710678f, -0.70710678f, 0.0f, -0.70710678f, 0.70710678f, 0.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_YZX")
		{
			ZERotationOrder RotationOrder = ZE_RO_YZX;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.0f, 0.0f, -1.0f, 0.70710678f, -0.70710678f, 0.0f, -0.70710678f, -0.70710678f, 0.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_ZXY")
		{
			ZERotationOrder RotationOrder = ZE_RO_ZXY;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.0f, 0.70710678f, 0.70710678f, 0.0f, -0.70710678f, 0.70710678f, 1.0f, 0.0f, 0.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_ZYX")
		{
			ZERotationOrder RotationOrder = ZE_RO_ZYX;

			ZEMatrix3x3::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.0f, 0.70710678f, -0.70710678f, 0.0f, -0.70710678f, -0.70710678f, -1.0f, 0.0f, 0.0f));
		}
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateRotationY(ZEMatrix3x3& Matrix, float Yawn)")
	{
		ZEMatrix3x3 Matrix;
		float Yawn = ZE_PI_4;

		ZEMatrix3x3::CreateRotationY(Matrix, Yawn);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.70710677f, 0.0f, 0.70710677f, 0.0f, 1.0f, 0.0f, -0.70710677f, 0.0f, 0.70710677f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateRotationZ(ZEMatrix3x3& Matrix, float Roll)")
	{
		ZEMatrix3x3 Matrix;
		float Roll = ZE_PI_4;

		ZEMatrix3x3::CreateRotationZ(Matrix, Roll);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(0.70710677f, -0.70710677f, 0.0f, 0.70710677f, 0.70710677f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateScale(ZEMatrix3x3& Matrix, float x, float y, float z)")
	{
		ZEMatrix3x3 Matrix;
		
		ZEMatrix3x3::CreateScale(Matrix, 1.0f, 2.0f, 3.0f);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 3.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix3x3::CreateScale2D(ZEMatrix3x3& Matrix, float x, float y)")
	{
		ZEMatrix3x3 ScaleMatrix;
		ZEMatrix3x3::CreateScale2D(ScaleMatrix, 2.0f, 2.0f);

		ZEVector2 SampleVector(5.0f, 5.0f);
		ZEVector2 ResultVector = ScaleMatrix * SampleVector;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.x, 10.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.y, 10.0f);
	}

	ZE_TEST_ITEM("static void ZEMatrix3x3::CreateScale2D(ZEMatrix3x3& Matrix, const ZEVector2& Scale)")
	{
		ZEMatrix3x3 ScaleMatrix;
		ZEMatrix3x3::CreateScale2D(ScaleMatrix, ZEVector2(2.0f, 2.0f));

		ZEVector2 SampleVector(5.0f, 5.0f);
		ZEVector2 ResultVector = ScaleMatrix * SampleVector;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.x, 10.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.y, 10.0f);
	}

	ZE_TEST_ITEM("static void ZEMatrix3x3::CreateTranslation2D(ZEMatrix3x3& Matrix, float x, float y)")
	{
		ZEMatrix3x3 TranslationMatrix;
		ZEMatrix3x3::CreateTranslation2D(TranslationMatrix, 5.0f, 5.0f);

		ZEVector2 SampleVector(3.0f, 3.0f);
		ZEVector2 ResultVector = TranslationMatrix * SampleVector;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.x, 8.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.y, 8.0f);
	}

	ZE_TEST_ITEM("static void ZEMatrix3x3::CreateTranslation2D(ZEMatrix3x3& Matrix, const ZEVector2& Translation)")
	{
		ZEMatrix3x3 TranslationMatrix;
		ZEMatrix3x3::CreateTranslation2D(TranslationMatrix, ZEVector2(5.0f, 5.0f));

		ZEVector2 SampleVector(3.0f, 3.0f);
		ZEVector2 ResultVector = TranslationMatrix * SampleVector;

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.x, 8.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultVector.y, 8.0f);
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::CreateZero(ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix;
		
		ZEMatrix3x3::CreateZero(Matrix);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	}

	ZE_TEST_ITEM("float ZEMatrix3x3::Determinant(const ZEMatrix3x3 &Matrix)")
	{
		ZEMatrix3x3 Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		
		float Determinant = ZEMatrix3x3::Determinant(Matrix);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Determinant, 0.0f);
	}

	ZE_TEST_ITEM("float ZEMatrix3x3::Determinant() const")
	{
		ZEMatrix3x3 Matrix(1.0f, 2.0f, -5.0f,
			4.0f, 2.0f, 1.0f,
			3.0f, 2.0f, 9.0f);

		float Determinant = Matrix.Determinant();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Determinant, -60.0f);
	}

	ZE_TEST_ITEM("bool ZEMatrix3x3::Equals(const ZEMatrix3x3& Matrix) const")
	{
		ZEMatrix3x3 MatrixA(1.0f, 2.0f, -5.0f,
			4.0f, 2.0f, 1.0f,
			3.0f, 2.0f, 9.0f);
		ZEMatrix3x3 MatrixB(1.00000001f, 2.00000001f, -5.00000001f,
			4.00000001f, 2.00000001f, 1.00000001f,
			3.00000001f, 2.00000001f, 9.00000001f);
		ZEMatrix3x3 MatrixC(1.00002f, 2.00002f, -5.00002f,
			4.00002f, 2.00002f, 1.00002f,
			3.00002f, 2.00002f, 9.00002f);

		bool Equal = MatrixA.Equals(MatrixB);
		ZE_TEST_CHECK_ERROR(Equal);

		Equal = MatrixA.Equals(MatrixC);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("bool ZEMatrix3x3::Equals(const ZEMatrix3x3& Matrix, float Threshold)")
	{
		float Threshold = 0.1f;
		ZEMatrix3x3 MatrixA(1.11f, 2.11f, -5.11f,
			4.11f, 2.11f, 1.11f,
			3.11f, 2.11f, 9.11f);

		ZEMatrix3x3 MatrixB(1.1f, 2.1f, -5.1f,
			4.1f, 2.1f, 1.1f,
			3.1f, 2.1f, 9.1f);

		ZEMatrix3x3 MatrixC(1.0f, 2.0f, -5.0f,
			4.0f, 2.0f, 1.0f,
			3.0f, 2.0f, 9.0f);

		bool Equal = MatrixA.Equals(MatrixB, Threshold);
		ZE_TEST_CHECK_ERROR(Equal);

		Equal = MatrixA.Equals(MatrixC, Threshold);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("static void ZEMatrix3x3::Get2DDecomposition(ZEVector2& Translation, float& Rotation, ZEVector2& Scale, const ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 TransformationMatrix;
		ZEMatrix3x3::CreateOrientation2D(TransformationMatrix, ZEVector2(3.0f, 5.0f), ZE_PI_2, ZEVector2(2.0f, 4.0f));

		ZEVector2 ResultPosition;
		float ResultRotation;
		ZEVector2 ResultScale;

		ZEMatrix3x3::Get2DDecomposition(ResultPosition, ResultRotation, ResultScale, TransformationMatrix);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultPosition.x, 3.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultPosition.y, 5.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation, ZE_PI_2);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.x, 2.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.y, 4.0f);
	}

	ZE_TEST_ITEM("float ZEMatrix3x3::Get2DRotation()")
	{
		ZEMatrix3x3 TransformationMatrix;
		ZEMatrix3x3::CreateOrientation2D(TransformationMatrix, ZEVector2(3.0f, 5.0f), ZE_PI_2, ZEVector2(2.0f, 4.0f));

		float ResultRotation = TransformationMatrix.Get2DRotation();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation, ZE_PI_2);
	}

	ZE_TEST_ITEM("ZEVector2 ZEMatrix3x3::Get2DScale()")
	{
		ZEMatrix3x3 TransformationMatrix;
		ZEMatrix3x3::CreateOrientation2D(TransformationMatrix, ZEVector2(3.0f, 5.0f), ZE_PI_2, ZEVector2(2.0f, 4.0f));

		ZEVector2 ResultScale = TransformationMatrix.Get2DScale();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.x, 2.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.y, 4.0f);
	}

	ZE_TEST_ITEM("ZEVector2 ZEMatrix3x3::Get2DTranslation()")
	{
		ZEMatrix3x3 TransformationMatrix;
		ZEMatrix3x3::CreateOrientation2D(TransformationMatrix, ZEVector2(3.0f, 5.0f), ZE_PI_2, ZEVector2(2.0f, 4.0f));

		ZEVector2 ResultPosition = TransformationMatrix.Get2DTranslation();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultPosition.x, 3.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultPosition.y, 5.0f);
	}

	ZE_TEST_ITEM("static void ZEMatrix3x3::GetDecomposition(ZEQuaternion& Rotation, ZEVector3& Scale, const ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 RotationMatrix;
		ZEMatrix3x3::CreateRotation(RotationMatrix, ZEQuaternion(ZE_PI_2, ZEVector3::UnitY));

		ZEMatrix3x3 ScaleMatrix;
		ZEMatrix3x3::CreateScale(ScaleMatrix, 2.0f, 3.0f, 4.0f);

		ZEMatrix3x3 TransformationMatrix = RotationMatrix * ScaleMatrix;

		ZEQuaternion ResultRotation;
		ZEVector3 ResultScale;

		ZEMatrix3x3::GetDecomposition(ResultRotation, ResultScale, TransformationMatrix);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.w, 0.70710677f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.x, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.y, 0.70710677f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.z, 0.0f);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.x, 2.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.y, 3.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.z, 4.0f);

	}

	ZE_TEST_ITEM("ZEQuaternion ZEMatrix3x3::GetRotation()")
	{
		ZEMatrix3x3 RotationMatrix;
		ZEMatrix3x3::CreateRotation(RotationMatrix, ZEQuaternion(ZE_PI_2, ZEVector3::UnitY));

		ZEMatrix3x3 ScaleMatrix;
		ZEMatrix3x3::CreateScale(ScaleMatrix, 2.0f, 3.0f, 4.0f);

		ZEMatrix3x3 TransformationMatrix = RotationMatrix * ScaleMatrix;

		ZEQuaternion ResultRotation = TransformationMatrix.GetRotation();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.w, 0.70710677f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.x, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.y, 0.70710677f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.z, 0.0f);
	}

	ZE_TEST_ITEM("ZEVector3 ZEMatrix3x3::GetScale()")
	{
		ZEMatrix3x3 RotationMatrix;
		ZEMatrix3x3::CreateRotation(RotationMatrix, ZEQuaternion(ZE_PI_2, ZEVector3::UnitY));

		ZEMatrix3x3 ScaleMatrix;
		ZEMatrix3x3::CreateScale(ScaleMatrix, 2.0f, 3.0f, 4.0f);

		ZEMatrix3x3 TransformationMatrix = RotationMatrix * ScaleMatrix;

		ZEVector3 ResultScale = TransformationMatrix.GetScale();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.x, 2.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.y, 3.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.z, 4.0f);
	}

	ZE_TEST_ITEM("bool ZEMatrix3x3::Inverse(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix)")
	{
		ZEMatrix3x3 Matrix(2.0f, 1.0f, 1.0f,
			3.0f, 2.0f, 1.0f,
			4.0f, -3.0f, 5.0f);

		ZEMatrix3x3 Out;

		bool Result = ZEMatrix3x3::Inverse(Out, Matrix);
		ZE_TEST_CHECK_ERROR(Result == true);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEMatrix3x3(-6.5f, 4.0f, 0.5f, 5.5f, -3.0f, -0.5f, 8.5f, -5.0f, -0.5f));
		
		ZE_TEST_CASE("for Determinant=0")
		{
			ZEMatrix3x3 Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
			ZEMatrix3x3 Out;

			bool Result = ZEMatrix3x3::Inverse(Out, Matrix);
			ZE_TEST_CHECK_ERROR(Result == false);
		}
	}

	ZE_TEST_ITEM("ZEMatrix3x3 ZEMatrix3x3::Inverse() const")
	{
		ZEMatrix3x3 Matrix;
		ZEMatrix3x3::CreateIdentity(Matrix);
		
		ZEMatrix3x3 Out = Matrix.Inverse();
		ZE_TEST_CHECK_EQUAL(Out, ZEMatrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::InverseSelf()")
	{
		ZEMatrix3x3 Matrix(2.0f, 1.0f, 1.0f,
			3.0f, 2.0f, 1.0f,
			4.0f, -3.0f, 5.0f);

		Matrix.InverseSelf();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix3x3(-6.5f, 4.0f, 0.5f, 5.5f, -3.0f, -0.5f, 8.5f, -5.0f, -0.5f));
	}

	ZE_TEST_ITEM("inline void ZEMatrix3x3::Multiply(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)")
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 Out;
		
		ZEMatrix3x3::Multiply(Out, A, B);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEMatrix3x3(312.0f, 486.0f, 798.0f, 159.0f, 251.0f, 410.0f, 166.0f, 264.0f, 430.0f));

		ZE_TEST_CASE("Multiply by Identity Matrix")
		{
			ZEMatrix3x3 Out;
			ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f,  9.0f,  9.0f, 10.0f);
			ZEMatrix3x3 Identity;
			ZEMatrix3x3::CreateIdentity(Identity);

			ZEMatrix3x3::Multiply(Out, Matrix, Identity);
			ZE_TEST_CHECK_EQUAL(Out, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f,  9.0f,  9.0f, 10.0f));
		}
	}

	ZE_TEST_ITEM("bool ZEMatrix3x3::operator != (const ZEMatrix3x3 &M) const")
	{
		ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		bool Result = (A != B);
		ZE_TEST_CHECK_ERROR(Result == false);

		ZE_TEST_CASE("!= WithMatrixArg for Comparison for Different Matrices")
		{
			ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f);

			ZEMatrix3x3 B(5.0f, 1.0f, 3.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 3.0f);

			bool Result = (A != B);
			ZE_TEST_CHECK_ERROR(Result == true);
		}
	}

	ZE_TEST_ITEM("ZEMatrix3x3 ZEMatrix3x3::operator-(const ZEMatrix3x3 &RightOperand) const")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Result;

		Result = A - B;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix3x3(33.0f, 19.0f, 10.0f, 4.0f, 0.0f, -3.0f, -5.0f, -7.0f, -8.0f));
	}

	ZE_TEST_ITEM("ZEMatrix3x3 ZEMatrix3x3::operator*(const ZEMatrix3x3 &RightOperand) const ")
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 Result = A * B;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix3x3(312.0f, 486.0f, 798.0f, 159.0f, 251.0f, 410.0f, 166.0f, 264.0f, 430.0f));
	}

	ZE_TEST_ITEM("ZEVector2 ZEMatrix3x3::operator*(const ZEVector2 &RightOperand) const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector2 Vector(1.0f, 2.0f);

		ZEVector2 Result = Matrix * Vector;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector2(97.0f, 41.0f));
	}

	ZE_TEST_ITEM("ZEVector3 ZEMatrix3x3::operator*(const ZEVector3 &RightOperand) const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZEVector3 Result = Matrix * Vector;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(129.0f, 59.0f, 57.0f));
	}

	ZE_TEST_ITEM("ZEVector4 ZEMatrix3x3::operator*(const ZEVector4 &RightOperand) const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		ZEVector4 Result = Matrix * Vector;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector4(129.0f, 59.0f, 57.0f, 1.0f));
	}

	ZE_TEST_ITEM("ZEMatrix3x3 ZEMatrix3x3::operator*(float S)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float S = 2.0f;

		ZEMatrix3x3 Result = Matrix * S;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix3x3(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f));
	}

	ZE_TEST_ITEM("ZEMatrix3x3& ZEMatrix3x3::operator *= (const ZEMatrix3x3 &RightOperand)")
	{
		ZEMatrix3x3 A(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		A *= B;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(A, ZEMatrix3x3(312.0f, 486.0f, 798.0f, 159.0f, 251.0f, 410.0f, 166.0f, 264.0f, 430.0f));
	}

	ZE_TEST_ITEM("ZEMatrix3x3& ZEMatrix3x3::operator *= (float S)")
	{
		ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float S = 2.0f;

		A *= S;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(A, ZEMatrix3x3(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f));
	}

	ZE_TEST_ITEM("float& ZEMatrix3x3::operator[](ZESize Index)")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float M11 = Matrix[0];
		float M21 = Matrix[1];
		float M31 = Matrix[2];
		ZE_TEST_CHECK_EQUAL(M11, 1.0f);
		ZE_TEST_CHECK_EQUAL(M21, 3.0f);
		ZE_TEST_CHECK_EQUAL(M31, 13.0f);
	}

	ZE_TEST_ITEM("float ZEMatrix3x3::operator[](ZESize Index) const")
	{
		ZEMatrix3x3 Matrix(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		float M12 = Matrix[3];
		float M21 = Matrix[1];
		float M32 = Matrix[5];
		ZE_TEST_CHECK_EQUAL(M12, 1.0f);
		ZE_TEST_CHECK_EQUAL(M21, 3.0f);
		ZE_TEST_CHECK_EQUAL(M32, 21.0f);
	}

	ZE_TEST_ITEM("ZEMatrix3x3 ZEMatrix3x3::operator+(const ZEMatrix3x3 &RightOperand) const")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f,  2.0f,  3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Result = A + B;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f));	
	}

	ZE_TEST_ITEM("ZEMatrix3x3& ZEMatrix3x3::operator += (const ZEMatrix3x3 &RightOperand)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		A += B;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(A, ZEMatrix3x3(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f));
	}

	ZE_TEST_ITEM("ZEMatrix3x3& ZEMatrix3x3::operator -= (const ZEMatrix3x3 &RightOperand)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		A -= B;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(A, ZEMatrix3x3(33.0f, 19.0f, 10.0f, 4.0f, 0.0f, -3.0f, -5.0f, -7.0f, -8.0f));
	}

	ZE_TEST_ITEM("bool ZEMatrix3x3::operator == (const ZEMatrix3x3 &M) const")
	{
		ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		ZEMatrix3x3 B(1.0f, 1.0f, 2.0f,
			3.0f, 5.0f, 8.0f,
			13.0f, 21.0f, 34.0f);

		bool Result = (A == B);
		ZE_TEST_CHECK_ERROR(Result == true);

		ZE_TEST_CASE("== WithMatrixArg for Comparison for Different Matrices")
		{
			ZEMatrix3x3 A(1.0f, 1.0f, 2.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 34.0f);

			ZEMatrix3x3 B(5.0f, 1.0f, 3.0f,
				3.0f, 5.0f, 8.0f,
				13.0f, 21.0f, 3.0f);

			bool Result = (A == B);
			ZE_TEST_CHECK_ERROR(Result == false);
		}
	}

	ZE_TEST_ITEM("inline void ZEMatrix3x3::Scale(ZEMatrix3x3 &Out, const ZEMatrix3x3& A, float s)")
	{
		ZEMatrix3x3 Matrix(1.0f, 0.0f, 1.5f,
			3.0f, 5.0f, 8.0f,
			13.0f, 7.0f, 14.0f);

		ZEMatrix3x3 Out;
		float s = 2.0f;

		ZEMatrix3x3::Scale(Out, Matrix, s);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEMatrix3x3(2.0f, 0.0f, 3.0f, 6.0f, 10.0f, 16.0f, 26.0f, 14.0f, 28.0f));
	}

	ZE_TEST_ITEM("inline void ZEMatrix3x3::Sub(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B)")
	{
		ZEMatrix3x3 A(34.0f, 21.0f, 13.0f,
			8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f);

		ZEMatrix3x3 B(1.0f,  2.0f, 3.0f,
			4.0f, 5.0f, 6.0f,
			7.0f, 8.0f, 9.0f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Sub(Out, A, B);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEMatrix3x3(33.0f, 19.0f, 10.0f, 4.0f, 0.0f, -3.0f, -5.0f, -7.0f, -8.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::Transform(ZEVector2& Out, const ZEMatrix3x3& Matrix, const ZEVector2& Vector)")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector2 Out;
		ZEVector2 Vector(1.0f, 2.0f);

		ZEMatrix3x3::Transform(Out, Matrix, Vector);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector2(97.0f, 41.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::Transform(ZEVector3& Out, const ZEMatrix3x3& Matrix, const ZEVector3& Vector)")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f,  9.0f, 10.0f);

		ZEVector3 Out;
		ZEVector3 Vector(1.0f, 2.0f, 3.0f);

		ZEMatrix3x3::Transform(Out, Matrix, Vector);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector3(129.0f, 59.0f, 57.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::Transform(ZEVector4& Out, const ZEMatrix3x3& Matrix, const ZEVector4& Vector)")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEVector4 Out;
		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 4.0f);

		ZEMatrix3x3::Transform(Out, Matrix, Vector);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector4(129.0f, 59.0f, 57.0f, 1.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::Transpose(ZEMatrix3x3& Out, const ZEMatrix3x3& Matrix)")
	{
		ZEMatrix3x3 Matrix(1.2f, 1.2f, 2.4f,
			3.6f, 6.0f, 9.6f,
			15.6f, 25.2f, 40.8f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Transpose(Out, Matrix);
		ZE_TEST_CHECK_EQUAL(Out, ZEMatrix3x3(1.2f, 3.6f, 15.6f, 1.2f, 6.0f, 25.2f, 2.4f, 9.6f, 40.8f));
	}

	ZE_TEST_ITEM("ZEMatrix3x3 ZEMatrix3x3::Transpose() const")
	{
		ZEMatrix3x3 Matrix(35.0f, 23.0f, 16.0f,
			12.0f, 10.0f, 9.0f,
			9.0f, 9.0f, 10.0f);

		ZEMatrix3x3 Out;

		ZEMatrix3x3::Transpose(Out, Matrix);
		ZE_TEST_CHECK_EQUAL(Out, ZEMatrix3x3(35.0f, 12.0f, 9.0f, 23.0f, 10.0f, 9.0f, 16.0f, 9.0f, 10.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix3x3::TransposeSelf()")
	{
		ZEMatrix3x3 Matrix(1.2f, 1.2f, 2.4f,
			3.6f, 6.0f, 9.6f,
			15.6f, 25.2f, 40.8f);

		Matrix.TransposeSelf();
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix3x3(1.2f, 3.6f, 15.6f, 1.2f, 6.0f, 25.2f, 2.4f, 9.6f, 40.8f));
	}
}
