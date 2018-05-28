//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVectord.h
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

#include "ZEMath/ZEVector.h"

// ZEVector2d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector2d
{
	ZE_FUNDAMENTAL
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
		bool						IsInfinite() const;
		bool						IsValidNotInf() const;
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
		bool						NotEquals(const ZEVector2d& Vector) const;
		bool						NotEquals(const ZEVector2d& Vector, double Threshold) const;

		void						FromVector2(const ZEVector2& Vector);
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

									ZE_VECTOR_SWIZZLE_2D_DEFINITION(ZEVector2d);

									ZEVector2d(double x, double y);
									ZEVector2d(const ZEVector2d &Start, const ZEVector2d &End);
									ZEVector2d();
};

ZEVector2d operator*(double S, const ZEVector2d& RightOperand);

// ZEVector3d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector3d
{
	ZE_FUNDAMENTAL
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
		bool						IsInfinite() const;
		bool						IsValidNotInf() const;
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
		bool						NotEquals(const ZEVector3d& Vector) const;
		bool						NotEquals(const ZEVector3d& Vector, double Threshold) const;

		void						FromVector3(const ZEVector3& Vector);
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

									ZE_VECTOR_SWIZZLE_3D_DEFINITION(ZEVector3d, ZEVector2d);

									ZEVector3d(double x, double y, double z);
									ZEVector3d(ZEVector2d& Base, double z);
									ZEVector3d(const ZEVector3d& Start, const ZEVector3d& End);
									ZEVector3d();
};

ZEVector3d operator*(double S, const ZEVector3d& RightOperand);

// ZEVector4d
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZEVector4d
{
	ZE_FUNDAMENTAL
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
		bool						IsInfinite() const;
		bool						IsValidNotInf() const;
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
		bool						NotEquals(const ZEVector4d& Vector) const;
		bool						NotEquals(const ZEVector4d& Vector, double Threshold) const;

		void						FromVector4(const ZEVector4& Vector);
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

									ZE_VECTOR_SWIZZLE_4D_DEFINITION(ZEVector4d, ZEVector3d, ZEVector2d);

									ZEVector4d(double x, double y, double z, double w);
									ZEVector4d(const ZEVector2d& Base, double z, double w);
									ZEVector4d(const ZEVector3d& Base, double w);
									ZEVector4d(const ZEVector4d& Start, const ZEVector4d& End);
									ZEVector4d();
};

ZEVector4d operator*(double S, const ZEVector4d& RightOperand);
