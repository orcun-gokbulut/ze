//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrix4x4Tests.cpp
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
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

bool ZETestInternalCheckClose(const ZEMatrix4x4& Actual, const ZEMatrix4x4& Expected, const ZEMatrix4x4& Threshold = ZEMatrix4x4(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD,ZE_TEST_CLOSE_THRESHOLD))
{
	return (ZETestInternalCheckClose(Actual.M11, Expected.M11, Threshold.M11) && ZETestInternalCheckClose(Actual.M12, Expected.M12, Threshold.M12) && ZETestInternalCheckClose(Actual.M13, Expected.M13, Threshold.M13) && ZETestInternalCheckClose(Actual.M14, Expected.M14, Threshold.M14) && ZETestInternalCheckClose(Actual.M21, Expected.M21, Threshold.M21) && ZETestInternalCheckClose(Actual.M22, Expected.M22, Threshold.M22) && ZETestInternalCheckClose(Actual.M23, Expected.M23, Threshold.M23) && ZETestInternalCheckClose(Actual.M24, Expected.M24, Threshold.M24) && ZETestInternalCheckClose(Actual.M31, Expected.M31, Threshold.M31) && ZETestInternalCheckClose(Actual.M32, Expected.M32, Threshold.M32) && ZETestInternalCheckClose(Actual.M33, Expected.M33, Threshold.M33) && ZETestInternalCheckClose(Actual.M34, Expected.M34, Threshold.M34) && ZETestInternalCheckClose(Actual.M41, Expected.M41, Threshold.M41) && ZETestInternalCheckClose(Actual.M42, Expected.M42, Threshold.M42) && ZETestInternalCheckClose(Actual.M43, Expected.M43, Threshold.M43) && ZETestInternalCheckClose(Actual.M44, Expected.M44, Threshold.M44));
}

ZE_TEST(ZEMatrix4x4)
{
	ZE_TEST_ITEM("ZEMatrix4x4::ZEMatrix4x4(float M11, float M12, float M13, float M14, float M21, float M22, float M23, float M24, float M31, float M32, float M33, float M34, float M41, float M42, float M43, float M44)")
	{
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 3.0f, 5.0f, 8.0f, 13.0f);

		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4( 1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 3.0f, 5.0f, 8.0f, 13.0f));
	}

	ZE_TEST_ITEM("ZEMatrix4x4::ZEMatrix4x4(float * M)")
	{
		float M[16] = {1.0f, 5.0f, 34.0f, 3.0f, 1.0f, 8.0f, 55.0f, 5.0f, 2.0f, 13.0f, 1.0f, 8.0f, 3.0f, 21.0f, 1.0f, 13.0f};
		ZEMatrix4x4 Matrix(M);

		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4( 1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 3.0f, 5.0f, 8.0f, 13.0f));
	}

	ZE_TEST_ITEM("static inline void ZEMatrix4x4::Create(ZEMatrix4x4 & Matrix, float M11, float M12, float M13, float M14, float M21, float M22, float M23, float M24, float M31, float M32, float M33, float M34, float M41, float M42, float M43, float M44)")
	{
		ZEMatrix4x4 Matrix;
		ZEMatrix4x4::Create(Matrix, 1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 3.0f, 5.0f, 8.0f, 13.0f);

		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 3.0f, 5.0f, 8.0f, 13.0f));

	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::Create(ZEMatrix4x4 & Matrix, float * M)")
	{
		ZEMatrix4x4 Matrix;
		float M[16]={13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f, 34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f};
		ZEMatrix4x4::Create(Matrix, M);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(13.0f, 2.0f, 21.0f, 3.0f, 8.0f, 1.0f, 13.0f, 2.0f, 5.0f, 1.0f, 8.0f, 1.0f, 3.0f, 34.0f, 5.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateIdentity(ZEMatrix4x4 & Matrix)")
	{
		ZEMatrix4x4 Matrix;
		ZEMatrix4x4::CreateIdentity(Matrix);

		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateLookAtTransform(ZEMatrix4x4 & Matrix, const ZEVector3 & Position, const ZEVector3 & Target, const ZEVector3 & Up)")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Position(3.0f, 4.0f, 5.0f);
		ZEVector3 Target(5.0f, 4.0f, 6.0f);
		ZEVector3 Up(0.0f, 1.0f, 0.0f);

		ZEMatrix4x4::CreateLookAtTransform(Matrix, Position, Target, Up);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.44721359f, 0.0f,  -0.89442718f, 3.13049516f, -0.0f, 1.0f, 0.0f, -4.0f, 0.89442718f, 0.0f, 0.44721359f, -4.9193497f, 0.0f, 0.0f, 0.0f, 1.0f));
		
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4 & Matrix, const ZEVector3 & Position, const ZEQuaternion & Rotation, const ZEVector3 & Scale)")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Position(1.0f, 1.0f, 1.0f);
		ZEQuaternion Rotation(ZE_PI_4, ZEVector3::UnitX);
		ZEVector3 Scale(0.0f,1.0f,0.0f);

		ZEMatrix4x4::CreateOrientation(Matrix, Position, Rotation, Scale);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.70710673f, -0.0f, 1.0f, 0.0f, 0.70710680f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));

		ZE_TEST_CASE("another test for CreateOrientation")
		{
			ZEMatrix4x4 Matrix;
			ZEVector3 Position(5.0f, 6.0f, 7.0f);
			ZEQuaternion Rotation(ZE_PI_2, ZEVector3::UnitX);
			ZEVector3 Scale(1.0f,1.0f,1.0f);

			ZEMatrix4x4::CreateOrientation(Matrix, Position, Rotation, Scale);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(1.000000f, 0.000000f ,0.000000f, 5.000000f,
				0.000000f, 0.000000f, -0.999999f, 6.000000f,
				0.000000f, 0.99999994f, 0.000000f, 7.000000f,
				0.000000f, 0.000000f, 0.000000f, 1.000000f));
		}
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4 & Matrix, const ZEVector3 & Position, const ZEQuaternion & Rotation)")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Position(5.0f, 6.0f, 7.0f);
		ZEQuaternion Rotation(ZE_PI_2, ZEVector3::UnitX);

		ZEMatrix4x4::CreateOrientation(Matrix, Position, Rotation);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(1.000000f, 0.000000f ,0.000000f, 5.000000f,
			0.000000f, 0.00000003f, -0.99999996f, 6.000000f,
			0.000000f, 0.99999996f, 0.00000003f, 7.000000f,
			0.000000f, 0.000000f, 0.000000f, 1.000000f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateOrthographicProjection(ZEMatrix4x4 & Matrix, float Width, float Height, float NearZ, float FarZ)")
	{
		ZEMatrix4x4 Matrix;

		float Width = 5.0f;
		float Height = 5.0f;
		float NearZ = 0.5f;
		float FarZ = 50.0f;

		ZEMatrix4x4::CreateOrthographicProjection(Matrix, Width, Height, NearZ, FarZ);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.40000000f, 0.0f, 0.0f, 0.0f, 0.0f, 0.40000000f, 0.0f, 0.0f, 0.0f, 0.0f, 0.02020202f, -0.0101010101f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreatePerspectiveProjection(ZEMatrix4x4 & Matrix, float FOV, float AspectRatio, float NearZ, float FarZ)")
	{
		ZEMatrix4x4 Matrix;

		float FOV = ZE_PI_2;
		float AspectRatio = 4.0f/3.0f;
		float NearZ = 0.5f;
		float FarZ = 50.0f;

		ZEMatrix4x4::CreatePerspectiveProjection(Matrix, FOV, AspectRatio, NearZ, FarZ);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0101010101f, -0.505050505f, 0.0f, 0.0f, 1.0f, 0.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateRotation(ZEMatrix4x4 & Matrix, const ZEQuaternion & Rotation)")
	{	
		ZEMatrix4x4 Matrix;
		ZEQuaternion Rotation(ZE_PI_4, ZEVector3::UnitX);

		ZEMatrix4x4::CreateRotation(Matrix, Rotation);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.70710673f, -0.70710680f, 0.0f, 0.0f, 0.70710680f, 0.70710673, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));

	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateRotationX(ZEMatrix4x4 & Matrix, float Pitch)")
	{
		ZEMatrix4x4 Matrix;
		float Pitch = ZE_PI_2;
	
		ZEMatrix4x4::CreateRotationX(Matrix, Pitch);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateRotation(ZEMatrix4x4& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder)")
	{
		ZEMatrix4x4 Matrix;
		float Pitch = ZE_PI;
		float Yawn = ZE_PI_2;
		float Roll = ZE_PI_4;

		ZE_TEST_CASE("for RotationOrder ZE_RO_XYZ")
		{
			ZERotationOrder RotationOrder = ZE_RO_XYZ;
			
			ZEMatrix4x4::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, 0.0f, 1.0f, 0.0f, -0.70710678f, -0.70710678f, 0.0f, 0.0f, 0.70710678f, -0.70710678f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_XZY")
		{
			ZERotationOrder RotationOrder = ZE_RO_XZY;

			ZEMatrix4x4::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, -0.70710678f, 0.70710678f, 0.0f, 0.0f, -0.70710678f, -0.70710678f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_YXZ")
		{
			ZERotationOrder RotationOrder = ZE_RO_YXZ;

			ZEMatrix4x4::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, 0.0f, -1.0f, 0.0f, -0.70710678f, -0.70710678f, 0.0f, 0.0f, -0.70710678f, 0.70710678f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_YZX")
		{
			ZERotationOrder RotationOrder = ZE_RO_YZX;

			ZEMatrix4x4::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, 0.0f, -1.0f, 0.0f, 0.70710678f, -0.70710678f, 0.0f, 0.0f, -0.70710678f, -0.70710678f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_ZXY")
		{
			ZERotationOrder RotationOrder = ZE_RO_ZXY;

			ZEMatrix4x4::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, 0.70710678f, 0.70710678f, 0.0f, 0.0f, -0.70710678f, 0.70710678f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		}

		ZE_TEST_CASE("for RotationOrder ZE_RO_ZYX")
		{
			ZERotationOrder RotationOrder = ZE_RO_ZYX;

			ZEMatrix4x4::CreateRotation(Matrix, Pitch, Yawn, Roll, RotationOrder);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, 0.70710678f, -0.70710678f, 0.0f, 0.0f, -0.70710678f, -0.70710678f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateRotationY(ZEMatrix4x4 & Matrix, float Yawn)")
	{
		ZEMatrix4x4 Matrix;
		float Yawn = ZE_PI_4;

		ZEMatrix4x4::CreateRotationY(Matrix, Yawn);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.70710678f, 0.0f, 0.70710678f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -0.70710678f, 0.0f, 0.70710678f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateRotationZ(ZEMatrix4x4 & Matrix, float Roll)")
	{
		ZEMatrix4x4 Matrix;
		float Roll = ZE_PI_2;

		ZEMatrix4x4::CreateRotationZ(Matrix, Roll);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateScale(ZEMatrix4x4 & Matrix, float x, float y, float z)")
	{
		ZEMatrix4x4 Matrix;

		ZEMatrix4x4::CreateScale(Matrix, 1.0f, 3.0f, 5.0f);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateScale(ZEMatrix4x4 & Matrix, const ZEVector3 & Scale)")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Scale(2.0f, 4.0f, 6.0f);

		ZEMatrix4x4::CreateScale(Matrix, Scale);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4 & Matrix, float x, float y, float z)")
	{
		ZEMatrix4x4 Matrix;

		ZEMatrix4x4::CreateTranslation(Matrix, 1.0f, 3.0f, 5.0f);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 3.0f, 0.0f, 0.0f, 1.0f, 5.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4 & Matrix, const ZEVector3 & Position)")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Position(2.0f, 4.0f, 6.0f);

		ZEMatrix4x4::CreateTranslation(Matrix, Position);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f, 0.0f, 1.0f, 6.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateViewPortTransform(ZEMatrix4x4 & Matrix, float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)")
	{

	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateViewTransform(ZEMatrix4x4 & Matrix, const ZEVector3 & Position, const ZEQuaternion & Rotation)")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Position(3.0f, 4.0f, 5.0f);
		ZEQuaternion Rotation(ZE_PI_4, ZEVector3::UnitY);

		ZEMatrix4x4::CreateViewTransform(Matrix, Position, Rotation);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(0.70710673f, 0.0f, -0.70710680f, 1.41421382f, 0.0f, 1.0f, 0.0f, -4.0f, 0.70710680f, 0.0f, 0.70710673f, -5.65685411f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::CreateZero(ZEMatrix4x4 & Matrix)")
	{
		ZEMatrix4x4 Matrix;

		ZEMatrix4x4::CreateZero(Matrix);
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	}

	ZE_TEST_ITEM("float ZEMatrix4x4::Determinant() const")
	{
		ZEMatrix4x4 Matrix(10.f, 10.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f);

		float Result = Matrix.Determinant();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 198.0f);
	}

	ZE_TEST_ITEM("static float ZEMatrix4x4::Determinant(const ZEMatrix4x4 & Matrix)")
	{
		ZEMatrix4x4 Matrix(10.f, 10.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f);
			
		float Result = ZEMatrix4x4::Determinant(Matrix);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, 198.0f);
	}

	ZE_TEST_ITEM("bool ZEMatrix4x4::Equals(const ZEMatrix4x4& Matrix) const")
	{
		ZEMatrix4x4 MatrixA(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 13.0f, 21.0f,
			34.0f, 45.0f, 26.0f, 15.0f,
			4.0f, 17.0f, 50.0f, 3.0f);

		ZEMatrix4x4 MatrixB(1.00000001f, 1.00000001f, 2.00000001f, 3.00000001f,
			5.00000001f, 8.00000001f, 13.00000001f, 21.00000001f,
			34.00000001f, 45.00000001f, 26.00000001f, 15.00000001f,
			4.00000001f, 17.00000001f, 50.00000001f, 3.00000001f);

		ZEMatrix4x4 MatrixC(1.00001f, 1.00001f, 2.00001f, 3.00001f,
			5.00001f, 8.00001f, 13.00001f, 21.0001f,
			34.00001f, 45.00001f, 26.00001f, 15.00001f,
			4.00001f, 17.00001f, 50.00001f, 3.00001f);

		bool Equal = MatrixA.Equals(MatrixB);
		ZE_TEST_CHECK_ERROR(Equal);

		Equal = MatrixA.Equals(MatrixC);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("bool ZEMatrix4x4::Equals(const ZEMatrix4x4& Matrix, float Threshold) const")
	{
		float Threshold = 0.1f;

		ZEMatrix4x4 MatrixA(1.11f, 1.11f, 2.11f, 3.11f,
			5.11f, 8.11f, 13.11f, 21.11f,
			34.11f, 45.11f, 26.11f, 15.11f,
			4.11f, 17.11f, 50.11f, 3.11f);

		ZEMatrix4x4 MatrixB(1.1f, 1.1f, 2.1f, 3.1f,
			5.1f, 8.1f, 13.1f, 21.1f,
			34.1f, 45.1f, 26.1f, 15.1f,
			4.1f, 17.1f, 50.1f, 3.1f);

		ZEMatrix4x4 MatrixC(1.0f, 1.1f, 2.1f, 3.1f,
			5.1f, 8.1f, 13.1f, 21.1f,
			34.1f, 45.1f, 26.1f, 15.1f,
			4.1f, 17.1f, 50.1f, 3.1f);

		bool Equal = MatrixA.Equals(MatrixB, Threshold);
		ZE_TEST_CHECK_ERROR(Equal);

		Equal = MatrixA.Equals(MatrixC, Threshold);
		ZE_TEST_CHECK_ERROR(!Equal);
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::GetDecomposition(ZEVector3& Translation, ZEQuaternion& Rotation, ZEVector3& Scale, const ZEMatrix4x4& Matrix)")
	{
		ZEMatrix4x4 Matrix;
		ZEMatrix4x4::CreateOrientation(Matrix, ZEVector3(3.0f, 4.0f, 5.0f), ZEQuaternion(ZE_PI_2, ZEVector3::UnitX), ZEVector3(2.5f, 3.5f, 4.5f));

		ZEVector3 Position;
		ZEQuaternion Rotation;
		ZEVector3 Scale;

		ZEMatrix4x4::GetDecomposition(Position, Rotation, Scale, Matrix);

		ZEMatrix4x4 Result;
		ZEMatrix4x4::CreateOrientation(Result, Position, Rotation, Scale);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, Result);

		ZEMatrix4x4::CreateOrientation(Matrix, ZEVector3(10.0f, 5.0f, 26.0f), ZEQuaternion(ZE_PI_4, ZEVector3::UnitY), ZEVector3(2.0f, 2.0f, 2.0f));

		ZEMatrix4x4::GetDecomposition(Position, Rotation, Scale, Matrix);
		ZEMatrix4x4::CreateOrientation(Result, Position, Rotation, Scale);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, Result);

		ZEMatrix4x4::CreateOrientation(Matrix, ZEVector3(0.0f, 0.0f, 0.0f), ZEQuaternion(ZE_PI_8, ZEVector3::One), ZEVector3(5.0f, 1.0f, 1.0f));

		ZEMatrix4x4::GetDecomposition(Position, Rotation, Scale, Matrix);
		ZEMatrix4x4::CreateOrientation(Result, Position, Rotation, Scale);

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, Result);

		ZE_TEST_CASE("Checking whether the decomposition contains cumulative transformations.")
		{
			Matrix = ZEMatrix4x4::Identity;

			ZEVector3 ScaleComponent(3.0f, 4.0f, 5.0f);
			ZEQuaternion RotationComponent01(ZE_PI_4, ZEVector3::UnitX);
			ZEVector3 TranslationComponent01(1.0f, 2.0f, 3.0f);
			ZEQuaternion RotationComponent02(ZE_PI_4, ZEVector3::UnitY);
			ZEVector3 TranslationComponent02(3.0f, 6.0f, 9.0f);

			ZEMatrix4x4 ScaleMatrix;
			ZEMatrix4x4::CreateScale(ScaleMatrix, ScaleComponent);
			ZEMatrix4x4 MatrixIteration01 = ScaleMatrix * Matrix;

			ZEMatrix4x4 RotationMatrix01;
			ZEMatrix4x4::CreateRotation(RotationMatrix01, RotationComponent01);
			ZEMatrix4x4 MatrixIteration02 = RotationMatrix01 * MatrixIteration01;

			ZEMatrix4x4 TranslationMatrix01;
			ZEMatrix4x4::CreateTranslation(TranslationMatrix01, TranslationComponent01);
			ZEMatrix4x4 MatrixIteration03 = TranslationMatrix01 * MatrixIteration02;

			ZEMatrix4x4 RotationMatrix02;
			ZEMatrix4x4::CreateRotation(RotationMatrix02, RotationComponent02);
			ZEMatrix4x4 MatrixIteration04 = RotationMatrix02 * MatrixIteration03;

			ZEMatrix4x4 TranslationMatrix02;
			ZEMatrix4x4::CreateTranslation(TranslationMatrix02, TranslationComponent02);
			ZEMatrix4x4 MatrixIteration05 = TranslationMatrix02 * MatrixIteration04;

			ZEVector3 SampleVector(0.0f, 5.0f, 0.0f); 

			ZEVector3 SampleTransformedVector = MatrixIteration05 * SampleVector;

			ZEVector3 ResultPosition;
			ZEQuaternion ResultRotation;
			ZEVector3 ResultScale;

			ZEMatrix4x4::GetDecomposition(ResultPosition, ResultRotation, ResultScale, MatrixIteration05);

			ZEMatrix4x4 ResultCumulativeMatrix;
			ZEMatrix4x4::CreateOrientation(ResultCumulativeMatrix, ResultPosition, ResultRotation, ResultScale);

			ZEVector3 ResultTransformedVector = ResultCumulativeMatrix * SampleVector;

			ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultTransformedVector.x, SampleTransformedVector.x);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultTransformedVector.y, SampleTransformedVector.y);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultTransformedVector.z, SampleTransformedVector.z);
		}
	}

	ZE_TEST_ITEM("ZEVector3 ZEMatrix4x4::GetTranslation()")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Position(3.0f, 4.0f, 5.0f);
		ZEMatrix4x4::CreateOrientation(Matrix, Position, ZEQuaternion(ZE_PI_2, ZEVector3::UnitX), ZEVector3(2.5f, 3.5f, 4.5f));

		ZEVector3 ResultPosition = Matrix.GetTranslation();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultPosition.x, Position.x);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultPosition.y, Position.y);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultPosition.z, Position.z);
	}

	ZE_TEST_ITEM("ZEQuaternion ZEMatrix4x4::GetRotation()")
	{
		ZEMatrix4x4 Matrix;
		ZEQuaternion Rotation(ZE_PI_2, ZEVector3::UnitX);
		ZEMatrix4x4::CreateOrientation(Matrix, ZEVector3(3.0f, 4.0f, 5.0f), Rotation, ZEVector3(2.5f, 3.5f, 4.5f));

		ZEQuaternion ResultRotation = Matrix.GetRotation();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.w, Rotation.w);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.x, Rotation.x);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.y, Rotation.y);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultRotation.z, Rotation.z);
	}

	ZE_TEST_ITEM("ZEVector3 ZEMatrix4x4::GetScale()")
	{
		ZEMatrix4x4 Matrix;
		ZEVector3 Scale(2.5f, 3.5f, 4.5f);
		ZEMatrix4x4::CreateOrientation(Matrix, ZEVector3(3.0f, 4.0f, 5.0f), ZEQuaternion(ZE_PI_2, ZEVector3::UnitX), Scale);

		ZEVector3 ResultScale = Matrix.GetScale();

		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.x, Scale.x);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.y, Scale.y);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(ResultScale.z, Scale.z);
	}

	ZE_TEST_ITEM("ZEMatrix4x4 ZEMatrix4x4::Inverse() const")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
			
		ZEMatrix4x4 Result = Matrix.Inverse();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix4x4(0.0f, 0.042553191489f, 0.09574468085106f, -0.095744680851063f, 0.33333333333333f, -1.5744680851063f, -0.2092198581560f, 1.5425531914893f, -0.66666666666f, 3.76595744680f, 0.0567375886524f, -3.72340425531f, 0.3333333333333f, -2.12765957446f, 0.0460992907801f, 2.28723404255319f));			
	}

	ZE_TEST_ITEM("static bool ZEMatrix4x4::Inverse(ZEMatrix4x4 & Out, const ZEMatrix4x4 & Matrix)")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f, 10.0f, 9.0f, 9.0f, 9.0f, 10.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f);
		ZEMatrix4x4 Out;

		ZEMatrix4x4::Inverse(Out, Matrix);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEMatrix4x4(0.0f, 0.042553191489f, 0.09574468085106f, -0.095744680851063f, 0.33333333333333f, -1.5744680851063f, -0.2092198581560f, 1.5425531914893f, -0.66666666666f, 3.76595744680f, 0.0567375886524f, -3.72340425531f, 0.3333333333333f, -2.12765957446f, 0.0460992907801f, 2.28723404255319f));		
	}

	ZE_TEST_ITEM("static inline void ZEMatrix4x4::Multiply(ZEMatrix4x4 & Out, const ZEMatrix4x4 & A, const ZEMatrix4x4 & B)")
	{
		ZEMatrix4x4 A(35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, 9.0f, 
			10.0f, 1.0f, 2.0f, 3.0f, 
			4.0f, 5.0f, 6.0f, 7.0f);

		ZEMatrix4x4 B(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 13.0f, 21.0f,
			34.0f, 55.0f, 1.0f, 1.0f,
			2.0f, 3.0f, 5.0f, 8.0f);

		ZEMatrix4x4 Out;

		ZEMatrix4x4::Multiply(Out, A, B);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Out, ZEMatrix4x4(718.0f, 1135.0f, 445.0f, 700.0f, 379.0f, 604.0f, 191.0f, 300.0f, 89.0f, 137.0f, 50.0f, 77.0f, 247.0f, 395.0f, 114.0f, 179.0f));	
	}

	ZE_TEST_ITEM("static inline void ZEMatrix4x4::Scale(ZEMatrix4x4 & Out, const ZEMatrix4x4 & A, float s)")
	{
		ZEMatrix4x4 A(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 55.0f, 1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f);
		ZEMatrix4x4 Out;
		float s = 2.0f;

		ZEMatrix4x4::Scale(Out, A, s);
		ZE_TEST_CHECK_EQUAL(Out, ZEMatrix4x4(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f, 110.0f, 2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f));					
	}

	ZE_TEST_ITEM("static inline void ZEMatrix4x4::Sub(ZEMatrix4x4 & Out, const ZEMatrix4x4 & A, const ZEMatrix4x4 & B)")
	{
		ZEMatrix4x4 A(13.0f, 8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f, 34.0f,
			21.0f, 13.0f, 8.0f, 5.0f,
			3.0f, 2.0f, 1.0f, 1.0f);

		ZEMatrix4x4 B(1.0f, 2.0f, 3.0f, 4.0f,
			5.0f, 6.0f, 7.0f, 8.0f,
			9.0f, 10.0f, 11.0f, 12.0f,
			13.0f, 14.0f, 15.0f, 16.0f);

		ZEMatrix4x4 Out;

		ZEMatrix4x4::Sub(Out, A, B);
		ZE_TEST_CHECK_EQUAL(Out, ZEMatrix4x4(12.0f, 6.0f, 2.0f, -1.0f, -3.0f, -5.0f, -6.0f, 26.0f, 12.0f, 3.0f, -3.0f, -7.0f, -10.0f, -12.0f, -14.0f, -15.0f));			
	}

	ZE_TEST_ITEM(" static void ZEMatrix4x4::Transform(ZEVector3 & Out, const ZEMatrix4x4 & Matrix, const ZEVector3 & Vector)")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, 9.0f, 
			10.0f, 1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f, 7.0f);

		ZEVector3 Vector(1.0f, 2.0f, 3.0f);
		ZEVector3 Out;

		ZEMatrix4x4::Transform(Out, Matrix, Vector);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector3(141.0f, 64.0f, 21.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::Transform(ZEVector4 & Out, const ZEMatrix4x4 & Matrix, const ZEVector4 & Vector)")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, 9.0f,
			10.0f, 1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f, 7.0f);

		ZEVector4 Vector(1.0f, 2.0f, 3.0f, 1.0f);
		ZEVector4 Out;

		ZEMatrix4x4::Transform(Out, Matrix, Vector);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector4(141.0f, 64.0f, 21.0f, 39.0f));
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::Transform3x3(ZEVector3 & Out, const ZEMatrix4x4 & Matrix, const ZEVector3 & Vector)")
	{
		ZEVector3 Vector(3.0f, 5.0f, 7.0f);
		ZEVector3 Out;
		ZEMatrix4x4 Matrix(13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f, 34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f);
			
		ZEMatrix4x4::Transform3x3(Out, Matrix, Vector);
		ZE_TEST_CHECK_EQUAL(Out, ZEVector3(114.0f, 18.0f, 184.0f));
	}

	ZE_TEST_ITEM("ZEMatrix4x4 ZEMatrix4x4::Transpose() const")
	{
		ZEMatrix4x4 Matrix(1.0f, 3.0f, 4.0f, 5.0f, 2.0f, 6.0f, 3.0f, 7.0f, 3.0f, 8.0f, 6.0f, 9.0f, 12.0f, 7.0f, 4.0f, 1.0f);
			
		ZEMatrix4x4 Result = Matrix.Transpose();
		ZE_TEST_CHECK_EQUAL(Result, ZEMatrix4x4(1.0f, 2.0f, 3.0f, 12.0f, 3.0f, 6.0f, 8.0f, 7.0f, 4.0f, 3.0f, 6.0f, 4.0f, 5.0f, 7.0f, 9.0f, 1.0f));				
	}

	ZE_TEST_ITEM("static void ZEMatrix4x4::Transpose(ZEMatrix4x4 & Out, const ZEMatrix4x4 & Matrix)")
	{
		ZEMatrix4x4 Matrix(1.0f, 3.0f, 4.0f, 5.0f, 2.0f, 6.0f, 3.0f, 7.0f, 3.0f, 8.0f, 6.0f, 9.0f, 12.0f, 7.0f, 4.0f, 1.0f);
		ZEMatrix4x4 Out;

		ZEMatrix4x4::Transpose(Out, Matrix);
		ZE_TEST_CHECK_EQUAL(Out, ZEMatrix4x4(1.0f, 2.0f, 3.0f, 12.0f, 3.0f, 6.0f, 8.0f,  7.0f, 4.0f, 3.0f, 6.0f,  4.0f, 5.0f, 7.0f, 9.0f,  1.0f));	
	}

	ZE_TEST_ITEM("void ZEMatrix4x4::TransposeSelf()")
	{
		ZEMatrix4x4 Matrix(1.0f, 3.0f, 4.0f, 5.0f, 2.0f, 6.0f, 3.0f, 7.0f, 3.0f, 8.0f, 6.0f, 9.0f, 12.0f, 7.0f, 4.0f, 1.0f);
		
		Matrix.TransposeSelf();
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(1.0f, 2.0f, 3.0f, 12.0f, 3.0f, 6.0f, 8.0f,  7.0f, 4.0f, 3.0f, 6.0f,  4.0f, 5.0f, 7.0f, 9.0f,  1.0f));		
	}

	ZE_TEST_ITEM("ZEMatrix4x4 ZEMatrix4x4::operator-(const ZEMatrix4x4 & RightOperand) const")
	{
		ZEMatrix4x4 Matrix(13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f, 34.0f, 21.0f, 13.0f, 8.0f, 5.0f, 3.0f, 2.0f, 1.0f, 1.0f);
		ZEMatrix4x4 RightOperand(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
		ZEMatrix4x4 Result;

		Result = Matrix - RightOperand;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix4x4(12.0f, 6.0f, 2.0f, -1.0f, -3.0f, -5.0f, -6.0f, 26.0f, 12.0f, 3.0f, -3.0f, -7.0f, -10.0f, -12.0f, -14.0f, -15.0f));		
	}

	ZE_TEST_ITEM("bool ZEMatrix4x4::operator!=(const ZEMatrix4x4 & M) const")
	{
		ZE_TEST_CASE("False")
		{
			ZEMatrix4x4 A(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 45.0f, 26.0f, 15.0f, 4.0f, 17.0f, 50.0f, 3.0f);
			ZEMatrix4x4 B(1.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 45.0f, 26.0f, 15.0f, 4.0f, 17.0f, 50.0f, 3.0f); 
			
			bool Result = (A != B);
			ZE_TEST_CHECK_ERROR(Result == false);
		}

		ZE_TEST_CASE("True")
		{
			ZEMatrix4x4 A(3.0f, 1.0f, 2.0f, 3.0f, 5.0f, 8.0f, 13.0f, 21.0f, 34.0f, 40.0f, 18.0f, 20.0f, 23.0f, 42.0f, 10.0f, 6.0f);
			ZEMatrix4x4 B(1.0f, 5.0f, 8.0f, 9.0f, 5.0f, 8.0f, 34.0f, 16.0f, 34.0f, 45.0f, 12.0f, 34.0f, 21.0f, 42.0f, 10.0f, 7.0f);
				
			bool Result = (A != B);
			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("ZEMatrix4x4 ZEMatrix4x4::operator*(const ZEMatrix4x4 & RightOperand) const")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, 9.0f, 
			10.0f, 1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f, 7.0f);

		ZEMatrix4x4 RightOperand(1.0f, 1.0f, 2.0f, 3.0f, 
			5.0f, 8.0f, 13.0f, 21.0f, 
			34.0f, 55.0f, 1.0f, 1.0f,
			2.0f, 3.0f, 5.0f, 8.0f);

		ZEMatrix4x4 Result;

		Result = Matrix * RightOperand;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix4x4(718.0f, 1135.0f, 445.0f, 700.0f, 379.0f, 604.0f, 191.0f, 300.0f, 89.0f, 137.0f, 50.0f, 77.0f, 247.0f, 395.0f, 114.0f, 179.0f));		
	}

	ZE_TEST_ITEM("ZEVector3 ZEMatrix4x4::operator*(const ZEVector3 & RightOperand) const")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, 9.0f, 
			10.0f, 1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f, 7.0f);

		ZEVector3 RightOperand(1.0f, 2.0f, 3.0f);
		ZEVector3 Result;

		Result = Matrix * RightOperand;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector3(141.0f, 64.0f, 21.0f));
	}

	ZE_TEST_ITEM("ZEVector4 ZEMatrix4x4::operator*(const ZEVector4 & RightOperand) const")
	{
		ZEMatrix4x4 Matrix(13.0f, 8.0f, 5.0f, 3.0f,
					2.0f, 1.0f, 1.0f, 34.0f,
					21.0f, 13.0f, 8.0f, 5.0f,
					3.0f, 2.0f, 1.0f, 1.0f);

		ZEVector4 RightOperand(3.0f, 5.0f, 7.0f, 9.0f);
		ZEVector4 Result;

		Result = Matrix * RightOperand;
		ZE_TEST_CHECK_EQUAL(Result, ZEVector4(117.0f, 52.0f, 189.0f, 27.0f));
	}

	ZE_TEST_ITEM("ZEMatrix4x4 ZEMatrix4x4::operator*(float S)")
	{
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 13.0f, 21.0f,
			34.0f, 55.0f, 1.0f, 1.0f,
			2.0f, 3.0f, 5.0f, 8.0f);

		ZEMatrix4x4 Result;
		float S = 2.0f;

		Result = Matrix * S;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Result, ZEMatrix4x4(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f, 110.0f, 2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix4x4::operator*=(const ZEMatrix4x4 & RightOperand)")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f,
					  10.0f, 9.0f, 9.0f, 9.0f, 
					  10.0f, 1.0f, 2.0f, 3.0f, 
					  4.0f, 5.0f, 6.0f, 7.0f);

		ZEMatrix4x4 RightOperand(1.0f, 1.0f, 2.0f, 3.0f,  
				      5.0f, 8.0f, 13.0f, 21.0f,
					  34.0f, 55.0f, 1.0f, 1.0f, 
					  2.0f, 3.0f, 5.0f, 8.0f);

		Matrix *= RightOperand;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(718.0f, 1135.0f, 445.0f, 700.0f, 379.0f, 604.0f, 191.0f, 300.0f, 89.0f, 137.0f, 50.0f, 77.0f, 247.0f, 395.0f, 114.0f, 179.0f));
			
	}

	ZE_TEST_ITEM("void ZEMatrix4x4::operator*=(float S)")
	{
		ZEMatrix4x4 Matrix(1.0f, 1.0f, 2.0f, 3.0f,
			5.0f, 8.0f, 13.0f, 21.0f,
			34.0f, 55.0f, 1.0f, 1.0f, 
			2.0f, 3.0f, 5.0f, 8.0f);

		float S = 2.0f;

		Matrix *= S;
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Matrix, ZEMatrix4x4(2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f, 26.0f, 42.0f, 68.0f, 110.0f, 2.0f, 2.0f, 4.0f, 6.0f, 10.0f, 16.0f));
	}

	ZE_TEST_ITEM(" float & ZEMatrix4x4::operator[](ZESize Index)")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, 9.0f,
			10.0f, 1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f, 7.0f);

		float M11 = Matrix[0];
		float M21 = Matrix[1];
		float M31 = Matrix[2];
		float M41 = Matrix[3];
		ZE_TEST_CHECK_EQUAL(M11, 35.0f);
		ZE_TEST_CHECK_EQUAL(M21, 10.0f);
		ZE_TEST_CHECK_EQUAL(M31, 10.0f);
		ZE_TEST_CHECK_EQUAL(M41, 4.0f);
	}

	ZE_TEST_ITEM("float ZEMatrix4x4::operator[](ZESize Index) const")
	{
		ZEMatrix4x4 Matrix(35.0f, 23.0f, 16.0f, 12.0f,
			10.0f, 9.0f, 9.0f, 9.0f,
			10.0f, 1.0f, 2.0f, 3.0f,
			4.0f, 5.0f, 6.0f, 7.0f);

		float M11 = Matrix[0];
		float M21 = Matrix[1];
		float M31 = Matrix[2];
		float M41 = Matrix[3];
		ZE_TEST_CHECK_EQUAL(M11, 35.0f);
		ZE_TEST_CHECK_EQUAL(M21, 10.0f);
		ZE_TEST_CHECK_EQUAL(M31, 10.0f);
		ZE_TEST_CHECK_EQUAL(M41, 4.0f);
	}

	ZE_TEST_ITEM("ZEMatrix4x4 ZEMatrix4x4::operator+(const ZEMatrix4x4 & RightOperand) const")
	{
		ZEMatrix4x4 Matrix(13.0f, 8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f, 34.0f, 
			21.0f, 13.0f, 8.0f, 5.0f,
			3.0f, 2.0f, 1.0f, 1.0f);

		ZEMatrix4x4 RightOperand(1.0f, 2.0f, 3.0f, 4.0f,
			5.0f, 6.0f, 7.0f, 8.0f, 
			9.0f, 10.0f, 11.0f, 12.0f, 
			13.0f, 14.0f, 15.0f, 16.0f);

		ZEMatrix4x4 Result;

		Result = Matrix + RightOperand;
		ZE_TEST_CHECK_EQUAL(Result, ZEMatrix4x4(14.0f, 10.0f, 8.0f, 7.0f, 7.0f, 7.0f, 8.0f, 42.0f, 30.0f, 23.0f, 19.0f, 17.0f, 16.0f, 16.0f, 16.0f, 17.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix4x4::operator+=(const ZEMatrix4x4 & RightOperand)")
	{
		ZEMatrix4x4 Matrix(13.0f, 8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f, 34.0f,
			21.0f, 13.0f, 8.0f, 5.0f,
			3.0f, 2.0f, 1.0f, 1.0f);

		ZEMatrix4x4 RightOperand(1.0f, 2.0f, 3.0f, 4.0f,
			5.0f, 6.0f, 7.0f, 8.0f,
			9.0f, 10.0f, 11.0f, 12.0f,
			13.0f, 14.0f, 15.0f, 16.0f);

		Matrix += RightOperand;
		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(14.0f, 10.0f, 8.0f, 7.0f, 7.0f, 7.0f, 8.0f, 42.0f, 30.0f, 23.0f, 19.0f, 17.0f, 16.0f, 16.0f, 16.0f, 17.0f));
	}

	ZE_TEST_ITEM("void ZEMatrix4x4::operator-=(const ZEMatrix4x4 & RightOperand)")
	{
		ZEMatrix4x4 Matrix(13.0f, 8.0f, 5.0f, 3.0f,
			2.0f, 1.0f, 1.0f, 34.0f,
			21.0f, 13.0f, 8.0f, 5.0f,
			3.0f, 2.0f, 1.0f, 1.0f);

		ZEMatrix4x4 RightOperand(1.0f, 2.0f, 3.0f, 4.0f,
			5.0f, 6.0f, 7.0f, 8.0f,
			9.0f, 10.0f, 11.0f, 12.0f,
			13.0f, 14.0f, 15.0f, 16.0f);

		Matrix -= RightOperand;

		ZE_TEST_CHECK_EQUAL(Matrix, ZEMatrix4x4(12.0f, 6.0f, 2.0f, -1.0f, -3.0f, -5.0f, -6.0f, 26.0f, 12.0f, 3.0f, -3.0f, -7.0f, -10.0f, -12.0f, -14.0f, -15.0f));
	}

	ZE_TEST_ITEM(" bool ZEMatrix4x4::operator==(const ZEMatrix4x4 & M) const")
	{
		ZE_TEST_CASE("True")
		{
			ZEMatrix4x4 A(1.0f, 1.0f, 2.0f, 3.0f,
				5.0f, 8.0f, 13.0f, 21.0f,
				34.0f, 45.0f, 26.0f, 15.0f,
				4.0f, 17.0f, 50.0f, 3.0f);

			ZEMatrix4x4 B(1.0f, 1.0f, 2.0f, 3.0f,
				5.0f, 8.0f, 13.0f, 21.0f,
				34.0f, 45.0f, 26.0f, 15.0f,
				4.0f, 17.0f, 50.0f, 3.0f);

			bool Result = (A == B);
			ZE_TEST_CHECK_ERROR(Result);
		}

		ZE_TEST_CASE("False")
		{
			ZEMatrix4x4 A(3.0f, 1.0f, 2.0f, 3.0f,
				5.0f, 8.0f, 13.0f, 21.0f,
				34.0f, 40.0f, 18.0f, 20.0f,
				23.0f, 42.0f, 10.0f, 6.0f);

			ZEMatrix4x4 B(1.0f, 5.0f, 8.0f, 9.0f,
				5.0f, 8.0f, 34.0f, 16.0f,
				34.0f, 45.0f, 12.0f, 34.0f,
				21.0f, 42.0f, 10.0f, 7.0f);

			bool Result = (A == B);
			ZE_TEST_CHECK_ERROR(Result == false);
		}
	}		
}
