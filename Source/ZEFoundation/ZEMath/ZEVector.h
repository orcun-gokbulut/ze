//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVector.h
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
#ifndef	__ZE_MATH_VECTOR_H__
#define __ZE_MATH_VECTOR_H__

#include <ZETypes.h>
#include "ZEMeta/ZEBuiltIn.h"

// ZEVector2
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector2
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				float x, y;
			};
			float M[2];
		};

	public:
		static const ZEVector2		Zero;
		static const ZEVector2		One;
		static const ZEVector2		UnitX;
		static const ZEVector2		UnitY;
		static const ZEVector2		MaxValue;
		static const ZEVector2		MinValue;

		static void					Create(ZEVector2& Out, float x, float y);
		static void					Create(ZEVector2& Out, const ZEVector2 &Start, const ZEVector2 &End);
		static void					CreateFromPolar(ZEVector2& Out, float Radius, float Theta);
		static void					ConvertToPolar(ZEVector2& In, float& Radius, float& Theta);

		static void					Add(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B);
		static void					Substution(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B);
		static void					Multiply(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B);
		static void					Divide(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B);
		static void					Divide(ZEVector2 &Out, const ZEVector2 &A, float s);
		static float				DotProduct(const ZEVector2 &A, const ZEVector2 &B);
		static void					Scale(ZEVector2 &Out, const ZEVector2 &A, float s);
		static float				Length(const ZEVector2 &A);
		static float				LengthSquare(const ZEVector2 &A);
		static float				Distance(const ZEVector2 &A, const ZEVector2 &B);
		static float				DistanceSquare(const ZEVector2 &A, const ZEVector2 &B);
		static float				SignedAngle(const ZEVector2& A, const ZEVector2& B);
		static void					Normalize(ZEVector2 &Out, const ZEVector2 &A);
		static void					Lerp(ZEVector2 &Out, const ZEVector2 &A, const ZEVector2 &B, float Factor);	
		static void					Max(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B);
		static void					Min(ZEVector2& Out, const ZEVector2& A, const ZEVector2& B);
		static void					Clamp(ZEVector2& Out, const ZEVector2& Vector, float MinValue, float MaxValue);
		static void					Clamp(ZEVector2& Out, const ZEVector2& Vector, const ZEVector2& MinValue, const ZEVector2& MaxValue);
		static void					Saturate(ZEVector2& Out, const ZEVector2& Vector);

		bool						IsValid() const;
		bool						IsNormalized() const;

		float						Length() const;
		float						LengthSquare() const;
		ZEVector2					Normalize() const;
		void						NormalizeSelf();
		ZEVector2					Clamp(float MinValue, float MaxValue) const;
		ZEVector2					Clamp(const ZEVector2& MinValue, const ZEVector2& MaxValue) const;
		void						ClampSelf(float MinValue, float MaxValue);
		void						ClampSelf(const ZEVector2& MinValue, const ZEVector2& MaxValue);
		ZEVector2					Saturate() const;
		void						SaturateSelf();
		float						Min() const;
		float						Max() const;

		bool						Equals(const ZEVector2& Vector) const;
		bool						Equals(const ZEVector2& Vector, float Threshold) const;

		ZEVector2					operator +(const ZEVector2 &RightOperand) const;
		ZEVector2					operator -(const ZEVector2 &RightOperand) const;
		ZEVector2					operator *(const ZEVector2 &RightOperand) const;
		ZEVector2					operator *(float s) const;
		ZEVector2					operator /(const ZEVector2 &RightOperand) const;
		ZEVector2					operator /(float s) const;

		ZEVector2					operator-() const;

		ZEVector2&					operator +=(const ZEVector2 &RightOperand);
		ZEVector2&					operator -=(const ZEVector2 &RightOperand);
		ZEVector2&					operator *=(const ZEVector2 &RightOperand);
		ZEVector2&					operator *=(float s);
		ZEVector2&					operator /=(const ZEVector2 &RightOperand);
		ZEVector2&					operator /=(float s);

		bool						operator ==(const ZEVector2 &RightOperand) const;
		bool						operator !=(const ZEVector2 &RightOperand) const;
		
		float						operator[](ZESize Index) const;
		float&						operator[](ZESize Index);

									ZEVector2(float x, float y);
									ZEVector2(const ZEVector2 &Start, const ZEVector2 &End);
									ZEVector2();
};

ZEVector2 operator*(float S, const ZEVector2& RightOperand);

// ZEVector2d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector2d
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				double x, y;
			};
			double M[2];
		};

	public:
		static const ZEVector2d		Zero;
		static const ZEVector2d		One;
		static const ZEVector2d		UnitX;
		static const ZEVector2d		UnitY;
		static const ZEVector2d		MaxValue;
		static const ZEVector2d		MinValue;

		static void					Create(ZEVector2d& Out, double x, double y);
		static void					Create(ZEVector2d& Out, const ZEVector2d& Start, const ZEVector2d& End);
		static void					CreateFromPolar(ZEVector2d& Out, double Radius, double Theta);
		static void					ConvertToPolar(ZEVector2d& In, double& Radius, double& Theta);

		static void					Add(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B);
		static void					Substution(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B);
		static void					Multiply(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B);
		static void					Divide(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B);
		static void					Divide(ZEVector2d& Out, const ZEVector2d& A, double s);
		static double				DotProduct(const ZEVector2d& A, const ZEVector2d& B);
		static void					Scale(ZEVector2d& Out, const ZEVector2d& A, double s);
		static double				Length(const ZEVector2d& A);
		static double				LengthSquare(const ZEVector2d& A);
		static double				Distance(const ZEVector2d& A, const ZEVector2d& B);
		static double				DistanceSquare(const ZEVector2d& A, const ZEVector2d& B);
		static double				SignedAngle(const ZEVector2d& A, const ZEVector2d& B);
		static void					Normalize(ZEVector2d& Out, const ZEVector2d& A);
		static void					Lerp(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B, double Factor);	
		static void					Max(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B);
		static void					Min(ZEVector2d& Out, const ZEVector2d& A, const ZEVector2d& B);
		static void					Clamp(ZEVector2d& Out, const ZEVector2d& Vector, double MinValue, double MaxValue);
		static void					Clamp(ZEVector2d& Out, const ZEVector2d& Vector, const ZEVector2d& MinValue, const ZEVector2d& MaxValue);
		static void					Saturate(ZEVector2d& Out, const ZEVector2d& Vector);

		static ZEVector2			ToVector2(const ZEVector2d& Vector);

		bool						IsValid() const;
		bool						IsNormalized() const;

		double						Length() const;
		double						LengthSquare() const;
		ZEVector2d					Normalize() const;
		void						NormalizeSelf();
		ZEVector2d					Clamp(double MinValue, double MaxValue) const;
		ZEVector2d					Clamp(const ZEVector2d& MinValue, const ZEVector2d& MaxValue) const;
		void						ClampSelf(double MinValue, double MaxValue);
		void						ClampSelf(const ZEVector2d& MinValue, const ZEVector2d& MaxValue);
		ZEVector2d					Saturate() const;
		void						SaturateSelf();
		double						Min() const;
		double						Max() const;

		bool						Equals(const ZEVector2d& Vector) const;
		bool						Equals(const ZEVector2d& Vector, double Threshold) const;

		ZEVector2					ToVector2() const;

		ZEVector2d					operator +(const ZEVector2d &RightOperand) const;
		ZEVector2d					operator -(const ZEVector2d &RightOperand) const;
		ZEVector2d					operator *(const ZEVector2d &RightOperand) const;
		ZEVector2d					operator *(double s) const;
		ZEVector2d					operator /(const ZEVector2d &RightOperand) const;
		ZEVector2d					operator /(double s) const;

		ZEVector2d					operator-() const;

		ZEVector2d&					operator +=(const ZEVector2d &RightOperand);
		ZEVector2d&					operator -=(const ZEVector2d &RightOperand);
		ZEVector2d&					operator *=(const ZEVector2d &RightOperand);
		ZEVector2d&					operator *=(double s);
		ZEVector2d&					operator /=(const ZEVector2d &RightOperand);
		ZEVector2d&					operator /=(double s);

		bool						operator ==(const ZEVector2d &RightOperand) const;
		bool						operator !=(const ZEVector2d &RightOperand) const;

		double						operator[](ZESize Index) const;
		double&						operator[](ZESize Index);

		ZEVector2d(double x, double y);
		ZEVector2d(const ZEVector2d &Start, const ZEVector2d &End);
		ZEVector2d();
};

ZEVector2d operator*(double S, const ZEVector2d& RightOperand);

// ZEVector3
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector3
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct 
			{
				float x, y, z;
			};
			float M[3];
		};

	public:
		static const ZEVector3		Zero;
		static const ZEVector3		One;
		static const ZEVector3		UnitX;
		static const ZEVector3		UnitY;
		static const ZEVector3		UnitZ;
		static const ZEVector3		MaxValue;
		static const ZEVector3		MinValue;

		static void					Create(ZEVector3& Out, float x, float y, float z);
		static void					Create(ZEVector3& Out, const ZEVector3 &Start, const ZEVector3 &End);
		
		static void					CreateFromSpherical(ZEVector3& Out, float Radius, float Theta, float Phi);
		static void					CreateFromCylindirical(ZEVector3& Out, float Radius, float Theta, float Height);

		static void					ConvertToSpherical(const ZEVector3& In, float& Radius, float& Theta, float& Phi);
		static void					ConvertToCylindirical(const ZEVector3& In, float& Radius, float& Theta, float& Height);

		static void					Add(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B);
		static void					Sub(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B);
		static void					Scale(ZEVector3& Out, const ZEVector3& A, float s);
		static void					Multiply(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B);
		static void					Divide(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B);
		static void					Divide(ZEVector3& Out, const ZEVector3& A, float s);
		static float				DotProduct(const ZEVector3& A, const ZEVector3& B);
		static void					CrossProduct(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B);
		static float				Length(const ZEVector3& Vector);
		static float				LengthSquare(const ZEVector3& Vector);
		static void					Normalize(ZEVector3& Out, const ZEVector3& Vector);	
		static float				Distance(const ZEVector3& A, const ZEVector3& B);
		static float				DistanceSquare(const ZEVector3& A, const ZEVector3& B);	
		static void					Lerp(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B, float Factor);	
		static void					Max(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B);
		static void					Min(ZEVector3& Out, const ZEVector3& A, const ZEVector3& B);
		static void					Clamp(ZEVector3& Out, const ZEVector3& Vector, float MinValue, float MaxValue);
		static void					Clamp(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue, const ZEVector3& MaxValue);
		static void					ClampLower(ZEVector3& Out, const ZEVector3& Vector, float MinValue);
		static void					ClampLower(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MinValue);
		static void					ClampUpper(ZEVector3& Out, const ZEVector3& Vector, float MaxValue);
		static void					ClampUpper(ZEVector3& Out, const ZEVector3& Vector, const ZEVector3& MaxValue);
		static void					Saturate(ZEVector3& Out, const ZEVector3& Vector);

		static ZEVector2			ToVector2(const ZEVector3& Vector);

		bool						IsValid() const;
		bool						IsNormalized() const;

		float						Length() const;
		float						LengthSquare() const;

		ZEVector3					Normalize() const;
		void						NormalizeSelf();

		ZEVector3					Clamp(float MinValue, float MaxValue) const;
		ZEVector3					Clamp(const ZEVector3& MinValue, const ZEVector3& MaxValue) const;
		ZEVector3					ClampLower(float MinValue) const;
		ZEVector3					ClampLower(const ZEVector3& MinValue) const;
		ZEVector3					ClampUpper(float MaxValue) const;
		ZEVector3					ClampUpper(const ZEVector3& MaxValue) const;

		void						ClampSelf(float MinValue, float MaxValue);
		void						ClampSelf(const ZEVector3& MinValue, const ZEVector3& MaxValue);
		void						ClampLowerSelf(float MinValue);
		void						ClampLowerSelf(const ZEVector3& MinValue);
		void						ClampUpperSelf(float MaxValue);
		void						ClampUpperSelf(const ZEVector3& MaxValue);

		ZEVector3					Saturate() const;
		void						SaturateSelf();

		float						Min() const;
		float						Max() const;

		bool						Equals(const ZEVector3& Vector) const;
		bool						Equals(const ZEVector3& Vector, float Threshold) const;

		ZEVector2					ToVector2() const;

		ZEVector3					operator+(const ZEVector3& RightOperand) const;
		ZEVector3					operator-(const ZEVector3& RightOperand) const;
		ZEVector3					operator*(const ZEVector3& RightOperand) const;
		ZEVector3					operator*(float s) const;
		ZEVector3					operator/(const ZEVector3& RightOperand) const;
		ZEVector3					operator/(float s) const;
		ZEVector3					operator-() const;

		ZEVector3&					operator += (const ZEVector3 &RightOperand);
		ZEVector3&					operator -= (const ZEVector3 &RightOperand);
		ZEVector3&					operator *= (const ZEVector3 &RightOperand);
		ZEVector3&					operator *= (float S);
		ZEVector3&					operator /= (const ZEVector3 &RightOperand);
		ZEVector3&					operator /= (float S);

		bool						operator == (const ZEVector3 &RightOperand) const;
		bool						operator != (const ZEVector3 &RightOperand) const;

		float						operator[](ZESize Index) const;
		float&						operator[](ZESize Index);

									ZEVector3(float x, float y, float z);
									ZEVector3(ZEVector2& Base, float z);
									ZEVector3(const ZEVector3& Start, const ZEVector3& End);
									ZEVector3();
};

ZEVector3 operator*(float S, const ZEVector3& RightOperand);

// ZEVector3d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector3d
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct 
			{
				double x, y, z;
			};
			double M[3];
		};

	public:
		static const ZEVector3d		Zero;
		static const ZEVector3d		One;
		static const ZEVector3d		UnitX;
		static const ZEVector3d		UnitY;
		static const ZEVector3d		UnitZ;
		static const ZEVector3d		MaxValue;
		static const ZEVector3d		MinValue;

		static void					Create(ZEVector3d& Out, double x, double y, double z);
		static void					Create(ZEVector3d& Out, const ZEVector3d& Start, const ZEVector3d& End);

		static void					CreateFromSpherical(ZEVector3d& Out, double Radius, double Theta, double Phi);
		static void					CreateFromCylindirical(ZEVector3d& Out, double Radius, double Theta, double Height);

		static void					ConvertToSpherical(const ZEVector3d& In, double& Radius, double& Theta, double& Phi);
		static void					ConvertToCylindirical(const ZEVector3d& In, double& Radius, double& Theta, double& Height);

		static void					Add(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B);
		static void					Sub(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B);
		static void					Scale(ZEVector3d& Out, const ZEVector3d& A, double s);
		static void					Multiply(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B);
		static void					Divide(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B);
		static void					Divide(ZEVector3d& Out, const ZEVector3d& A, double s);
		static double				DotProduct(const ZEVector3d& A, const ZEVector3d& B);
		static void					CrossProduct(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B);
		static double				Length(const ZEVector3d& Vector);
		static double				LengthSquare(const ZEVector3d& Vector);
		static void					Normalize(ZEVector3d& Out, const ZEVector3d& Vector);	
		static double				Distance(const ZEVector3d& A, const ZEVector3d& B);
		static double				DistanceSquare(const ZEVector3d& A, const ZEVector3d& B);	
		static void					Lerp(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B, double Factor);	
		static void					Max(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B);
		static void					Min(ZEVector3d& Out, const ZEVector3d& A, const ZEVector3d& B);
		static void					Clamp(ZEVector3d& Out, const ZEVector3d& Vector, double MinValue, double MaxValue);
		static void					Clamp(ZEVector3d& Out, const ZEVector3d& Vector, const ZEVector3d& MinValue, const ZEVector3d& MaxValue);
		static void					ClampLower(ZEVector3d& Out, const ZEVector3d& Vector, double MinValue);
		static void					ClampLower(ZEVector3d& Out, const ZEVector3d& Vector, const ZEVector3d& MinValue);
		static void					ClampUpper(ZEVector3d& Out, const ZEVector3d& Vector, double MaxValue);
		static void					ClampUpper(ZEVector3d& Out, const ZEVector3d& Vector, const ZEVector3d& MaxValue);
		static void					Saturate(ZEVector3d& Out, const ZEVector3d& Vector);

		static ZEVector2d			ToVector2d(const ZEVector3d& Vector);
		static ZEVector3			ToVector3(const ZEVector3d& Vector);

		bool						IsValid() const;
		bool						IsNormalized() const;

		double						Length() const;
		double						LengthSquare() const;

		ZEVector3d					Normalize() const;
		void						NormalizeSelf();

		ZEVector3d					Clamp(double MinValue, double MaxValue) const;
		ZEVector3d					Clamp(const ZEVector3d& MinValue, const ZEVector3d& MaxValue) const;
		ZEVector3d					ClampLower(double MinValue) const;
		ZEVector3d					ClampLower(const ZEVector3d& MinValue) const;
		ZEVector3d					ClampUpper(double MaxValue) const;
		ZEVector3d					ClampUpper(const ZEVector3d& MaxValue) const;

		void						ClampSelf(double MinValue, double MaxValue);
		void						ClampSelf(const ZEVector3d& MinValue, const ZEVector3d& MaxValue);
		void						ClampLowerSelf(double MinValue);
		void						ClampLowerSelf(const ZEVector3d& MinValue);
		void						ClampUpperSelf(double MaxValue);
		void						ClampUpperSelf(const ZEVector3d& MaxValue);

		ZEVector3d					Saturate() const;
		void						SaturateSelf();

		double						Min() const;
		double						Max() const;

		bool						Equals(const ZEVector3d& Vector) const;
		bool						Equals(const ZEVector3d& Vector, double Threshold) const;

		ZEVector2d					ToVector2d() const;
		ZEVector3					ToVector3() const;

		ZEVector3d					operator+(const ZEVector3d& RightOperand) const;
		ZEVector3d					operator-(const ZEVector3d& RightOperand) const;
		ZEVector3d					operator*(const ZEVector3d& RightOperand) const;
		ZEVector3d					operator*(double s) const;
		ZEVector3d					operator/(const ZEVector3d& RightOperand) const;
		ZEVector3d					operator/(double s) const;
		ZEVector3d					operator-() const;

		ZEVector3d&					operator += (const ZEVector3d& RightOperand);
		ZEVector3d&					operator -= (const ZEVector3d& RightOperand);
		ZEVector3d&					operator *= (const ZEVector3d& RightOperand);
		ZEVector3d&					operator *= (double S);
		ZEVector3d&					operator /= (const ZEVector3d& RightOperand);
		ZEVector3d&					operator /= (double S);

		bool						operator == (const ZEVector3d& RightOperand) const;
		bool						operator != (const ZEVector3d& RightOperand) const;

		double						operator[](ZESize Index) const;
		double&						operator[](ZESize Index);

		ZEVector3d(double x, double y, double z);
		ZEVector3d(ZEVector2d& Base, double z);
		ZEVector3d(const ZEVector3d& Start, const ZEVector3d& End);
		ZEVector3d();
};

ZEVector3d operator*(double S, const ZEVector3d& RightOperand);

// ZEVector4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector4
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				float x, y, z, w;
			};
			float M[4];
		};

	public:
		static const ZEVector4		Zero;
		static const ZEVector4		One;
		static const ZEVector4		UnitX;
		static const ZEVector4		UnitY;
		static const ZEVector4		UnitZ;
		static const ZEVector4		UnitW;
		static const ZEVector4		HomogenousUnitX;
		static const ZEVector4		HomogenousUnitY;
		static const ZEVector4		HomogenousUnitZ;
		static const ZEVector4		MaxValue;
		static const ZEVector4		MinValue;

		static void					Create(ZEVector4& Out, float x, float y, float z, float w);
		static void					Create(ZEVector4& Out, const ZEVector4 &Start, const ZEVector4 &End);

		static void					Add(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B);
		static void					Sub(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B);
		static void					Scale(ZEVector4& Out, const ZEVector4& A, float s);
		static void					Multiply(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B);
		static void					Divide(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B);
		static void					Divide(ZEVector4& Out, const ZEVector4& A, float s);
		static float				DotProduct(const ZEVector4& A, const ZEVector4& B);
		static float				Length(const ZEVector4& Vector);
		static float				LengthSquare(const ZEVector4& Vector);
		static void					Normalize(ZEVector4& Out, const ZEVector4& Vector);	
		static float				Distance(const ZEVector4& A, const ZEVector4& B);
		static float				DistanceSquare(const ZEVector4& A, const ZEVector4& B);	
		static void					Lerp(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B, float Factor);
		static void					Max(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B);
		static void					Min(ZEVector4& Out, const ZEVector4& A, const ZEVector4& B);
		static void					Clamp(ZEVector4& Out, const ZEVector4& Vector, float MinValue, float MaxValue);
		static void					Clamp(ZEVector4& Out, const ZEVector4& Vector, const ZEVector4& MinValue, const ZEVector4& MaxValue);
		static void					Saturate(ZEVector4& Out, const ZEVector4& Vector);
		
		static ZEVector2			ToVector2(const ZEVector4& Vector);
		static ZEVector3			ToVector3(const ZEVector4& Vector);

		bool						IsValid() const;
		bool						IsNormalized() const;

		float						Length() const;
		float						LengthSquare() const;
		ZEVector4					Normalize() const;
		void						NormalizeSelf();
		ZEVector4					Clamp(float MinValue, float MaxValue) const;
		ZEVector4					Clamp(const ZEVector4& MinValue, const ZEVector4& MaxValue) const;
		void						ClampSelf(float MinValue, float MaxValue);
		void						ClampSelf(const ZEVector4& MinValue, const ZEVector4& MaxValue);
		ZEVector4					Saturate() const;
		void						SaturateSelf();
		float						Min() const;
		float						Max() const;

		bool						Equals(const ZEVector4& Vector) const;
		bool						Equals(const ZEVector4& Vector, float Threshold) const;

		ZEVector2					ToVector2() const;
		ZEVector3					ToVector3() const;

		ZEVector4					operator+(const ZEVector4 &RightOperand) const;
		ZEVector4					operator-(const ZEVector4 &RightOperand) const;
		ZEVector4					operator*(const ZEVector4 &RightOperand) const;
		ZEVector4					operator*(float S) const;
		ZEVector4					operator/(const ZEVector4 &RightOperand) const;
		ZEVector4					operator/(float S) const;
		ZEVector4					operator-() const;

		ZEVector4&					operator += (const ZEVector4 &RightOperand);
		ZEVector4&					operator -= (const ZEVector4 &RightOperand);
		ZEVector4&					operator *= (const ZEVector4 &RightOperand);
		ZEVector4&					operator *= (float S);
		ZEVector4&					operator /= (const ZEVector4 &RightOperand);
		ZEVector4&					operator /= (float S);

		bool						operator == (const ZEVector4 &RightOperand) const;
		bool						operator != (const ZEVector4 &RightOperand) const;

		float						operator[](ZESize Index) const;
		float&						operator[](ZESize Index);

									ZEVector4(float x, float y, float z, float w);
									ZEVector4(const ZEVector2& Base, float z, float w);
									ZEVector4(const ZEVector3& Base, float w);
									ZEVector4(const ZEVector4& Start, const ZEVector4& End);
									ZEVector4();
};

ZEVector4 operator*(float S, const ZEVector4& RightOperand);

// ZEVector4d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector4d
{
	ZE_BUILTIN_OBJECT

	public:
		union
		{
			struct
			{
				double x, y, z, w;
			};
			double M[4];
		};

	public:
		static const ZEVector4d		Zero;
		static const ZEVector4d		One;
		static const ZEVector4d		UnitX;
		static const ZEVector4d		UnitY;
		static const ZEVector4d		UnitZ;
		static const ZEVector4d		UnitW;
		static const ZEVector4d		HomogenousUnitX;
		static const ZEVector4d		HomogenousUnitY;
		static const ZEVector4d		HomogenousUnitZ;
		static const ZEVector4d		MaxValue;
		static const ZEVector4d		MinValue;

		static void					Create(ZEVector4d& Out, double x, double y, double z, double w);
		static void					Create(ZEVector4d& Out, const ZEVector4d& Start, const ZEVector4d& End);

		static void					Add(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B);
		static void					Sub(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B);
		static void					Scale(ZEVector4d& Out, const ZEVector4d& A, double s);
		static void					Multiply(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B);
		static void					Divide(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B);
		static void					Divide(ZEVector4d& Out, const ZEVector4d& A, double s);
		static double				DotProduct(const ZEVector4d& A, const ZEVector4d& B);
		static double				Length(const ZEVector4d& Vector);
		static double				LengthSquare(const ZEVector4d& Vector);
		static void					Normalize(ZEVector4d& Out, const ZEVector4d& Vector);	
		static double				Distance(const ZEVector4d& A, const ZEVector4d& B);
		static double				DistanceSquare(const ZEVector4d& A, const ZEVector4d& B);	
		static void					Lerp(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B, double Factor);
		static void					Max(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B);
		static void					Min(ZEVector4d& Out, const ZEVector4d& A, const ZEVector4d& B);
		static void					Clamp(ZEVector4d& Out, const ZEVector4d& Vector, double MinValue, double MaxValue);
		static void					Clamp(ZEVector4d& Out, const ZEVector4d& Vector, const ZEVector4d& MinValue, const ZEVector4d& MaxValue);
		static void					Saturate(ZEVector4d& Out, const ZEVector4d& Vector);

		static ZEVector2d			ToVector2d(const ZEVector4d& Vector);
		static ZEVector3d			ToVector3d(const ZEVector4d& Vector);
		static ZEVector4			ToVector4(const ZEVector4d& Vector);

		bool						IsValid() const;
		bool						IsNormalized() const;

		double						Length() const;
		double						LengthSquare() const;
		ZEVector4d					Normalize() const;
		void						NormalizeSelf();
		ZEVector4d					Clamp(double MinValue, double MaxValue) const;
		ZEVector4d					Clamp(const ZEVector4d& MinValue, const ZEVector4d& MaxValue) const;
		void						ClampSelf(double MinValue, double MaxValue);
		void						ClampSelf(const ZEVector4d& MinValue, const ZEVector4d& MaxValue);
		ZEVector4d					Saturate() const;
		void						SaturateSelf();
		double						Min() const;
		double						Max() const;

		bool						Equals(const ZEVector4d& Vector) const;
		bool						Equals(const ZEVector4d& Vector, double Threshold) const;

		ZEVector2d					ToVector2d() const;
		ZEVector3d					ToVector3d() const;
		ZEVector4					ToVector4() const;

		ZEVector4d					operator+(const ZEVector4d& RightOperand) const;
		ZEVector4d					operator-(const ZEVector4d& RightOperand) const;
		ZEVector4d					operator*(const ZEVector4d& RightOperand) const;
		ZEVector4d					operator*(double S) const;
		ZEVector4d					operator/(const ZEVector4d& RightOperand) const;
		ZEVector4d					operator/(double S) const;
		ZEVector4d					operator-() const;

		ZEVector4d&					operator += (const ZEVector4d& RightOperand);
		ZEVector4d&					operator -= (const ZEVector4d& RightOperand);
		ZEVector4d&					operator *= (const ZEVector4d& RightOperand);
		ZEVector4d&					operator *= (double S);
		ZEVector4d&					operator /= (const ZEVector4d& RightOperand);
		ZEVector4d&					operator /= (double S);

		bool						operator == (const ZEVector4d& RightOperand) const;
		bool						operator != (const ZEVector4d& RightOperand) const;

		double						operator[](ZESize Index) const;
		double&						operator[](ZESize Index);

		ZEVector4d(double x, double y, double z, double w);
		ZEVector4d(const ZEVector2d& Base, double z, double w);
		ZEVector4d(const ZEVector3d& Base, double w);
		ZEVector4d(const ZEVector4d& Start, const ZEVector4d& End);
		ZEVector4d();
};

ZEVector4d operator*(double S, const ZEVector4d& RightOperand);

#endif
