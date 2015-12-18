//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrix.h
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
#ifndef	__ZE_MATH_MATRIX_H__
#define __ZE_MATH_MATRIX_H__

#include "ZETypes.h"
#include "ZEMeta\ZEBuiltIn.h"

class ZEVector2;
class ZEVector2d;
class ZEVector3;
class ZEVector3d;
class ZEVector4;
class ZEVector4d;
class ZEQuaternion;

enum ZERotationOrder
{
	ZE_RO_XYZ,
	ZE_RO_XZY,
	ZE_RO_YXZ,
	ZE_RO_YZX,
	ZE_RO_ZXY,
	ZE_RO_ZYX
};

// ZEMatrix3x3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEMatrix3x3
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				float	M11,M21,M31,
						M12,M22,M32,
						M13,M23,M33;
			};
			float M[3][3];
			float MA[9];
		};

		static const ZEMatrix3x3		Zero;
		static const ZEMatrix3x3		Identity;
		
		static void						Create(ZEMatrix3x3& Matrix, float M11, float M12, float M13,
											   float M21, float M22, float M23,
											   float M31, float M32, float M33);
		static void						Create(ZEMatrix3x3& Matrix, float* M);
		static void						CreateRotation(ZEMatrix3x3& Matrix, const ZEQuaternion& Rotation);
		static void						CreateRotation(ZEMatrix3x3& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder = ZE_RO_XYZ);
		static void						CreateRotationX(ZEMatrix3x3& Matrix, float Pitch);
		static void						CreateRotationY(ZEMatrix3x3& Matrix, float Yawn);
		static void						CreateRotationZ(ZEMatrix3x3& Matrix, float Roll);
		static void						CreateScale(ZEMatrix3x3& Matrix, float x, float y, float z);
		static void						CreateScale(ZEMatrix3x3& Matrix, const ZEVector3& Scale);
		static void						CreateIdentity(ZEMatrix3x3& Matrix);
		static void						CreateZero(ZEMatrix3x3& Matrix);

		static void						CreateTranslation2D(ZEMatrix3x3& Matrix, float x, float y);
		static void						CreateTranslation2D(ZEMatrix3x3& Matrix, const ZEVector2& Translation);
		static void						CreateRotation2D(ZEMatrix3x3& Matrix, float Angle);
		static void						CreateScale2D(ZEMatrix3x3& Matrix, float x, float y);
		static void						CreateScale2D(ZEMatrix3x3& Matrix, const ZEVector2& Scale);
		static void						CreateOrientation2D(ZEMatrix3x3& Matrix, const ZEVector2& Translation, float Angle, const ZEVector2& Scale);

		static void						Add(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B);
		static void						Sub(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B);
		static void						Multiply(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, const ZEMatrix3x3 &B);
		static void						Scale(ZEMatrix3x3 &Out, const ZEMatrix3x3 &A, float s);
		static void						Transpose(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix);
		static float					Determinant(const ZEMatrix3x3 &Matrix);
		static bool						Inverse(ZEMatrix3x3 &Out, const ZEMatrix3x3 &Matrix);

		bool							Equals(const ZEMatrix3x3& Matrix) const;
		bool							Equals(const ZEMatrix3x3& Matrix, float Threshold);

		static void						Transform(ZEVector2& Out, const ZEMatrix3x3 &Matrix, const ZEVector2& Vector);
		static void						Transform(ZEVector3& Out, const ZEMatrix3x3 &Matrix, const ZEVector3& Vector);
		static void						Transform(ZEVector4& Out, const ZEMatrix3x3 &Matrix, const ZEVector4& Vector);

		float							Determinant() const;

		ZEMatrix3x3						Inverse() const;
		ZEMatrix3x3						Transpose() const;

		void							InverseSelf();
		void							TransposeSelf();

		ZEQuaternion					GetRotation();
		ZEVector3						GetScale();
		static void						GetDecomposition(ZEQuaternion& Rotation, ZEVector3& Scale, const ZEMatrix3x3& Matrix);

		ZEVector2						Get2DTranslation();
		float							Get2DRotation();
		ZEVector2						Get2DScale();
		static void						Get2DDecomposition(ZEVector2& Translation, float& Rotation, ZEVector2& Scale, const ZEMatrix3x3& Matrix);

		ZEMatrix3x3						operator+(const ZEMatrix3x3 &RightOperand) const;
		ZEMatrix3x3						operator-(const ZEMatrix3x3 &RightOperand) const;
		ZEMatrix3x3						operator*(const ZEMatrix3x3 &RightOperand) const;
		ZEMatrix3x3						operator*(float S);

		ZEMatrix3x3&					operator += (const ZEMatrix3x3 &RightOperand);
		ZEMatrix3x3&					operator -= (const ZEMatrix3x3 &RightOperand);
		ZEMatrix3x3&					operator *= (const ZEMatrix3x3 &RightOperand);
		ZEMatrix3x3&					operator *= (float S);

		ZEVector2						operator*(const ZEVector2 &RightOperand) const;
		ZEVector3						operator*(const ZEVector3 &RightOperand) const;
		ZEVector4						operator*(const ZEVector4 &RightOperand) const;

		bool							operator == (const ZEMatrix3x3 &M) const;
		bool							operator != (const ZEMatrix3x3 &M) const;
		
		float&							operator[](ZESize Index); 		
		float							operator[](ZESize Index) const;

										ZEMatrix3x3(float M11, float M12, float M13,
													float M21, float M22, float M23,
													float M31, float M32, float M33);
										ZEMatrix3x3(float* M);
										ZEMatrix3x3();
};

ZEMatrix3x3 operator*(float S, const ZEMatrix3x3& RightOperand);

// ZEMatrix3x3d
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEMatrix3x3d
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				double	M11,M21,M31,
						M12,M22,M32,
						M13,M23,M33;
			};
			double M[3][3];
			double MA[9];
		};

		static const ZEMatrix3x3d		Zero;
		static const ZEMatrix3x3d		Identity;

		static void						Create(ZEMatrix3x3d& Matrix, double M11, double M12, double M13,
																	 double M21, double M22, double M23,
																	 double M31, double M32, double M33);
		static void						Create(ZEMatrix3x3d& Matrix, double* M);
		static void						CreateRotation(ZEMatrix3x3d& Matrix, const ZEQuaternion& Rotation);
		static void						CreateRotation(ZEMatrix3x3d& Matrix, double Pitch, double Yawn, double Roll, ZERotationOrder RotationOrder = ZE_RO_XYZ);
		static void						CreateRotationX(ZEMatrix3x3d& Matrix, double Pitch);
		static void						CreateRotationY(ZEMatrix3x3d& Matrix, double Yawn);
		static void						CreateRotationZ(ZEMatrix3x3d& Matrix, double Roll);
		static void						CreateScale(ZEMatrix3x3d& Matrix, double x, double y, double z);
		static void						CreateScale(ZEMatrix3x3d& Matrix, const ZEVector3d& Scale);
		static void						CreateIdentity(ZEMatrix3x3d& Matrix);
		static void						CreateZero(ZEMatrix3x3d& Matrix);

		static void						CreateTranslation2D(ZEMatrix3x3d& Matrix, double x, double y);
		static void						CreateTranslation2D(ZEMatrix3x3d& Matrix, const ZEVector2d& Translation);
		static void						CreateRotation2D(ZEMatrix3x3d& Matrix, double Angle);
		static void						CreateScale2D(ZEMatrix3x3d& Matrix, double x, double y);
		static void						CreateScale2D(ZEMatrix3x3d& Matrix, const ZEVector2d& Scale);
		static void						CreateOrientation2D(ZEMatrix3x3d& Matrix, const ZEVector2d& Translation, double Angle, const ZEVector2d& Scale);

		static void						Add(ZEMatrix3x3d& Out, const ZEMatrix3x3d& A, const ZEMatrix3x3d& B);
		static void						Sub(ZEMatrix3x3d& Out, const ZEMatrix3x3d& A, const ZEMatrix3x3d& B);
		static void						Multiply(ZEMatrix3x3d& Out, const ZEMatrix3x3d& A, const ZEMatrix3x3d& B);
		static void						Scale(ZEMatrix3x3d& Out, const ZEMatrix3x3d& A, double s);
		static void						Transpose(ZEMatrix3x3d& Out, const ZEMatrix3x3d& Matrix);
		static double					Determinant(const ZEMatrix3x3d& Matrix);
		static bool						Inverse(ZEMatrix3x3d& Out, const ZEMatrix3x3d& Matrix);

		bool							Equals(const ZEMatrix3x3d& Matrix) const;
		bool							Equals(const ZEMatrix3x3d& Matrix, double Threshold);

		static void						Transform(ZEVector2d& Out, const ZEMatrix3x3d& Matrix, const ZEVector2d& Vector);
		static void						Transform(ZEVector3d& Out, const ZEMatrix3x3d& Matrix, const ZEVector3d& Vector);
		static void						Transform(ZEVector4d& Out, const ZEMatrix3x3d& Matrix, const ZEVector4d& Vector);

		double							Determinant() const;

		ZEMatrix3x3d					Inverse() const;
		ZEMatrix3x3d					Transpose() const;

		void							InverseSelf();
		void							TransposeSelf();

		ZEQuaternion					GetRotation();
		ZEVector3d						GetScale();
		static void						GetDecomposition(ZEQuaternion& Rotation, ZEVector3d& Scale, const ZEMatrix3x3d& Matrix);

		ZEVector2d						Get2DTranslation();
		double							Get2DRotation();
		ZEVector2d						Get2DScale();
		static void						Get2DDecomposition(ZEVector2d& Translation, double& Rotation, ZEVector2d& Scale, const ZEMatrix3x3d& Matrix);

		ZEMatrix3x3						ToMatrix3x3() const;

		ZEMatrix3x3d					operator+(const ZEMatrix3x3d& RightOperand) const;
		ZEMatrix3x3d					operator-(const ZEMatrix3x3d& RightOperand) const;
		ZEMatrix3x3d					operator*(const ZEMatrix3x3d& RightOperand) const;
		ZEMatrix3x3d					operator*(double S);

		ZEMatrix3x3d&					operator += (const ZEMatrix3x3d& RightOperand);
		ZEMatrix3x3d&					operator -= (const ZEMatrix3x3d& RightOperand);
		ZEMatrix3x3d&					operator *= (const ZEMatrix3x3d& RightOperand);
		ZEMatrix3x3d&					operator *= (double S);

		ZEVector2d						operator*(const ZEVector2d& RightOperand) const;
		ZEVector3d						operator*(const ZEVector3d& RightOperand) const;
		ZEVector4d						operator*(const ZEVector4d& RightOperand) const;

		bool							operator == (const ZEMatrix3x3d& M) const;
		bool							operator != (const ZEMatrix3x3d& M) const;

		double&							operator[](ZESize Index); 		
		double							operator[](ZESize Index) const;

		ZEMatrix3x3d(double M11, double M12, double M13,
					double M21, double M22, double M23,
					double M31, double M32, double M33);
		ZEMatrix3x3d(double* M);
		ZEMatrix3x3d();
};

ZEMatrix3x3d operator*(double S, const ZEMatrix3x3d& RightOperand);

// ZEMatrix4x4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEMatrix4x4
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				float M11, M21, M31, M41,
					  M12, M22, M32, M42,
					  M13, M23, M33, M43,
					  M14, M24, M34, M44;
			};
			float M[4][4];
			float MA[16];
		};

		static const ZEMatrix4x4		Zero;
		static const ZEMatrix4x4		Identity;

		static void						Create(ZEMatrix4x4& Matrix, float M11, float M12, float M13, float M14,
											   float M21, float M22, float M23, float M24,
											   float M31, float M32, float M33, float M34,
											   float M41, float M42, float M43, float M44);
		static void						Create(ZEMatrix4x4& Matrix, float* M);
		static void						CreateRotationX(ZEMatrix4x4& Matrix, float Pitch);
		static void						CreateRotationY(ZEMatrix4x4& Matrix, float Yawn);
		static void						CreateRotationZ(ZEMatrix4x4& Matrix, float Roll);
		static void						CreateRotation(ZEMatrix4x4& Matrix, const ZEQuaternion& Rotation);
		static void						CreateRotation(ZEMatrix4x4& Matrix, float Pitch, float Yawn, float Roll, ZERotationOrder RotationOrder = ZE_RO_XYZ);
		static void						CreateScale(ZEMatrix4x4& Matrix, float x, float y, float z);
		static void						CreateScale(ZEMatrix4x4& Matrix, const ZEVector3& Scale);
		static void						CreateTranslation(ZEMatrix4x4& Matrix, float x, float y, float z);
		static void						CreateTranslation(ZEMatrix4x4& Matrix, const ZEVector3& Position);
		static void						CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Scale);
		static void						CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation);
		static void						CreateOrientation(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEVector3& Scale);
		static void						CreateIdentity(ZEMatrix4x4& Matrix);
		static void						CreateZero(ZEMatrix4x4& Matrix);
		static void						CreateViewTransform(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEQuaternion& Rotation);
		static void						CreateLookAtTransform(ZEMatrix4x4& Matrix, const ZEVector3& Position, const ZEVector3& Target, const ZEVector3& Up);
		static void						CreateOrthographicProjection(ZEMatrix4x4& Matrix, float Width, float Height, float NearZ, float FarZ);
		static void						CreatePerspectiveProjection(ZEMatrix4x4& Matrix, float FOV, float AspectRatio, float NearZ, float FarZ);
		static void						CreateViewPortTransform(ZEMatrix4x4& Matrix, float Left, float Right, float Bottom, float Top, float NearZ, float FarZ);
		static void						CreateScreenTransform(ZEMatrix4x4& Matrix, float ScreenWidth, float ScreenHeight);

		static void						Add(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B);
		static void						Sub(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B);
		static void						Multiply(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, const ZEMatrix4x4 &B);
		static void						Scale(ZEMatrix4x4 &Out, const ZEMatrix4x4 &A, float s);
		static void						Transpose(ZEMatrix4x4 &Out, const ZEMatrix4x4 &Matrix);
		static bool						Inverse(ZEMatrix4x4 &Out, const ZEMatrix4x4 &Matrix);
		static float					Determinant(const ZEMatrix4x4 &Matrix);

		bool							Equals(const ZEMatrix4x4& Matrix) const;
		bool							Equals(const ZEMatrix4x4& Matrix, float Threshold) const;

		static void						Transform(ZEVector3 &Out, const ZEMatrix4x4 &Matrix, const ZEVector3& Vector);
		static void						Transform3x3(ZEVector3 &Out, const ZEMatrix4x4 &Matrix, const ZEVector3& Vector);
		static void						Transform(ZEVector4 &Out, const ZEMatrix4x4 &Matrix, const ZEVector4& Vector);

		float							Determinant() const;
		
		ZEMatrix4x4						Inverse() const;
		ZEMatrix4x4						Transpose() const;
		
		void							InverseSelf();
		void							TransposeSelf();

		ZEVector3						GetTranslation();
		ZEQuaternion					GetRotation();
		ZEVector3						GetScale();
		static void						GetDecomposition(ZEVector3& Translation, ZEQuaternion& Rotation, ZEVector3& Scale, const ZEMatrix4x4& Matrix);

		ZEMatrix4x4						operator+(const ZEMatrix4x4& RightOperand) const;
		ZEMatrix4x4						operator-(const ZEMatrix4x4& RightOperand) const;
		ZEMatrix4x4						operator*(const ZEMatrix4x4& RightOperand) const;
		ZEVector3						operator*(const ZEVector3& RightOperand) const;
		ZEVector4						operator*(const ZEVector4& RightOperand) const;
		ZEMatrix4x4						operator*(float S);

		ZEMatrix4x4&					operator += (const ZEMatrix4x4& RightOperand);
		ZEMatrix4x4&					operator -= (const ZEMatrix4x4& RightOperand);
		ZEMatrix4x4&					operator *= (const ZEMatrix4x4& RightOperand);
		ZEMatrix4x4&					operator *= (float S);

		bool							operator == (const ZEMatrix4x4 &M) const;
		bool							operator != (const ZEMatrix4x4 &M) const;
		
		float&							operator[](ZESize Index); 
		float							operator[](ZESize Index) const;

										ZEMatrix4x4(float M11, float M12, float M13, float M14,
													float M21, float M22, float M23, float M24,
													float M31, float M32, float M33, float M34,
													float M41, float M42, float M43, float M44);
										ZEMatrix4x4(float* M);
										ZEMatrix4x4();
};

ZEMatrix4x4 operator*(float S, const ZEMatrix4x4& RightOperand);

// ZEMatrix4x4d
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEMatrix4x4d
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				double  M11, M21, M31, M41,
						M12, M22, M32, M42,
						M13, M23, M33, M43,
						M14, M24, M34, M44;
			};
			double M[4][4];
			double MA[16];
		};

		static const ZEMatrix4x4d		Zero;
		static const ZEMatrix4x4d		Identity;

		static void						Create(ZEMatrix4x4d& Matrix, double M11, double M12, double M13, double M14,
																	double M21, double M22, double M23, double M24,
																	double M31, double M32, double M33, double M34,
																	double M41, double M42, double M43, double M44);
		static void						Create(ZEMatrix4x4d& Matrix, double* M);
		static void						CreateRotationX(ZEMatrix4x4d& Matrix, double Pitch);
		static void						CreateRotationY(ZEMatrix4x4d& Matrix, double Yawn);
		static void						CreateRotationZ(ZEMatrix4x4d& Matrix, double Roll);
		static void						CreateRotation(ZEMatrix4x4d& Matrix, const ZEQuaternion& Rotation);
		static void						CreateRotation(ZEMatrix4x4d& Matrix, double Pitch, double Yawn, double Roll, ZERotationOrder RotationOrder = ZE_RO_XYZ);
		static void						CreateScale(ZEMatrix4x4d& Matrix, double x, double y, double z);
		static void						CreateScale(ZEMatrix4x4d& Matrix, const ZEVector3d& Scale);
		static void						CreateTranslation(ZEMatrix4x4d& Matrix, double x, double y, double z);
		static void						CreateTranslation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position);
		static void						CreateOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation, const ZEVector3d& Scale);
		static void						CreateOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation);
		static void						CreateOrientation(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEVector3d& Scale);
		static void						CreateIdentity(ZEMatrix4x4d& Matrix);
		static void						CreateZero(ZEMatrix4x4d& Matrix);
		static void						CreateViewTransform(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEQuaternion& Rotation);
		static void						CreateLookAtTransform(ZEMatrix4x4d& Matrix, const ZEVector3d& Position, const ZEVector3d& Target, const ZEVector3d& Up);
		static void						CreateOrthographicProjection(ZEMatrix4x4d& Matrix, double Width, double Height, double NearZ, double FarZ);
		static void						CreatePerspectiveProjection(ZEMatrix4x4d& Matrix, double FOV, double AspectRatio, double NearZ, double FarZ);
		static void						CreateViewPortTransform(ZEMatrix4x4d& Matrix, double Left, double Right, double Bottom, double Top, double NearZ, double FarZ);
		static void						CreateScreenTransform(ZEMatrix4x4d& Matrix, double ScreenWidth, double ScreenHeight);

		static void						Add(ZEMatrix4x4d& Out, const ZEMatrix4x4d& A, const ZEMatrix4x4d& B);
		static void						Sub(ZEMatrix4x4d& Out, const ZEMatrix4x4d& A, const ZEMatrix4x4d& B);
		static void						Multiply(ZEMatrix4x4d& Out, const ZEMatrix4x4d& A, const ZEMatrix4x4d& B);
		static void						Scale(ZEMatrix4x4d& Out, const ZEMatrix4x4d& A, double s);
		static void						Transpose(ZEMatrix4x4d& Out, const ZEMatrix4x4d& Matrix);
		static bool						Inverse(ZEMatrix4x4d& Out, const ZEMatrix4x4d& Matrix);
		static double					Determinant(const ZEMatrix4x4d& Matrix);

		bool							Equals(const ZEMatrix4x4d& Matrix) const;
		bool							Equals(const ZEMatrix4x4d& Matrix, double Threshold) const;

		static void						Transform(ZEVector3d& Out, const ZEMatrix4x4d& Matrix, const ZEVector3d& Vector);
		static void						Transform3x3(ZEVector3d& Out, const ZEMatrix4x4d& Matrix, const ZEVector3d& Vector);
		static void						Transform(ZEVector4d& Out, const ZEMatrix4x4d& Matrix, const ZEVector4d& Vector);

		double							Determinant() const;

		ZEMatrix4x4d					Inverse() const;
		ZEMatrix4x4d					Transpose() const;

		void							InverseSelf();
		void							TransposeSelf();

		ZEVector3d						GetTranslation();
		ZEQuaternion					GetRotation();
		ZEVector3d						GetScale();
		static void						GetDecomposition(ZEVector3d& Translation, ZEQuaternion& Rotation, ZEVector3d& Scale, const ZEMatrix4x4d& Matrix);

		ZEMatrix4x4						ToMatrix4x4() const;

		ZEMatrix4x4d					operator+(const ZEMatrix4x4d& RightOperand) const;
		ZEMatrix4x4d					operator-(const ZEMatrix4x4d& RightOperand) const;
		ZEMatrix4x4d					operator*(const ZEMatrix4x4d& RightOperand) const;
		ZEVector3d						operator*(const ZEVector3d& RightOperand) const;
		ZEVector4d						operator*(const ZEVector4d& RightOperand) const;
		ZEMatrix4x4d					operator*(double S);

		ZEMatrix4x4d&					operator += (const ZEMatrix4x4d& RightOperand);
		ZEMatrix4x4d&					operator -= (const ZEMatrix4x4d& RightOperand);
		ZEMatrix4x4d&					operator *= (const ZEMatrix4x4d& RightOperand);
		ZEMatrix4x4d&					operator *= (double S);

		bool							operator == (const ZEMatrix4x4d &M) const;
		bool							operator != (const ZEMatrix4x4d &M) const;

		double&							operator[](ZESize Index); 
		double							operator[](ZESize Index) const;

		ZEMatrix4x4d(double M11, double M12, double M13, double M14,
					 double M21, double M22, double M23, double M24,
					 double M31, double M32, double M33, double M34,
					 double M41, double M42, double M43, double M44);
		ZEMatrix4x4d(double* M);
		ZEMatrix4x4d();
};

ZEMatrix4x4d operator*(double S, const ZEMatrix4x4d& RightOperand);


#endif
