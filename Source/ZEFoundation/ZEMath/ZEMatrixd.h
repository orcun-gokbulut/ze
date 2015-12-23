//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMatrixd.h
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

#include "ZEMath/ZEMatrix.h"

class ZEVector2d;
class ZEVector3d;
class ZEVector4d;

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
