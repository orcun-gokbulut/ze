//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrixd.cpp
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

#include "ZEMatrixd.h"
#include "ZEVectord.h"
#include "ZEQuaternion.h"
#include "ZEMath.h"
#include <memory.h>
#include "ZEMath/ZEAngle.h"
#include "ZEError.h"

#define ZE_MATRIX4X4_ROW_ELEMENT_COUNT		(ZESize)4
#define ZE_MATRIX4X4_COL_ELEMENT_COUNT		(ZESize)4

#define SWAP(x, y) temp = (x); (x) = (y); (y) = temp

// ZEMatrix3x3d
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEMatrix3x3d ZEMatrix3x3d::Zero = ZEMatrix3x3d(0.0, 0.0, 0.0,
												     0.0, 0.0, 0.0,
												     0.0, 0.0, 0.0);

const ZEMatrix3x3d ZEMatrix3x3d::Identity = ZEMatrix3x3d(1.0, 0.0, 0.0,
													     0.0, 1.0, 0.0,
													     0.0, 0.0, 1.0);

void ZEMatrix3x3d::Create(ZEMatrix3x3d &Matrix, double M11, double M12, double M13,
													   double M21, double M22, double M23,
													   double M31, double M32, double M33)
{
	Matrix.M11 = M11; Matrix.M12 = M12; Matrix.M13 = M13;
	Matrix.M21 = M21; Matrix.M22 = M22; Matrix.M23 = M23;
	Matrix.M31 = M31; Matrix.M32 = M32; Matrix.M33 = M33;
}

void ZEMatrix3x3d::Create(ZEMatrix3x3d& Matrix, double M[9])
{
	memcpy(Matrix.M, M, sizeof(ZEMatrix3x3d));
}

void ZEMatrix3x3d::CreateRotation(ZEMatrix3x3d& Matrix, const ZEQuaternion& Rotation)
{
	double xx = Rotation.x * Rotation.x;
	double xy = Rotation.x * Rotation.y;
	double xz = Rotation.x * Rotation.z;
	double xw = Rotation.x * Rotation.w;
	double yy = Rotation.y * Rotation.y;
	double yz = Rotation.y * Rotation.z;
	double yw = Rotation.y * Rotation.w;
	double zz = Rotation.z * Rotation.z;
	double zw = Rotation.z * Rotation.w;
	double ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0 - 2.0 * yy - 2.0 * zz;
	Matrix.M12 = 	   2.0 * xy - 2.0 * zw;
	Matrix.M13 = 	   2.0 * xz + 2.0 * yw;

	Matrix.M21 =	   2.0 * xy + 2.0 * zw;
	Matrix.M22 = 1.0 - 2.0 * xx - 2.0 * zz;
	Matrix.M23 = 	   2.0 * yz - 2.0 * xw;

	Matrix.M31 = 	   2.0 * xz - 2.0 * yw;
	Matrix.M32 = 	   2.0 * yz + 2.0 * xw;
	Matrix.M33 = 1.0 - 2.0 * xx - 2.0 * yy;
}

void ZEMatrix3x3d::CreateRotation(ZEMatrix3x3d& Matrix, double Pitch, double Yawn, double Roll, ZERotationOrder RotationOrder)
{
	ZEMatrix3x3d XRotation, YRotation, ZRotation, Temp;

	ZEMatrix3x3d::CreateRotationX(XRotation, Pitch);
	ZEMatrix3x3d::CreateRotationY(YRotation, Yawn);
	ZEMatrix3x3d::CreateRotationZ(ZRotation, Roll);

	switch(RotationOrder)
	{
		case ZE_RO_XYZ:
			ZEMatrix3x3d::Multiply(Temp, XRotation, YRotation);
			ZEMatrix3x3d::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_XZY:
			ZEMatrix3x3d::Multiply(Temp, XRotation, ZRotation);
			ZEMatrix3x3d::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_YXZ:
			ZEMatrix3x3d::Multiply(Temp, YRotation, XRotation);
			ZEMatrix3x3d::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_YZX:
			ZEMatrix3x3d::Multiply(Temp, YRotation, ZRotation);
			ZEMatrix3x3d::Multiply(Matrix, Temp, XRotation);
			break;

		case ZE_RO_ZXY:
			ZEMatrix3x3d::Multiply(Temp, ZRotation, XRotation);
			ZEMatrix3x3d::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_ZYX:
			ZEMatrix3x3d::Multiply(Temp, ZRotation, YRotation);
			ZEMatrix3x3d::Multiply(Matrix, Temp, XRotation);
			break;
	}
}

void ZEMatrix3x3d::CreateRotationX(ZEMatrix3x3d& Matrix, double Pitch)
{
	double Cos = ZEAngle::Cos((float)Pitch);
	double Sin = ZEAngle::Sin((float)Pitch);

	Matrix.M11 = 1.0;
	Matrix.M12 = 0.0;
	Matrix.M13 = 0.0;

	Matrix.M21 = 0.0;
	Matrix.M22 = Cos;
	Matrix.M23 = -Sin;

	Matrix.M31 = 0.0;
	Matrix.M32 = Sin;
	Matrix.M33 = Cos;
}

void ZEMatrix3x3d::CreateRotationY(ZEMatrix3x3d& Matrix, double Yawn)
{
	double Cos = ZEAngle::Cos((float)Yawn);
	double Sin = ZEAngle::Sin((float)Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0;
	Matrix.M13 = Sin;

	Matrix.M21 = 0.0;
	Matrix.M22 = 1.0;
	Matrix.M23 = 0.0;

	Matrix.M31 = -Sin;
	Matrix.M32 = 0.0;
	Matrix.M33 = Cos;
}

void ZEMatrix3x3d::CreateRotationZ(ZEMatrix3x3d& Matrix, double Roll)
{
	double Cos = ZEAngle::Cos((float)Roll);
	double Sin = ZEAngle::Sin((float)Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = -Sin;
	Matrix.M13 = 0.0;

	Matrix.M21 = Sin;
	Matrix.M22 = Cos;
	Matrix.M23 = 0.0;

	Matrix.M31 = 0.0;
	Matrix.M32 = 0.0;
	Matrix.M33 = 1.0;
}

void ZEMatrix3x3d::CreateIdentity(ZEMatrix3x3d& Matrix)
{
	Create(Matrix,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0);
}

void ZEMatrix3x3d::CreateZero(ZEMatrix3x3d& Matrix)
{
	Create(Matrix,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0);
}

void ZEMatrix3x3d::CreateScale(ZEMatrix3x3d& Matrix, double x, double y, double z)
{
	Create(Matrix,
		x, 0.0, 0.0,
		0.0, y, 0.0,
		0.0, 0.0, z);
}

void ZEMatrix3x3d::CreateScale(ZEMatrix3x3d& Matrix, const ZEVector3d& Scale)
{
	Create(Matrix,
		Scale.x, 0.0, 0.0,
		0.0, Scale.y, 0.0,
		0.0, 0.0, Scale.z);
}


void ZEMatrix3x3d::CreateTranslation2D(ZEMatrix3x3d& Matrix, double x, double y)
{
	Create(Matrix,
		1.0, 0.0, x,
		0.0, 1.0, y,
		0.0, 0.0, 1.0);

}

void ZEMatrix3x3d::CreateTranslation2D(ZEMatrix3x3d& Matrix, const ZEVector2d& Position)
{
	Create(Matrix,
		1.0, 0.0, Position.x,
		0.0, 1.0, Position.y,
		0.0, 0.0, 1.0);

}

void ZEMatrix3x3d::CreateRotation2D(ZEMatrix3x3d& Matrix, double Angle)
{
	double CosAngle = ZEAngle::Cos((float)Angle);
	double SinAngle = ZEAngle::Sin((float)Angle);

	Matrix.M11 = CosAngle;
	Matrix.M12 = -SinAngle;
	Matrix.M13 = 0.0;

	Matrix.M21 = SinAngle;
	Matrix.M22 = CosAngle;
	Matrix.M23 = 0.0;

	Matrix.M31 = 0.0;
	Matrix.M32 = 0.0;
	Matrix.M33 = 1.0;
}

void ZEMatrix3x3d::CreateScale2D(ZEMatrix3x3d& Matrix, double x, double y)
{
	Create(Matrix,
		x, 0.0, 0.0,
		0.0, y, 0.0,
		0.0, 0.0, 1.0);
}

void ZEMatrix3x3d::CreateScale2D(ZEMatrix3x3d& Matrix, const ZEVector2d& Scale)
{
	Create(Matrix,
		Scale.x, 0.0, 0.0,
		0.0, Scale.y, 0.0,
		0.0, 0.0, 1.0);
}

void ZEMatrix3x3d::CreateOrientation2D(ZEMatrix3x3d& Matrix, const ZEVector2d& Translation, double Angle, const ZEVector2d& Scale)
{
	double Cos = ZEAngle::Cos((float)Angle);
	double Sin = ZEAngle::Sin((float)Angle);

	Matrix.M11 = Cos * Scale.x;
	Matrix.M12 = -Sin * Scale.y;
	Matrix.M13 = Translation.x;

	Matrix.M21 = Sin * Scale.x;
	Matrix.M22 = Cos * Scale.y;
	Matrix.M23 = Translation.y;

	Matrix.M31 = 0.0;
	Matrix.M32 = 0.0;
	Matrix.M33 = 1.0;
}

double ZEMatrix3x3d::Determinant() const
{
	return ZEMatrix3x3d::Determinant(*this);
}

ZEMatrix3x3d ZEMatrix3x3d::Inverse() const
{
	ZEMatrix3x3d Temp;
	ZEMatrix3x3d::Inverse(Temp, *this);
	return Temp;
}

ZEMatrix3x3d ZEMatrix3x3d::Transpose() const
{
	ZEMatrix3x3d Temp;
	ZEMatrix3x3d::Transpose(Temp, *this);
	return Temp;
}

void ZEMatrix3x3d::InverseSelf()
{
	ZEMatrix3x3d Temp;
	ZEMatrix3x3d::Inverse(Temp, *this);
	*this = Temp;
}

void ZEMatrix3x3d::TransposeSelf()
{
	ZEMatrix3x3d Temp;
	ZEMatrix3x3d::Transpose(Temp, *this);
	*this = Temp;
}

bool ZEMatrix3x3d::Equals(const ZEMatrix3x3d& Matrix) const
{
	for (ZESize I = 0; I < 9; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > ZE_ZERO_THRESHOLD)
			return false;

	return true;
}

bool ZEMatrix3x3d::Equals(const ZEMatrix3x3d& Matrix, double Threshold)
{
	for (ZESize I = 0; I < 9; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > Threshold)
			return false;

	return true;
}

ZEQuaternion ZEMatrix3x3d::GetRotation()
{
	ZEVector3d TempScale;
	TempScale.x = ZEVector3d(M11, M21, M31).Length();
	TempScale.y = ZEVector3d(M12, M22, M32).Length();
	TempScale.z = ZEVector3d(M13, M23, M33).Length();

	ZEMatrix3x3d ScaleMatrix;
	ZEMatrix3x3d::CreateScale(ScaleMatrix, TempScale.x, TempScale.y, TempScale.z);

	ZEMatrix3x3d RotationMatrix = (*this) * ScaleMatrix.Inverse();

	ZEQuaternion Rotation;
	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix.ToMatrix3x3());
	return Rotation;
}

ZEVector3d ZEMatrix3x3d::GetScale()
{
	ZEVector3d Scale;

	Scale.x = ZEVector3d(M11, M21, M31).Length();
	Scale.y = ZEVector3d(M12, M22, M32).Length();
	Scale.z = ZEVector3d(M13, M23, M33).Length();

	return Scale;
}

void ZEMatrix3x3d::GetDecomposition(ZEQuaternion& Rotation, ZEVector3d& Scale, const ZEMatrix3x3d& Matrix)
{
	Scale.x = ZEVector3d(Matrix.M11, Matrix.M21, Matrix.M31).Length();
	Scale.y = ZEVector3d(Matrix.M12, Matrix.M22, Matrix.M32).Length();
	Scale.z = ZEVector3d(Matrix.M13, Matrix.M23, Matrix.M33).Length();

	ZEMatrix3x3d ScaleMatrix;
	ZEMatrix3x3d::CreateScale(ScaleMatrix, Scale.x, Scale.y, Scale.z);

	ZEMatrix3x3d RotationMatrix = Matrix * ScaleMatrix.Inverse();

	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix.ToMatrix3x3());
}

ZEVector2d ZEMatrix3x3d::Get2DTranslation()
{
	return ZEVector2d(M13, M23);
}

double ZEMatrix3x3d::Get2DRotation()
{
	return ZEAngle::ArcTan2((float)M12, (float)M22);
}

ZEVector2d ZEMatrix3x3d::Get2DScale()
{
	ZEVector2d Scale;

	Scale.x = ZEVector2d(M11, M21).Length();
	Scale.y = ZEVector2d(M12, M22).Length();

	return Scale;
}

void ZEMatrix3x3d::Get2DDecomposition(ZEVector2d& Translation, double& Rotation, ZEVector2d& Scale, const ZEMatrix3x3d& Matrix)
{
	Translation.x = Matrix.M13;
	Translation.y = Matrix.M23;

	Scale.x = ZEVector2d(Matrix.M11, Matrix.M21).Length();
	Scale.y = ZEVector2d(Matrix.M12, Matrix.M22).Length();

	Rotation = ZEAngle::ArcTan2((float)Matrix.M12, (float)Matrix.M22);
}

void ZEMatrix3x3d::FromMatrix3x3(const ZEMatrix3x3& Matrix)
{
	for (ZESize I = 0; I < 3; I++)
		for (ZESize J = 0; J < 3; J++)
			this->M[I][J] = (double)Matrix.M[I][J];
}

ZEMatrix3x3 ZEMatrix3x3d::ToMatrix3x3() const
{
	return ZEMatrix3x3((float)M11, (float)M12, (float)M13, 
					   (float)M21, (float)M22, (float)M23, 
					   (float)M31, (float)M32, (float)M33);
}

ZEMatrix3x3d ZEMatrix3x3d::operator+(const ZEMatrix3x3d& RightOperand) const 
{
	ZEMatrix3x3d Temp;
	Add(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3d ZEMatrix3x3d::operator-(const ZEMatrix3x3d& RightOperand) const 
{
	ZEMatrix3x3d Temp;
	Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3d ZEMatrix3x3d::operator*(const ZEMatrix3x3d& RightOperand) const 
{
	ZEMatrix3x3d Temp;
	Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3d ZEMatrix3x3d::operator*(double S) 
{
	ZEMatrix3x3d Temp;
	Scale(Temp, *this, S);
	return Temp;
}

ZEVector2d ZEMatrix3x3d::operator*(const ZEVector2d& RightOperand) const
{
	ZEVector2d Temp;
	ZEMatrix3x3d::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3d ZEMatrix3x3d::operator*(const ZEVector3d& RightOperand) const
{
	ZEVector3d Temp;
	ZEMatrix3x3d::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4d ZEMatrix3x3d::operator*(const ZEVector4d& RightOperand) const
{
	ZEVector4d Temp;
	ZEMatrix3x3d::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix3x3d& ZEMatrix3x3d::operator += (const ZEMatrix3x3d& RightOperand)
{
	Add(*this, *this, RightOperand);
	return *this;
}

ZEMatrix3x3d& ZEMatrix3x3d::operator -= (const ZEMatrix3x3d& RightOperand)
{
	Sub(*this, *this, RightOperand);
	return *this;
}

ZEMatrix3x3d& ZEMatrix3x3d::operator *= (const ZEMatrix3x3d& RightOperand)
{
	ZEMatrix3x3d Temp;
	Multiply(Temp, *this, RightOperand);
	return *this = Temp;
}

ZEMatrix3x3d& ZEMatrix3x3d::operator *= (double S)
{
	Scale(*this, *this, S);
	return *this;
}

bool ZEMatrix3x3d::operator == (const ZEMatrix3x3d& M) const 
{
	return Equals(M);
}

bool ZEMatrix3x3d::operator != (const ZEMatrix3x3d& M) const 
{
	return !Equals(M);
}

double ZEMatrix3x3d::operator[](ZESize Index) const
{
	return MA[Index];
}

double& ZEMatrix3x3d::operator[](ZESize Index)
{
	return MA[Index];
}

ZEMatrix3x3d::ZEMatrix3x3d(double M00, double M01, double M02,
						   double M10, double M11, double M12,
						   double M20, double M21, double M22)
{
	Create(*this, M00, M01, M02, M10, M11, M12, M20, M21, M22);  
}

ZEMatrix3x3d::ZEMatrix3x3d(double M[9])
{
	Create(*this, M);
}

ZEMatrix3x3d::ZEMatrix3x3d()
{
}

ZEMatrix3x3d operator*(double S, const ZEMatrix3x3d& RightOperand)
{
	ZEMatrix3x3d Temp;
	ZEMatrix3x3d::Scale(Temp, RightOperand, S);
	return Temp;
}

// ZEMatrix4x4d
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ZEMatrix4x4d ZEMatrix4x4d::Zero = ZEMatrix4x4d(0.0, 0.0, 0.0, 0.0,
													 0.0, 0.0, 0.0, 0.0,
													 0.0, 0.0, 0.0, 0.0,
													 0.0, 0.0, 0.0, 0.0);

const ZEMatrix4x4d ZEMatrix4x4d::Identity = ZEMatrix4x4d(1.0, 0.0, 0.0, 0.0,
														 0.0, 1.0, 0.0, 0.0,
														 0.0, 0.0, 1.0, 0.0,
														 0.0, 0.0, 0.0, 1.0);

void ZEMatrix4x4d::Create(ZEMatrix4x4d& Matrix,
								double M11, double M12, double M13, double M14,
								double M21, double M22, double M23, double M24,
								double M31, double M32, double M33, double M34,
								double M41, double M42, double M43, double M44)
{
	Matrix.M11 = M11; Matrix.M21 = M21; Matrix.M31 = M31; Matrix.M41 = M41;
	Matrix.M12 = M12; Matrix.M22 = M22; Matrix.M32 = M32; Matrix.M42 = M42;
	Matrix.M13 = M13; Matrix.M23 = M23; Matrix.M33 = M33; Matrix.M43 = M43;
	Matrix.M14 = M14; Matrix.M24 = M24; Matrix.M34 = M34; Matrix.M44 = M44;
}

void ZEMatrix4x4d::Create(ZEMatrix4x4d& Matrix, double M[16])
{
	memcpy(Matrix.M, M, sizeof(ZEMatrix4x4d));
}

void ZEMatrix4x4d::CreateRotation(ZEMatrix4x4d& Matrix, const ZEQuaternion& Rotation)
{
	double xx = Rotation.x * Rotation.x;
	double xy = Rotation.x * Rotation.y;
	double xz = Rotation.x * Rotation.z;
	double xw = Rotation.x * Rotation.w;
	double yy = Rotation.y * Rotation.y;
	double yz = Rotation.y * Rotation.z;
	double yw = Rotation.y * Rotation.w;
	double zz = Rotation.z * Rotation.z;
	double zw = Rotation.z * Rotation.w;
	double ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0 - 2.0 * yy - 2.0 * zz;
	Matrix.M12 = 	   2.0 * xy - 2.0 * zw;
	Matrix.M13 = 	   2.0 * xz + 2.0 * yw;
	Matrix.M14 = 0.0;

	Matrix.M21 = 	   2.0 * xy + 2.0 * zw;
	Matrix.M22 = 1.0 - 2.0 * xx - 2.0 * zz;
	Matrix.M23 = 	   2.0 * yz - 2.0 * xw;
	Matrix.M24 = 0.0;

	Matrix.M31 = 	   2.0 * xz - 2.0 * yw;
	Matrix.M32 = 	   2.0 * yz + 2.0 * xw;
	Matrix.M33 = 1.0 - 2.0 * xx - 2.0 * yy;
	Matrix.M34 = 0.0;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateRotation(ZEMatrix4x4d& Matrix, double Pitch, double Yawn, double Roll, ZERotationOrder RotationOrder)
{
	ZEMatrix4x4d XRotation, YRotation, ZRotation, Temp;

	ZEMatrix4x4d::CreateRotationX(XRotation, Pitch);
	ZEMatrix4x4d::CreateRotationY(YRotation, Yawn);
	ZEMatrix4x4d::CreateRotationZ(ZRotation, Roll);

	switch(RotationOrder)
	{
		case ZE_RO_XYZ:
			ZEMatrix4x4d::Multiply(Temp, XRotation, YRotation);
			ZEMatrix4x4d::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_XZY:
			ZEMatrix4x4d::Multiply(Temp, XRotation, ZRotation);
			ZEMatrix4x4d::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_YXZ:
			ZEMatrix4x4d::Multiply(Temp, YRotation, XRotation);
			ZEMatrix4x4d::Multiply(Matrix, Temp, ZRotation);
			break;

		case ZE_RO_YZX:
			ZEMatrix4x4d::Multiply(Temp, YRotation, ZRotation);
			ZEMatrix4x4d::Multiply(Matrix, Temp, XRotation);
			break;

		case ZE_RO_ZXY:
			ZEMatrix4x4d::Multiply(Temp, ZRotation, XRotation);
			ZEMatrix4x4d::Multiply(Matrix, Temp, YRotation);
			break;

		case ZE_RO_ZYX:
			ZEMatrix4x4d::Multiply(Temp, ZRotation, YRotation);
			ZEMatrix4x4d::Multiply(Matrix, Temp, XRotation);
			break;
	}
}

void ZEMatrix4x4d::CreateRotationX(ZEMatrix4x4d& Matrix, double Pitch)
{
	double Cos = ZEAngle::Cos((float)Pitch);
	double Sin = ZEAngle::Sin((float)Pitch);

	Matrix.M11 = 1.0;
	Matrix.M12 = 0.0;
	Matrix.M13 = 0.0;
	Matrix.M14 = 0.0;

	Matrix.M21 = 0.0;
	Matrix.M22 = Cos;
	Matrix.M23 = -Sin;
	Matrix.M24 = 0.0;

	Matrix.M31 = 0.0;
	Matrix.M32 = Sin;
	Matrix.M33 = Cos;
	Matrix.M34 = 0.0;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateRotationY(ZEMatrix4x4d& Matrix, double Yawn)
{
	double Cos = ZEAngle::Cos((float)Yawn);
	double Sin = ZEAngle::Sin((float)Yawn);

	Matrix.M11 = Cos;
	Matrix.M12 = 0.0;
	Matrix.M13 = Sin;
	Matrix.M14 = 0.0;

	Matrix.M21 = 0.0;
	Matrix.M22 = 1.0;
	Matrix.M23 = 0.0;
	Matrix.M24 = 0.0;

	Matrix.M31 = -Sin;
	Matrix.M32 = 0.0;
	Matrix.M33 = Cos;
	Matrix.M34 = 0.0;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateRotationZ(ZEMatrix4x4d& Matrix, double Roll)
{
	double Cos = ZEAngle::Cos((float)Roll);
	double Sin = ZEAngle::Sin((float)Roll);

	Matrix.M11 = Cos;
	Matrix.M12 = -Sin;
	Matrix.M13 = 0.0;
	Matrix.M14 = 0.0;

	Matrix.M21 = Sin;
	Matrix.M22 = Cos;
	Matrix.M23 = 0.0;
	Matrix.M24 = 0.0;

	Matrix.M31 = 0.0;
	Matrix.M32 = 0.0;
	Matrix.M33 = 1.0;
	Matrix.M34 = 0.0;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateScale(ZEMatrix4x4d& Matrix, double x, double y, double z)
{
	Create(Matrix,
		x, 0.0, 0.0, 0.0,
		0.0, y, 0.0, 0.0,
		0.0, 0.0, z, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateScale(ZEMatrix4x4d& Matrix, const ZEVector3d& Scale)
{
	Create(Matrix,
		Scale.x, 0.0, 0.0, 0.0,
		0.0, Scale.y, 0.0, 0.0,
		0.0, 0.0, Scale.z, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateTranslation(ZEMatrix4x4d& Matrix, double x, double y, double z)
{
	Create(Matrix,
		1.0, 0.0, 0.0, x,
		0.0, 1.0, 0.0, y,
		0.0, 0.0, 1.0, z,
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation, const ZEVector3d& Scale)
{
	double xx = Rotation.x * Rotation.x;
	double xy = Rotation.x * Rotation.y;
	double xz = Rotation.x * Rotation.z;
	double xw = Rotation.x * Rotation.w;
	double yy = Rotation.y * Rotation.y;
	double yz = Rotation.y * Rotation.z;
	double yw = Rotation.y * Rotation.w;
	double zz = Rotation.z * Rotation.z;
	double zw = Rotation.z * Rotation.w;
	double ww = Rotation.w * Rotation.w;

	Matrix.M11 = (1.0 - 2.0 * yy - 2.0 * zz) * Scale.x;
	Matrix.M12 = (		2.0 * xy - 2.0 * zw) * Scale.y;
	Matrix.M13 = (		2.0 * xz + 2.0 * yw) * Scale.z;
	Matrix.M14 = Position.x;				 

	Matrix.M21 = (		2.0 * xy + 2.0 * zw) * Scale.x;
	Matrix.M22 = (1.0 - 2.0 * xx - 2.0 * zz) * Scale.y;
	Matrix.M23 = (		2.0 * yz - 2.0 * xw) * Scale.z;
	Matrix.M24 = Position.y;				 

	Matrix.M31 = (		2.0 * xz - 2.0 * yw) * Scale.x;
	Matrix.M32 = (		2.0 * yz + 2.0 * xw) * Scale.y;
	Matrix.M33 = (1.0 - 2.0 * xx - 2.0 * yy) * Scale.z;
	Matrix.M34 = Position.z;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation)
{
	double xx = Rotation.x * Rotation.x;
	double xy = Rotation.x * Rotation.y;
	double xz = Rotation.x * Rotation.z;
	double xw = Rotation.x * Rotation.w;
	double yy = Rotation.y * Rotation.y;
	double yz = Rotation.y * Rotation.z;
	double yw = Rotation.y * Rotation.w;
	double zz = Rotation.z * Rotation.z;
	double zw = Rotation.z * Rotation.w;
	double ww = Rotation.w * Rotation.w;

	Matrix.M11 = 1.0 - 2.0 * yy - 2.0 * zz;
	Matrix.M12 = 	   2.0 * xy - 2.0 * zw;
	Matrix.M13 = 	   2.0 * xz + 2.0 * yw;
	Matrix.M14 = Position.x;

	Matrix.M21 = 	   2.0 * xy + 2.0 * zw;
	Matrix.M22 = 1.0 - 2.0 * xx - 2.0 * zz;
	Matrix.M23 = 	   2.0 * yz - 2.0 * xw;
	Matrix.M24 = Position.y;

	Matrix.M31 = 	   2.0 * xz - 2.0 * yw;
	Matrix.M32 = 	   2.0 * yz + 2.0 * xw;
	Matrix.M33 = 1.0 - 2.0 * xx - 2.0 * yy;
	Matrix.M34 = Position.z;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEVector3d& Scale)
{
	Create(Matrix,
		Scale.x, 0.0, 0.0, Position.x,
		0.0, Scale.y, 0.0, Position.y,
		0.0, 0.0, Scale.z, Position.z,
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateInvOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation, const ZEVector3d& Scale)
{
	double xx = -Rotation.x * -Rotation.x;
	double xy = -Rotation.x * -Rotation.y;
	double xz = -Rotation.x * -Rotation.z;
	double xw = -Rotation.x *  Rotation.w;
	double yy = -Rotation.y * -Rotation.y;
	double yz = -Rotation.y * -Rotation.z;
	double yw = -Rotation.y *  Rotation.w;
	double zz = -Rotation.z * -Rotation.z;
	double zw = -Rotation.z *  Rotation.w;
	double ww =  Rotation.w *  Rotation.w;

	double InvScaleX = (1.0 / Scale.x);
	Matrix.M11 = (1.0 - 2.0 * yy - 2.0 * zz) * InvScaleX;
	Matrix.M12 = (		 2.0 * xy - 2.0 * zw) * InvScaleX;
	Matrix.M13 = (		 2.0 * xz + 2.0 * yw) * InvScaleX;
	Matrix.M14 = (Matrix.M11 * -Position.x + Matrix.M12 * -Position.y + Matrix.M13 * -Position.z) * InvScaleX;

	double InvScaleY = (1.0 / Scale.y);
	Matrix.M21 = (		 2.0 * xy + 2.0 * zw) * InvScaleY;
	Matrix.M22 = (1.0 - 2.0 * xx - 2.0 * zz) * InvScaleY;
	Matrix.M23 = (		 2.0 * yz - 2.0 * xw) * InvScaleY;
	Matrix.M24 = (Matrix.M21 * -Position.x + Matrix.M22 * -Position.y + Matrix.M23 * -Position.z) * InvScaleY;				 

	double InvScaleZ = (1.0 / Scale.z);
	Matrix.M31 = (		 2.0 * xz - 2.0 * yw) * InvScaleZ;
	Matrix.M32 = (		 2.0 * yz + 2.0 * xw) * InvScaleZ;
	Matrix.M33 = (1.0 - 2.0 * xx - 2.0 * yy) * InvScaleZ;
	Matrix.M34 = (Matrix.M31 * -Position.x + Matrix.M32 * -Position.y + Matrix.M33 * -Position.z) * InvScaleZ;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateInvOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation)
{
	double xx = -Rotation.x * -Rotation.x;
	double xy = -Rotation.x * -Rotation.y;
	double xz = -Rotation.x * -Rotation.z;
	double xw = -Rotation.x *  Rotation.w;
	double yy = -Rotation.y * -Rotation.y;
	double yz = -Rotation.y * -Rotation.z;
	double yw = -Rotation.y *  Rotation.w;
	double zz = -Rotation.z * -Rotation.z;
	double zw = -Rotation.z *  Rotation.w;
	double ww =  Rotation.w *  Rotation.w;

	Matrix.M11 = 1.0 - 2.0 * yy - 2.0 * zz;
	Matrix.M12 = 		2.0 * xy - 2.0 * zw;
	Matrix.M13 = 		2.0 * xz + 2.0 * yw;
	Matrix.M14 = Matrix.M11 * -Position.x + Matrix.M12 * -Position.y + Matrix.M13 * -Position.z;

	Matrix.M21 = 		2.0 * xy + 2.0 * zw;
	Matrix.M22 = 1.0 - 2.0 * xx - 2.0 * zz;
	Matrix.M23 = 		2.0 * yz - 2.0 * xw;
	Matrix.M24 = Matrix.M21 * -Position.x + Matrix.M22 * -Position.y + Matrix.M23 * -Position.z;				 

	Matrix.M31 = 		2.0 * xz - 2.0 * yw;
	Matrix.M32 = 		2.0 * yz + 2.0 * xw;
	Matrix.M33 = 1.0 - 2.0 * xx - 2.0 * yy;
	Matrix.M34 = Matrix.M31 * -Position.x + Matrix.M32 * -Position.y + Matrix.M33 * -Position.z;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateInvOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEVector3d& Scale)
{
	Matrix.M11 = 1.0 / Scale.x;
	Matrix.M12 = 0.0;
	Matrix.M13 = 0.0;
	Matrix.M14 = -Position.x * Matrix.M11;

	Matrix.M21 =  0.0;
	Matrix.M22 = 1.0 / Scale.y;
	Matrix.M23 = 0.0;
	Matrix.M24 = -Position.y * Matrix.M22;				 

	Matrix.M31 = 0.0;
	Matrix.M32 = 0.0;
	Matrix.M33 = 1.0 / Scale.z;
	Matrix.M34 =  -Position.z * Matrix.M33;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateTranslation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position)
{
	Create(Matrix,
		1.0, 0.0, 0.0, Position.x,
		0.0, 1.0, 0.0, Position.y,
		0.0, 0.0, 1.0, Position.z,
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateIdentity(ZEMatrix4x4d& Matrix)
{
	Create(Matrix,
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateZero(ZEMatrix4x4d& Matrix)
{
	Create(Matrix,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0);
}

void ZEMatrix4x4d::CreateViewTransform(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation)
{
	ZEQuaternion RotationInv;
	ZEQuaternion::Conjugate(RotationInv, Rotation);

	double xx = RotationInv.x * RotationInv.x;
	double xy = RotationInv.x * RotationInv.y;
	double xz = RotationInv.x * RotationInv.z;
	double xw = RotationInv.x * RotationInv.w;
	double yy = RotationInv.y * RotationInv.y;
	double yz = RotationInv.y * RotationInv.z;
	double yw = RotationInv.y * RotationInv.w;
	double zz = RotationInv.z * RotationInv.z;
	double zw = RotationInv.z * RotationInv.w;
	double ww = RotationInv.w * RotationInv.w;

	Matrix.M11 = 1.0 - 2.0 * yy - 2.0 * zz;
	Matrix.M12 = 	   2.0 * xy - 2.0 * zw;
	Matrix.M13 = 	   2.0 * xz + 2.0 * yw;
	Matrix.M14 = -Position.x * Matrix.M11 + -Position.y * Matrix.M12 + -Position.z * Matrix.M13;

	Matrix.M21 = 	   2.0 * xy + 2.0 * zw;
	Matrix.M22 = 1.0 - 2.0 * xx - 2.0 * zz;
	Matrix.M23 = 	   2.0 * yz - 2.0 * xw;
	Matrix.M24 = -Position.x * Matrix.M21 + -Position.y * Matrix.M22 + -Position.z * Matrix.M23;

	Matrix.M31 = 	   2.0 * xz - 2.0 * yw;
	Matrix.M32 = 	   2.0 * yz + 2.0 * xw;
	Matrix.M33 = 1.0 - 2.0 * xx - 2.0 * yy;
	Matrix.M34 = -Position.x * Matrix.M31 + -Position.y * Matrix.M32 + -Position.z * Matrix.M33;

	Matrix.M41 = 0.0;
	Matrix.M42 = 0.0;
	Matrix.M43 = 0.0;
	Matrix.M44 = 1.0;
}

void ZEMatrix4x4d::CreateLookAtTransform(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEVector3d& Target, const ZEVector3d& Up)
{
	ZEVector3d XAxis;
	ZEVector3d YAxis;
	ZEVector3d ZAxis;

	ZEVector3d::Normalize(ZAxis, Target - Position);
	ZEVector3d::CrossProduct(XAxis, Up, ZAxis);
	ZEVector3d::Normalize(XAxis, XAxis);
	ZEVector3d::CrossProduct(YAxis, ZAxis, XAxis);

	ZEMatrix4x4d::Create(Matrix, 
		XAxis.x, XAxis.y, XAxis.z, -ZEVector3d::DotProduct(XAxis, Position),
		YAxis.x, YAxis.y, YAxis.z, -ZEVector3d::DotProduct(YAxis, Position),
		ZAxis.x, ZAxis.y, ZAxis.z, -ZEVector3d::DotProduct(ZAxis, Position),
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateOrthographicProjection(ZEMatrix4x4d& Matrix, double Width, double Height, double NearZ, double FarZ)
{
	Create(Matrix,
		2.0/Width, 0.0, 0.0, 0.0, 
		0.0, 2.0/Height, 0.0, 0.0, 
		0.0, 0.0, 1.0/(FarZ - NearZ), NearZ/(NearZ - FarZ), 
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreatePerspectiveProjection(ZEMatrix4x4d& Matrix, double FOV, double AspectRatio, double NearZ, double FarZ)
{
	double Ys = 1.0f / ZEAngle::Tan((float)FOV * 0.5f);
	double Xs = Ys / AspectRatio;

	Create(Matrix, 
		Xs, 0.0, 0.0, 0.0,
		0.0, Ys, 0.0, 0.0,
		0.0, 0.0, FarZ / (FarZ - NearZ), -NearZ * FarZ / (FarZ - NearZ),
		0.0, 0.0, 1.0, 0.0);
}

void ZEMatrix4x4d::CreateViewPortTransform(ZEMatrix4x4d& Matrix, double Left, double Right, double Top, double Bottom, double NearZ, double FarZ)
{
	Create(Matrix,
		2.0/(Right - Left), 0.0, 0.0, (Left + Right)/(Left - Right), 
		0.0, 2.0/(Top - Bottom), 0.0, (Top + Bottom)/(Bottom - Top), 
		0.0, 0.0, 1.0/(FarZ - NearZ), NearZ/(NearZ - FarZ), 
		0.0, 0.0, 0.0, 1.0);
}

void ZEMatrix4x4d::CreateScreenTransform(ZEMatrix4x4d& Matrix, double ScreenWidth, double ScreenHeight)
{
	double Xs = ScreenWidth / 2.0;
	double Ys = ScreenHeight / -2.0;

	Create(Matrix,
		Xs, 0.0, 0.0, Xs,
		0.0, Ys, 0.0, -Ys,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

double ZEMatrix4x4d::Determinant(const ZEMatrix4x4d& Matrix)
{
	ZEMatrix3x3d A, B, C, D;
	ZEMatrix3x3d::Create(A, 
		Matrix.M22, Matrix.M23, Matrix.M24, 
		Matrix.M32, Matrix.M33, Matrix.M34, 
		Matrix.M42, Matrix.M43, Matrix.M44);

	ZEMatrix3x3d::Create(B, 
		Matrix.M21, Matrix.M23, Matrix.M24, 
		Matrix.M31, Matrix.M33, Matrix.M34, 
		Matrix.M41, Matrix.M43, Matrix.M44);

	ZEMatrix3x3d::Create(C, 
		Matrix.M21, Matrix.M22, Matrix.M24, 
		Matrix.M31, Matrix.M32, Matrix.M34, 
		Matrix.M41, Matrix.M42, Matrix.M44);

	ZEMatrix3x3d::Create(D, 
		Matrix.M21, Matrix.M22, Matrix.M23, 
		Matrix.M31, Matrix.M32, Matrix.M33, 
		Matrix.M41, Matrix.M42, Matrix.M43);

	return Matrix.M11 * A.Determinant() + Matrix.M13 * C.Determinant() - Matrix.M12 * B.Determinant() - Matrix.M14 * D.Determinant();
}

double ZEMatrix4x4d::Determinant() const
{
	return ZEMatrix4x4d::Determinant(*this);
}

ZEMatrix4x4d ZEMatrix4x4d::Inverse() const
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Inverse(Temp, *this);
	return Temp;
}

ZEMatrix4x4d ZEMatrix4x4d::Transpose() const
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Transpose(Temp, *this);
	return Temp;
}

void ZEMatrix4x4d::InverseSelf()
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Inverse(Temp, *this);
	*this = Temp;
}

void ZEMatrix4x4d::TransposeSelf()
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Transpose(Temp, *this);
	*this = Temp;
}

bool ZEMatrix4x4d::Equals(const ZEMatrix4x4d& Matrix) const
{
	for (ZESize I = 0; I < 16; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > ZE_ZERO_THRESHOLD)
			return false;

	return true;
}

bool ZEMatrix4x4d::Equals(const ZEMatrix4x4d& Matrix, double Threshold) const
{
	for (ZESize I = 0; I < 16; I++)
		if (ZEMath::Abs(MA[I] - Matrix.MA[I]) > Threshold)
			return false;

	return true;
}

ZEVector3d ZEMatrix4x4d::GetTranslation()
{
	return ZEVector3d(M14, M24, M34);
}

ZEQuaternion ZEMatrix4x4d::GetRotation()
{
	ZEVector3d TempScale;
	TempScale.x = ZEVector3d(M11, M21, M31).Length();
	TempScale.y = ZEVector3d(M12, M22, M32).Length();
	TempScale.z = ZEVector3d(M13, M23, M33).Length();

	ZEMatrix4x4d ScaleMatrix;
	ZEMatrix4x4d::CreateScale(ScaleMatrix, TempScale);

	ZEMatrix4x4d RotationMatrix = (*this) * ScaleMatrix.Inverse();

	ZEQuaternion Rotation;
	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix.ToMatrix4x4());
	return Rotation;
}

ZEVector3d ZEMatrix4x4d::GetScale()
{
	ZEVector3d Scale;

	Scale.x = ZEVector3d(M11, M21, M31).Length();
	Scale.y = ZEVector3d(M12, M22, M32).Length();
	Scale.z = ZEVector3d(M13, M23, M33).Length();

	return Scale;
}

void ZEMatrix4x4d::GetDecomposition(ZEVector3d& Translation, ZEQuaternion& Rotation, ZEVector3d& Scale, const ZEMatrix4x4d& Matrix)
{
	Translation.x = Matrix.M14;
	Translation.y = Matrix.M24;
	Translation.z = Matrix.M34;

	Scale.x = ZEVector3d(Matrix.M11, Matrix.M21, Matrix.M31).Length();
	Scale.y = ZEVector3d(Matrix.M12, Matrix.M22, Matrix.M32).Length();
	Scale.z = ZEVector3d(Matrix.M13, Matrix.M23, Matrix.M33).Length();

	ZEMatrix4x4d TempScaleMatrix;
	ZEMatrix4x4d::CreateScale(TempScaleMatrix, Scale);

	ZEMatrix4x4d RotationMatrix = Matrix * TempScaleMatrix.Inverse();

	ZEQuaternion::CreateFromMatrix(Rotation, RotationMatrix.ToMatrix4x4());
}

void ZEMatrix4x4d::FromMatrix4x4(const ZEMatrix4x4& Matrix)
{
	for (ZESize I = 0; I < 4; I++)
		for (ZESize J = 0; J < 4; J++)
			this->M[I][J] = (double)Matrix.M[I][J];
}

ZEMatrix4x4 ZEMatrix4x4d::ToMatrix4x4() const
{
	return ZEMatrix4x4((float)M11, (float)M12, (float)M13, (float)M14, 
						(float)M21, (float)M22, (float)M23, (float)M24, 
						(float)M31, (float)M32, (float)M33, (float)M34,
						(float)M41, (float)M42, (float)M43, (float)M44);
}

ZEMatrix4x4d ZEMatrix4x4d::operator+(const ZEMatrix4x4d& RightOperand) const 
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Add(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4d ZEMatrix4x4d::operator-(const ZEMatrix4x4d& RightOperand) const 
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Sub(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4d ZEMatrix4x4d::operator*(const ZEMatrix4x4d& RightOperand) const 
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Multiply(Temp, *this, RightOperand);
	return Temp;
}

ZEVector3d ZEMatrix4x4d::operator*(const ZEVector3d& RightOperand) const
{
	ZEVector3d Temp;
	ZEMatrix4x4d::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEVector4d ZEMatrix4x4d::operator*(const ZEVector4d& RightOperand) const
{
	ZEVector4d Temp;
	ZEMatrix4x4d::Transform(Temp, *this, RightOperand);
	return Temp;
}

ZEMatrix4x4d ZEMatrix4x4d::operator*(double S) 
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Scale(Temp, *this, S);
	return Temp;
}

ZEMatrix4x4d& ZEMatrix4x4d::operator += (const ZEMatrix4x4d& RightOperand)
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Add(Temp, *this, RightOperand);
	*this = Temp;
	return *this;
}

ZEMatrix4x4d& ZEMatrix4x4d::operator -= (const ZEMatrix4x4d& RightOperand)
{
	Sub(*this, *this, RightOperand);
	return *this;
}

ZEMatrix4x4d& ZEMatrix4x4d::operator *= (const ZEMatrix4x4d& RightOperand)
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Multiply(Temp, *this, RightOperand);
	return *this = Temp;
}

ZEMatrix4x4d& ZEMatrix4x4d::operator *= (double S)
{
	ZEMatrix4x4d::Scale(*this, *this, S);
	return *this;
}

bool ZEMatrix4x4d::operator == (const ZEMatrix4x4d& M) const 
{
	return Equals(M);
}

bool ZEMatrix4x4d::operator != (const ZEMatrix4x4d& M) const 
{
	return !Equals(M);
}

double ZEMatrix4x4d::operator[](ZESize Index) const
{
	return MA[Index];
}

double& ZEMatrix4x4d::operator[](ZESize Index)
{
	return MA[Index];
}

ZEMatrix4x4d::ZEMatrix4x4d(double M11, double M12, double M13, double M14,
						 double M21, double M22, double M23, double M24,
						 double M31, double M32, double M33, double M34,
						 double M41, double M42, double M43, double M44)
{
	ZEMatrix4x4d::Create(*this, M11, M12, M13, M14, M21, M22, M23, M24, M31, M32, M33, M34, M41, M42, M43, M44);  
}

ZEMatrix4x4d::ZEMatrix4x4d(double M[16])
{
	ZEMatrix4x4d::Create(*this, M);
}

ZEMatrix4x4d::ZEMatrix4x4d()
{

}

ZEMatrix4x4d operator*(double S, const ZEMatrix4x4d& RightOperand)
{
	ZEMatrix4x4d Temp;
	ZEMatrix4x4d::Scale(Temp, RightOperand, S);
	return Temp;
}
