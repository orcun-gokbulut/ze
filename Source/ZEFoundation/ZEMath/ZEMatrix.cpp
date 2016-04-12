//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrix.cpp
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

#include "ZEMatrix.h"
#include "ZEVector.h"
#include "ZEQuaternion.h"
#include "ZEMath.h"
#include <memory.h>
#include "ZEMath/ZEAngle.h"
#include "ZEError.h"

#define ZE_MATRIX4X4_ROW_ELEMENT_COUNT		(ZESize)4
#define ZE_MATRIX4X4_COL_ELEMENT_COUNT		(ZESize)4

#define SWAP(x, y) temp = (x); (x) = (y); (y) = temp

// ZEMatrix3x3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEMatrix3x3 ZEMatrix3x3::Zero = ZEMatrix3x3(0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f);

const ZEMatrix3x3 ZEMatrix3x3::Identity = ZEMatrix3x3(1.0f, 0.0f, 0.0f,
													  0.0f, 1.0f, 0.0f,
													  0.0f, 0.0f, 1.0f);

inline void ZEMatrix3x3::Create(ZEMatrix3x3 &Matrix, float M11, float M12, float M13,
														float M21, float M22, float M23,
														float M31, float M32, float M33)
{
	Matrix.M11 = M11; Matrix.M12 = M12; Matrix.M13 = M13;
	Matrix.M21 = M21; Matrix.M22 = M22; Matrix.M23 = M23;
	Matrix.M31 = M31; Matrix.M32 = M32; Matrix.M33 = M33;
}

inline void ZEMatrix3x3::Create(ZEMatrix3x3& Matrix, float M[9])
{
	memcpy(Matrix.M, M, sizeof(ZEMatrix3x3));
}

void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, const ZEQuaternion& Rotation)
{
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;

	Matrix.M31 = 		2.0f * xz -	2.0f * yw;
	Matrix.M32 = 		2.0f * yz +	2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx -	2.0f * yy;
}

void ZEMatrix3x3::CreateRotation(ZEMatrix3x3& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder)
{
	ZEMatrix3x3 XRotation, YRotation, ZRotation, Temp;

	ZEMatrix3x3::CreateRotationX(XRotation, Pitch);
	ZEMatrix3x3::CreateRotationY(YRotation, Yawn);
	ZEMatrix3x3::CreateRotationZ(ZRotation, Roll);

	switch(RotationOrder)
	{
		case ZE_RO_XYZ:
			ZEMatrix3x3::Multiply(Temp, XRotation, YRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_XZY:
			ZEMatrix3x3::Multiply(Temp, XRotation, ZRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_YXZ:
			ZEMatrix3x3::Multiply(Temp, YRotation, XRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_YZX:
			ZEMatrix3x3::Multiply(Temp, YRotation, ZRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, XRotation);
			break;

		case ZE_RO_ZXY:
			ZEMatrix3x3::Multiply(Temp, ZRotation, XRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_ZYX:
			ZEMatrix3x3::Multiply(Temp, ZRotation, YRotation);
			ZEMatrix3x3::Multiply(Matrix, Temp, XRotation);
			break;
	}
}

void ZEMatrix3x3::CreateRotationX(ZEMatrix3x3& Matrix, float Pitch)
{
	float Cos = ZEAngle::Cos(Pitch);
	float Sin = ZEAngle::Sin(Pitch);

	Matrix.M11 = 1.0f;
	Matrix.M12 = 0.0f;
	Matrix.M13 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = Cos;
	Matrix.M23 = -Sin;

	Matrix.M31 = 0.0f;
	Matrix.M32 = Sin;
	Matrix.M33 = Cos;
}

void ZEMatrix3x3::CreateRotationY(ZEMatrix3x3& Matrix, float Yawn)
{
	float Cos = ZEAngle::Cos(Yawn);
	float Sin = ZEAngle::Sin(Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0f;
	Matrix.M13 = Sin;

	Matrix.M21 = 0.0f;
	Matrix.M22 = 1.0f;
	Matrix.M23 = 0.0f;

	Matrix.M31 = -Sin;
	Matrix.M32 = 0.0f;
	Matrix.M33 = Cos;
}

void ZEMatrix3x3::CreateRotationZ(ZEMatrix3x3& Matrix, float Roll)
{
	float Cos = ZEAngle::Cos(Roll);
	float Sin = ZEAngle::Sin(Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = -Sin;
	Matrix.M13 = 0.0f;

	Matrix.M21 = Sin;
	Matrix.M22 = Cos;
	Matrix.M23 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = 0.0f;
	Matrix.M33 = 1.0f;
}

void ZEMatrix3x3::CreateIdentity(ZEMatrix3x3& Matrix)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f);
}

void ZEMatrix3x3::CreateZero(ZEMatrix3x3& Matrix)
{
	Create(Matrix,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f);
}

void ZEMatrix3x3::CreateScale(ZEMatrix3x3& Matrix, float x, float y, float z)
{
	Create(Matrix,
			x, 0.0f, 0.0f,
			0.0f, y, 0.0f,
			0.0f, 0.0f, z);
}

void ZEMatrix3x3::CreateScale(ZEMatrix3x3& Matrix, const ZEVector3& Scale)
{
	Create(Matrix,
		Scale.x, 0.0f, 0.0f,
		0.0f, Scale.y, 0.0f,
		0.0f, 0.0f, Scale.z);
}


void ZEMatrix3x3::CreateTranslation2D(ZEMatrix3x3& Matrix, float x, float y)
{
	Create(Matrix,
		1.0f, 0.0f, x,
		0.0f, 1.0f, y,
		0.0f, 0.0f, 1.0f);

}

void ZEMatrix3x3::CreateTranslation2D(ZEMatrix3x3& Matrix, const ZEVector2& Position)
{
	Create(Matrix,
		1.0f, 0.0f, Position.x,
		0.0f, 1.0f, Position.y,
		0.0f, 0.0f, 1.0f);

}

void ZEMatrix3x3::CreateRotation2D(ZEMatrix3x3& Matrix, float Angle)
{
	float CosAngle = ZEAngle::Cos(Angle);
	float SinAngle = ZEAngle::Sin(Angle);

	Matrix.M11 = CosAngle;
	Matrix.M12 = -SinAngle;
	Matrix.M13 = 0.0f;

	Matrix.M21 = SinAngle;
	Matrix.M22 = CosAngle;
	Matrix.M23 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = 0.0f;
	Matrix.M33 = 1.0f;
}

void ZEMatrix3x3::CreateScale2D(ZEMatrix3x3& Matrix, float x, float y)
{
	Create(Matrix,
		x, 0.0f, 0.0f,
		0.0f, y, 0.0f,
		0.0f, 0.0f, 1.0f);
}

void ZEMatrix3x3::CreateScale2D(ZEMatrix3x3& Matrix, const ZEVector2& Scale)
{
	Create(Matrix,
		Scale.x, 0.0f, 0.0f,
		0.0f, Scale.y, 0.0f,
		0.0f, 0.0f, 1.0f);
}

void ZEMatrix3x3::CreateOrientation2D(ZEMatrix3x3& Matrix, const ZEVector2& Translation, float Angle, const ZEVector2& Scale)
{
	float Cos = ZEAngle::Cos(Angle);
	float Sin = ZEAngle::Sin(Angle);

	Matrix.M11 = Cos * Scale.x;
	Matrix.M12 = -Sin * Scale.y;
	Matrix.M13 = Translation.x;

	Matrix.M21 = Sin * Scale.x;
	Matrix.M22 = Cos * Scale.y;
	Matrix.M23 = Translation.y;

	Matrix.M31 = 0.0f;
	Matrix.M32 = 0.0f;
	Matrix.M33 = 1.0f;
}

float ZEMatrix3x3::Determinant() const
{
	return ZEMatrix3x3::Determinant(*this);
}

ZEMatrix3x3 ZEMatrix3x3::Inverse() const
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Inverse(Temp, *this);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::Transpose() const
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Transpose(Temp, *this);
	return Temp;
}

void ZEMatrix3x3::InverseSelf()
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Inverse(Temp, *this);
	*this = Temp;
}

void ZEMatrix3x3::TransposeSelf()
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Transpose(Temp, *this);
	*this = Temp;
}

bool ZEMatrix3x3::Equals(const ZEMatrix3x3& Matrix) const
{
	for (ZESize I = 0; I < 9; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > ZE_ZERO_THRESHOLD)
			return false;

	return true;
}

bool ZEMatrix3x3::Equals(const ZEMatrix3x3& Matrix, float Threshold)
{
	for (ZESize I = 0; I < 9; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > Threshold)
			return false;

	return true;
}

ZEQuaternion ZEMatrix3x3::GetRotation()
{
	ZEVector3 TempScale;
	TempScale.x = ZEVector3(M11, M21, M31).Length();
	TempScale.y = ZEVector3(M12, M22, M32).Length();
	TempScale.z = ZEVector3(M13, M23, M33).Length();

	ZEMatrix3x3 ScaleMatrix;
	ZEMatrix3x3::CreateScale(ScaleMatrix, TempScale.x, TempScale.y, TempScale.z);

	ZEMatrix3x3 RotationMatrix = (*this) * ScaleMatrix.Inverse();

	ZEQuaternion Rotation;
	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix);
	return Rotation;
}

ZEVector3 ZEMatrix3x3::GetScale()
{
	ZEVector3 Scale;

	Scale.x = ZEVector3(M11, M21, M31).Length();
	Scale.y = ZEVector3(M12, M22, M32).Length();
	Scale.z = ZEVector3(M13, M23, M33).Length();

	return Scale;
}

void ZEMatrix3x3::GetDecomposition(ZEQuaternion& Rotation, ZEVector3& Scale, const ZEMatrix3x3& Matrix)
{
	Scale.x = ZEVector3(Matrix.M11, Matrix.M21, Matrix.M31).Length();
	Scale.y = ZEVector3(Matrix.M12, Matrix.M22, Matrix.M32).Length();
	Scale.z = ZEVector3(Matrix.M13, Matrix.M23, Matrix.M33).Length();

	ZEMatrix3x3 ScaleMatrix;
	ZEMatrix3x3::CreateScale(ScaleMatrix, Scale.x, Scale.y, Scale.z);

	ZEMatrix3x3 RotationMatrix = Matrix * ScaleMatrix.Inverse();

	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix);
}

ZEVector2 ZEMatrix3x3::Get2DTranslation()
{
	return ZEVector2(M13, M23);
}

float ZEMatrix3x3::Get2DRotation()
{
	return ZEAngle::ArcTan2(M12, M22);
}

ZEVector2 ZEMatrix3x3::Get2DScale()
{
	ZEVector2 Scale;

	Scale.x = ZEVector2(M11, M21).Length();
	Scale.y = ZEVector2(M12, M22).Length();

	return Scale;
}

void ZEMatrix3x3::Get2DDecomposition(ZEVector2& Translation, float& Rotation, ZEVector2& Scale, const ZEMatrix3x3& Matrix)
{
	Translation.x = Matrix.M13;
	Translation.y = Matrix.M23;

	Scale.x = ZEVector2(Matrix.M11, Matrix.M21).Length();
	Scale.y = ZEVector2(Matrix.M12, Matrix.M22).Length();

	Rotation = ZEAngle::ArcTan2(Matrix.M12, Matrix.M22);
}


ZEMatrix3x3Shader ZEMatrix3x3::ToMatrix3x3Shader()
{
	ZEMatrix3x3Shader Matrix;
	Matrix.M11 = M11;
	Matrix.M21 = M21;
	Matrix.M31 = M31;
	Matrix.Unused0 = 0.0f;

	Matrix.M12 = M12;
	Matrix.M22 = M22;
	Matrix.M32 = M32;
	Matrix.Unused1 = 0.0f;

	Matrix.M13 = M13;
	Matrix.M23 = M23;
	Matrix.M33 = M33;
	Matrix.Unused2 = 0.0f;

	return Matrix;
}

ZEMatrix3x3 ZEMatrix3x3::operator+(const ZEMatrix3x3 &RightOperand) const 
{
	ZEMatrix3x3 Temp;
	Add(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::operator-(const ZEMatrix3x3 &RightOperand) const 
{
	ZEMatrix3x3 Temp;
	Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::operator*(const ZEMatrix3x3 &RightOperand) const 
{
	ZEMatrix3x3 Temp;
	Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3 ZEMatrix3x3::operator*(float S) 
{
	ZEMatrix3x3 Temp;
	Scale(Temp, *this, S);
	return Temp;
}

ZEVector2 ZEMatrix3x3::operator*(const ZEVector2 &RightOperand) const
{
	ZEVector2 Temp;
	ZEMatrix3x3::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEMatrix3x3::operator*(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEMatrix3x3::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEMatrix3x3::operator*(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEMatrix3x3::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3& ZEMatrix3x3::operator += (const ZEMatrix3x3 &RightOperand)
{
	Add(*this, *this, RightOperand);
	return *this;
}

ZEMatrix3x3& ZEMatrix3x3::operator -= (const ZEMatrix3x3 &RightOperand)
{
	Sub(*this, *this, RightOperand);
	return *this;
}

ZEMatrix3x3& ZEMatrix3x3::operator *= (const ZEMatrix3x3 &RightOperand)
{
	ZEMatrix3x3 Temp;
	Multiply(Temp, *this, RightOperand);
	return *this = Temp;
}

ZEMatrix3x3& ZEMatrix3x3::operator *= (float S)
{
	Scale(*this, *this, S);
	return *this;
}

bool ZEMatrix3x3::operator == (const ZEMatrix3x3 &M) const 
{
	return Equals(M);
}

bool ZEMatrix3x3::operator != (const ZEMatrix3x3 &M) const 
{
	return !Equals(M);
}

float ZEMatrix3x3::operator[](ZESize Index) const
{
	return MA[Index];
}

float& ZEMatrix3x3::operator[](ZESize Index)
{
	return MA[Index];
}

ZEMatrix3x3::ZEMatrix3x3(float M00, float M01, float M02,
			float M10, float M11, float M12,
			float M20, float M21, float M22)
{
	Create(*this, M00, M01, M02, M10, M11, M12, M20, M21, M22);  
}

ZEMatrix3x3::ZEMatrix3x3(float M[9])
{
	Create(*this, M);
}

ZEMatrix3x3::ZEMatrix3x3()
{
}

ZEMatrix3x3 operator*(float S, const ZEMatrix3x3& RightOperand)
{
	ZEMatrix3x3 Temp;
	ZEMatrix3x3::Scale(Temp, RightOperand, S);
	return Temp;
}

// ZEMatrix4x4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEMatrix4x4 ZEMatrix4x4::Zero = ZEMatrix4x4(0.0f, 0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f, 0.0f,
												  0.0f, 0.0f, 0.0f, 0.0f);

const ZEMatrix4x4 ZEMatrix4x4::Identity = ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
													  0.0f, 1.0f, 0.0f, 0.0f,
													  0.0f, 0.0f, 1.0f, 0.0f,
													  0.0f, 0.0f, 0.0f, 1.0f);

inline void ZEMatrix4x4::Create(ZEMatrix4x4& Matrix,
				float M11, float M12, float M13, float M14,
				float M21, float M22, float M23, float M24,
				float M31, float M32, float M33, float M34,
				float M41, float M42, float M43, float M44)
{
	Matrix.M11 = M11; Matrix.M21 = M21; Matrix.M31 = M31; Matrix.M41 = M41;
	Matrix.M12 = M12; Matrix.M22 = M22; Matrix.M32 = M32; Matrix.M42 = M42;
	Matrix.M13 = M13; Matrix.M23 = M23; Matrix.M33 = M33; Matrix.M43 = M43;
	Matrix.M14 = M14; Matrix.M24 = M24; Matrix.M34 = M34; Matrix.M44 = M44;
}

inline void ZEMatrix4x4::Create(ZEMatrix4x4& Matrix, float M[16])
{
	memcpy(Matrix.M, M, sizeof(ZEMatrix4x4));
}

void ZEMatrix4x4::CreateRotation(ZEMatrix4x4& Matrix, const ZEQuaternion& Rotation)
{
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;
	Matrix.M24 = 0.0f;

	Matrix.M31 = 		2.0f * xz - 2.0f * yw;
	Matrix.M32 = 		2.0f * yz + 2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx - 2.0f * yy;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateRotation(ZEMatrix4x4& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder)
{
	ZEMatrix4x4 XRotation, YRotation, ZRotation, Temp;

	ZEMatrix4x4::CreateRotationX(XRotation, Pitch);
	ZEMatrix4x4::CreateRotationY(YRotation, Yawn);
	ZEMatrix4x4::CreateRotationZ(ZRotation, Roll);

	switch(RotationOrder)
	{
	case ZE_RO_XYZ:
		ZEMatrix4x4::Multiply(Temp, XRotation, YRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, ZRotation);
		break;

	case ZE_RO_XZY:
		ZEMatrix4x4::Multiply(Temp, XRotation, ZRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, YRotation);
		break;

	case ZE_RO_YXZ:
		ZEMatrix4x4::Multiply(Temp, YRotation, XRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, ZRotation);
		break;

	case ZE_RO_YZX:
		ZEMatrix4x4::Multiply(Temp, YRotation, ZRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, XRotation);
		break;

	case ZE_RO_ZXY:
		ZEMatrix4x4::Multiply(Temp, ZRotation, XRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, YRotation);
		break;

	case ZE_RO_ZYX:
		ZEMatrix4x4::Multiply(Temp, ZRotation, YRotation);
		ZEMatrix4x4::Multiply(Matrix, Temp, XRotation);
		break;
	}
}

void ZEMatrix4x4::CreateRotationX(ZEMatrix4x4& Matrix, float Pitch)
{
	float Cos = ZEAngle::Cos(Pitch);
	float Sin = ZEAngle::Sin(Pitch);

	Matrix.M11 = 1.0f;
	Matrix.M12 = 0.0f;
	Matrix.M13 = 0.0f;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = Cos;
	Matrix.M23 = -Sin;
	Matrix.M24 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = Sin;
	Matrix.M33 = Cos;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateRotationY(ZEMatrix4x4& Matrix, float Yawn)
{
	float Cos = ZEAngle::Cos(Yawn);
	float Sin = ZEAngle::Sin(Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0f;
	Matrix.M13 = Sin;
	Matrix.M14 = 0.0f;

	Matrix.M21 = 0.0f;
	Matrix.M22 = 1.0f;
	Matrix.M23 = 0.0f;
	Matrix.M24 = 0.0f;

	Matrix.M31 = -Sin;
	Matrix.M32 = 0.0f;
	Matrix.M33 = Cos;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateRotationZ(ZEMatrix4x4& Matrix, float Roll)
{
	float Cos = ZEAngle::Cos(Roll);
	float Sin = ZEAngle::Sin(Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = -Sin;
	Matrix.M13 = 0.0f;
	Matrix.M14 = 0.0f;

	Matrix.M21 = Sin;
	Matrix.M22 = Cos;
	Matrix.M23 = 0.0f;
	Matrix.M24 = 0.0f;

	Matrix.M31 = 0.0f;
	Matrix.M32 = 0.0f;
	Matrix.M33 = 1.0f;
	Matrix.M34 = 0.0f;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateScale(ZEMatrix4x4& Matrix, float x, float y, float z)
{
	Create(Matrix,
			x, 0.0f, 0.0f, 0.0f,
			0.0f, y, 0.0f, 0.0f,
			0.0f, 0.0f, z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f );
}

void ZEMatrix4x4::CreateScale(ZEMatrix4x4& Matrix, const ZEVector3& Scale)
{
	Create(Matrix,
			Scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, Scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, Scale.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4& Matrix, float x, float y, float z)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Scale)
{
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = (1.0f - 2.0f * yy - 2.0f * zz) * Scale.x;
	Matrix.M12 = (		 2.0f * xy - 2.0f * zw) * Scale.y;
	Matrix.M13 = (		 2.0f * xz + 2.0f * yw) * Scale.z;
	Matrix.M14 = Position.x;				 
											 
	Matrix.M21 = (		 2.0f * xy + 2.0f * zw) * Scale.x;
	Matrix.M22 = (1.0f - 2.0f * xx - 2.0f * zz) * Scale.y;
	Matrix.M23 = (		 2.0f * yz - 2.0f * xw) * Scale.z;
	Matrix.M24 = Position.y;				 
											 
	Matrix.M31 = (		 2.0f * xz - 2.0f * yw) * Scale.x;
	Matrix.M32 = (		 2.0f * yz + 2.0f * xw) * Scale.y;
	Matrix.M33 = (1.0f - 2.0f * xx - 2.0f * yy) * Scale.z;
	Matrix.M34 = Position.z;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation)
{
	float xx = Rotation.x * Rotation.x;
	float xy = Rotation.x * Rotation.y;
	float xz = Rotation.x * Rotation.z;
	float xw = Rotation.x * Rotation.w;
	float yy = Rotation.y * Rotation.y;
	float yz = Rotation.y * Rotation.z;
	float yw = Rotation.y * Rotation.w;
	float zz = Rotation.z * Rotation.z;
	float zw = Rotation.z * Rotation.w;
	float ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;
	Matrix.M14 = Position.x;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;
	Matrix.M24 = Position.y;

	Matrix.M31 = 		2.0f * xz - 2.0f * yw;
	Matrix.M32 = 		2.0f * yz + 2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx - 2.0f * yy;
	Matrix.M34 = Position.z;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEVector3& Scale)
{
	Create(Matrix,
		Scale.x, 0.0f, 0.0f, Position.x,
		0.0f, Scale.y, 0.0f, Position.y,
		0.0f, 0.0f, Scale.z, Position.z,
		0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateTranslation(ZEMatrix4x4& Matrix, const ZEVector3& Position)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, Position.x,
			0.0f, 1.0f, 0.0f, Position.y,
			0.0f, 0.0f, 1.0f, Position.z,
			0.0f, 0.0f, 0.0f, 1.0f );
}

void ZEMatrix4x4::CreateIdentity(ZEMatrix4x4& Matrix)
{
	Create(Matrix,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f );
}

void ZEMatrix4x4::CreateZero(ZEMatrix4x4& Matrix)
{
	Create(Matrix,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f );
}

void ZEMatrix4x4::CreateViewTransform(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation)
{
	ZEQuaternion RotationInv;
	ZEQuaternion::Conjugate(RotationInv, Rotation);

	float xx = RotationInv.x * RotationInv.x;
	float xy = RotationInv.x * RotationInv.y;
	float xz = RotationInv.x * RotationInv.z;
	float xw = RotationInv.x * RotationInv.w;
	float yy = RotationInv.y * RotationInv.y;
	float yz = RotationInv.y * RotationInv.z;
	float yw = RotationInv.y * RotationInv.w;
	float zz = RotationInv.z * RotationInv.z;
	float zw = RotationInv.z * RotationInv.w;
	float ww = RotationInv.w * RotationInv.w;

	Matrix.M11 = 1.0f - 2.0f * yy - 2.0f * zz;
	Matrix.M12 = 		2.0f * xy - 2.0f * zw;
	Matrix.M13 = 		2.0f * xz + 2.0f * yw;
	Matrix.M14 = -Position.x * Matrix.M11 + -Position.y * Matrix.M12 + -Position.z * Matrix.M13;

	Matrix.M21 = 		2.0f * xy + 2.0f * zw;
	Matrix.M22 = 1.0f - 2.0f * xx - 2.0f * zz;
	Matrix.M23 = 		2.0f * yz - 2.0f * xw;
	Matrix.M24 = -Position.x * Matrix.M21 + -Position.y * Matrix.M22 + -Position.z * Matrix.M23;

	Matrix.M31 = 		2.0f * xz - 2.0f * yw;
	Matrix.M32 = 		2.0f * yz + 2.0f * xw;
	Matrix.M33 = 1.0f - 2.0f * xx - 2.0f * yy;
	Matrix.M34 = -Position.x * Matrix.M31 + -Position.y * Matrix.M32 + -Position.z * Matrix.M33;

	Matrix.M41 = 0.0f;
	Matrix.M42 = 0.0f;
	Matrix.M43 = 0.0f;
	Matrix.M44 = 1.0f;
}

void ZEMatrix4x4::CreateLookAtTransform(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEVector3& Target, const ZEVector3& Up)
{
	ZEVector3 XAxis;
	ZEVector3 YAxis;
	ZEVector3 ZAxis;

	ZEVector3::Normalize(ZAxis, Target - Position);
	ZEVector3::CrossProduct(XAxis, Up, ZAxis);
	ZEVector3::Normalize(XAxis, XAxis);
	ZEVector3::CrossProduct(YAxis, ZAxis, XAxis);

	ZEMatrix4x4::Create(Matrix, 
		XAxis.x, XAxis.y, XAxis.z, -ZEVector3::DotProduct(XAxis, Position),
		YAxis.x, YAxis.y, YAxis.z, -ZEVector3::DotProduct(YAxis, Position),
		ZAxis.x, ZAxis.y, ZAxis.z, -ZEVector3::DotProduct(ZAxis, Position),
		0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateOrthographicProjection(ZEMatrix4x4& Matrix, float Width, float Height, float NearZ, float FarZ)
{
	Create(Matrix,
			2.0f/Width, 0.0f, 0.0f, 0.0f, 
			0.0f, 2.0f/Height, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f/(FarZ - NearZ), NearZ/(NearZ - FarZ), 
			0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreatePerspectiveProjection(ZEMatrix4x4& Matrix, float FOV, float AspectRatio, float NearZ, float FarZ)
{
	float Temp = FarZ;
	FarZ = NearZ;
	NearZ = Temp;

	float Ys = 1.0f / ZEAngle::Tan(FOV * 0.5f);
	float Xs = Ys / AspectRatio;
	float Range = FarZ - NearZ;

	Create(Matrix, 
		Xs, 0.0f, 0.0f, 0.0f,
		0.0f, Ys, 0.0f, 0.0f,
		0.0f, 0.0f, FarZ / Range, -FarZ * NearZ / Range,
		0.0f, 0.0f, 1.0f, 0.0f);
}

void ZEMatrix4x4::CreatePerspectiveProjectionOffCenter(ZEMatrix4x4& Matrix, float Left, float Right, float Bottom, float Top, float NearZ, float FarZ)
{
	float OriginalNearZ = NearZ;
	float Temp = FarZ;
	FarZ = NearZ;
	NearZ = Temp;

	float Width = Right - Left;
	float Height = Top - Bottom;
	float Range = FarZ - NearZ;
	float OffsetX = (Left + Right) / -Width;
	float OffsetY = (Bottom + Top) / -Height;
	float OffsetZ = FarZ * NearZ / -Range;

	Create(Matrix,
		2.0f * OriginalNearZ / Width, 0.0f, OffsetX, 0.0f,
		0.0f, 2.0f * OriginalNearZ / Height, OffsetY, 0.0f,
		0.0f, 0.0f, FarZ / Range, OffsetZ,
		0.0f, 0.0f, 1.0f, 0.0f);
}

void ZEMatrix4x4::CreateViewPortTransform(ZEMatrix4x4& Matrix, float Left, float Right, float Top, float Bottom, float NearZ, float FarZ)
{
	Create(Matrix,
			2.0f / (Right - Left), 0.0f, 0.0f, (Left + Right) / (Left - Right), 
			0.0f, 2.0f / (Top - Bottom), 0.0f, (Top + Bottom) / (Bottom - Top), 
			0.0f, 0.0f, 1.0f / (FarZ - NearZ), NearZ / (NearZ - FarZ), 
			0.0f, 0.0f, 0.0f, 1.0f);
}

void ZEMatrix4x4::CreateScreenTransform(ZEMatrix4x4& Matrix, float ScreenWidth, float ScreenHeight)
{
	float Xs = ScreenWidth / 2.0f;
	float Ys = ScreenHeight / -2.0f;

	Create(Matrix,
		Xs, 0.0f, 0.0f, Xs,
		0.0f, Ys, 0.0f, -Ys,
		0.0f, 0.0f,	1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

float ZEMatrix4x4::Determinant(const ZEMatrix4x4 &Matrix)
{
	ZEMatrix3x3 A, B, C, D;
	ZEMatrix3x3::Create(A, 
		Matrix.M22, Matrix.M23, Matrix.M24, 
		Matrix.M32, Matrix.M33, Matrix.M34, 
		Matrix.M42, Matrix.M43, Matrix.M44);

	ZEMatrix3x3::Create(B, 
		Matrix.M21, Matrix.M23, Matrix.M24, 
		Matrix.M31, Matrix.M33, Matrix.M34, 
		Matrix.M41, Matrix.M43, Matrix.M44);

    ZEMatrix3x3::Create(C, 
		Matrix.M21, Matrix.M22, Matrix.M24, 
		Matrix.M31, Matrix.M32, Matrix.M34, 
		Matrix.M41, Matrix.M42, Matrix.M44);

	ZEMatrix3x3::Create(D, 
		Matrix.M21, Matrix.M22, Matrix.M23, 
		Matrix.M31, Matrix.M32, Matrix.M33, 
		Matrix.M41, Matrix.M42, Matrix.M43);

	return Matrix.M11 * A.Determinant() + Matrix.M13 * C.Determinant() - Matrix.M12 * B.Determinant() - Matrix.M14 * D.Determinant();
}

float ZEMatrix4x4::Determinant() const
{
	return ZEMatrix4x4::Determinant(*this);
}

ZEMatrix4x4 ZEMatrix4x4::Inverse() const
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Inverse(Temp, *this);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::Transpose() const
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Transpose(Temp, *this);
	return Temp;
}

void ZEMatrix4x4::InverseSelf()
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Inverse(Temp, *this);
	*this = Temp;
}

void ZEMatrix4x4::TransposeSelf()
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Transpose(Temp, *this);
	*this = Temp;
}

bool ZEMatrix4x4::Equals(const ZEMatrix4x4& Matrix) const
{
	for (ZESize I = 0; I < 16; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > ZE_ZERO_THRESHOLD)
			return false;

	return true;
}

bool ZEMatrix4x4::Equals(const ZEMatrix4x4& Matrix, float Threshold) const
{
	for (ZESize I = 0; I < 16; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > Threshold)
			return false;

	return true;
}

ZEVector3 ZEMatrix4x4::GetTranslation()
{
	return ZEVector3(M14, M24, M34);
}

ZEQuaternion ZEMatrix4x4::GetRotation()
{
	ZEVector3 TempScale;
	TempScale.x = ZEVector3(M11, M21, M31).Length();
	TempScale.y = ZEVector3(M12, M22, M32).Length();
	TempScale.z = ZEVector3(M13, M23, M33).Length();

	ZEMatrix4x4 ScaleMatrix;
	ZEMatrix4x4::CreateScale(ScaleMatrix, TempScale);

	ZEMatrix4x4 RotationMatrix = (*this) * ScaleMatrix.Inverse();

	ZEQuaternion Rotation;
	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix);
	return Rotation;
}

ZEVector3 ZEMatrix4x4::GetScale()
{
	ZEVector3 Scale;

	Scale.x = ZEVector3(M11, M21, M31).Length();
	Scale.y = ZEVector3(M12, M22, M32).Length();
	Scale.z = ZEVector3(M13, M23, M33).Length();

	return Scale;
}

void ZEMatrix4x4::GetDecomposition(ZEVector3& Translation, ZEQuaternion& Rotation, ZEVector3& Scale, const ZEMatrix4x4& Matrix)
{
	Translation.x = Matrix.M14;
	Translation.y = Matrix.M24;
	Translation.z = Matrix.M34;

	Scale.x = ZEVector3(Matrix.M11, Matrix.M21, Matrix.M31).Length();
	Scale.y = ZEVector3(Matrix.M12, Matrix.M22, Matrix.M32).Length();
	Scale.z = ZEVector3(Matrix.M13, Matrix.M23, Matrix.M33).Length();

	ZEMatrix4x4 TempScaleMatrix;
	ZEMatrix4x4::CreateScale(TempScaleMatrix, Scale);

	ZEMatrix4x4 RotationMatrix = Matrix * TempScaleMatrix.Inverse();

	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix);
}

ZEMatrix4x4 ZEMatrix4x4::operator+(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Add(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator-(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator*(const ZEMatrix4x4 &RightOperand) const 
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3 ZEMatrix4x4::operator*(const ZEVector3 &RightOperand) const
{
	ZEVector3 Temp;
	ZEMatrix4x4::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4 ZEMatrix4x4::operator*(const ZEVector4 &RightOperand) const
{
	ZEVector4 Temp;
	ZEMatrix4x4::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4 ZEMatrix4x4::operator*(float S) 
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Scale(Temp, *this, S);
	return Temp;
}

ZEMatrix4x4& ZEMatrix4x4::operator += (const ZEMatrix4x4 &RightOperand)
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Add(Temp, *this, RightOperand);
	*this = Temp;
	return *this;
}

ZEMatrix4x4& ZEMatrix4x4::operator -= (const ZEMatrix4x4 &RightOperand)
{
	Sub(*this, *this, RightOperand);
	return *this;
}

ZEMatrix4x4& ZEMatrix4x4::operator *= (const ZEMatrix4x4 &RightOperand)
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Multiply(Temp, *this, RightOperand);
	return *this = Temp;
}

ZEMatrix4x4& ZEMatrix4x4::operator *= (float S)
{
	ZEMatrix4x4::Scale(*this, *this, S);
	return *this;
}

bool ZEMatrix4x4::operator == (const ZEMatrix4x4 &M) const 
{
	return Equals(M);
}

bool ZEMatrix4x4::operator != (const ZEMatrix4x4 &M) const 
{
	return !Equals(M);
}

float ZEMatrix4x4::operator[](ZESize Index) const
{
	return MA[Index];
}

float& ZEMatrix4x4::operator[](ZESize Index)
{
	return MA[Index];
}

ZEMatrix4x4::ZEMatrix4x4(float M11, float M12, float M13, float M14,
						 float M21, float M22, float M23, float M24,
						 float M31, float M32, float M33, float M34,
						 float M41, float M42, float M43, float M44)
{
	ZEMatrix4x4::Create(*this, M11, M12, M13, M14, M21, M22, M23, M24, M31, M32, M33, M34, M41, M42, M43, M44);  
}

ZEMatrix4x4::ZEMatrix4x4(float M[16])
{
	ZEMatrix4x4::Create(*this, M);
}

ZEMatrix4x4::ZEMatrix4x4()
{

}

ZEMatrix4x4 operator*(float S, const ZEMatrix4x4& RightOperand)
{
	ZEMatrix4x4 Temp;
	ZEMatrix4x4::Scale(Temp, RightOperand, S);
	return Temp;
}
