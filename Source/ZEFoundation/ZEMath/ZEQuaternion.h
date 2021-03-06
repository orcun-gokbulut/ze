//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEQuaternion.h
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
#ifndef	__ZE_MATH_QUATERNION_H__
#define __ZE_MATH_QUATERNION_H__

#include "ZETypes.h"
#include "ZEVector.h"
#include "ZEMatrix.h"
#include "ZEMeta\ZEMTFundamental.h"
#include "ZEVectord.h"

class ZEQuaternion
{
	ZE_FUNDAMENTAL

	public:
		static const ZEQuaternion		Zero;
		static const ZEQuaternion		Identity;

		union
		{
			struct
			{
				float	w, x, y, z;
			};
			float		M[4];
		};

		static void						Create(ZEQuaternion& Output, float w, float x, float y, float z);
		static void						CreateIdentity(ZEQuaternion& Output);
		
		static void						CreateFromEuler(ZEQuaternion& Output, float x, float y, float z);
		static void						CreateFromEuler(ZEQuaternion& Output, const ZEVector3& Rotation);
		static void						CreateFromMatrix(ZEQuaternion& Output, const ZEMatrix3x3& Matrix);
		static void						CreateFromMatrix(ZEQuaternion& Output, const ZEMatrix4x4& Matrix);
		static void						CreateFromDirection(ZEQuaternion& Output, const ZEVector3& Direction, const ZEVector3& Up = ZEVector3(0.0f, 1.0f, 0.0f));
		static void						CreateFromDirectionChecked(ZEQuaternion& Output, const ZEVector3& Direction, const ZEVector3& Up = ZEVector3(0.0f, 1.0f, 0.0f));
		static void						CreateFromAngleAxis(ZEQuaternion& Output, float Angle, const ZEVector3& Axis);

		static void						Product(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B);
		static void						VectorProduct(ZEVector3& Output, const ZEQuaternion& Quaternion, const ZEVector3& Vector);
		static void						VectorProduct(ZEVector3d& Output, const ZEQuaternion& Quaternion, const ZEVector3d& Vector);

		static void						Slerp(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B, float Factor);
		static void						Nlerp(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B, float Factor);
		static void						Conjugate(ZEQuaternion& Output, const ZEQuaternion& Quaternion);
		static void						Normalize(ZEQuaternion& Output, const ZEQuaternion& Quaternion);
		static float					Length(const ZEQuaternion& Quaternion);
		static float					LengthSquare(const ZEQuaternion& Quaternion);
		static void						ScaleRotation(ZEQuaternion& Output, const ZEQuaternion& Input, float Scale);

		static void						ConvertToEulerAngles(float& x, float& y, float& z, const ZEQuaternion& Quaternion);
		static void						ConvertToEulerAngles(ZEVector3& Rotation, const ZEQuaternion& Quaternion);
		static void						ConvertToLookAndUp(ZEVector3& Look, ZEVector3& Up, const ZEQuaternion& Quaternion);
		static void						ConvertToAngleAxis(float& Angle, ZEVector3& Axis, const ZEQuaternion& Quaternion);

		bool							IsValid() const;
		bool							IsNormalized() const;

		float							Length() const;
		float							LengthSquare() const;

		float							RotationAngle() const;
		ZEVector3						RotationAxis() const;

		ZEQuaternion					Conjugate() const;
		ZEQuaternion					Normalize() const;

		void							ConjugateSelf();
		void							NormalizeSelf();

		bool							Equals(const ZEQuaternion& Quaternion) const;
		bool							Equals(const ZEQuaternion& Quaternion, float Threshold) const;

		ZEVector3						operator*(const ZEVector3& Vector) const;
		ZEVector3d						operator*(const ZEVector3d& Vector) const;
		ZEQuaternion					operator*(const ZEQuaternion& Other) const;
		ZEQuaternion&					operator*=(const ZEQuaternion& Other);

		bool							operator==(const ZEQuaternion& RightOperand) const;
		bool							operator!=(const ZEQuaternion& RightOperand) const;

		float							operator[](ZESize Index) const;
		float&							operator[](ZESize Index);

										ZEQuaternion(float w, float x, float y, float z);
										ZEQuaternion(float Angle, const ZEVector3& Axis);
										ZEQuaternion();
};


#endif
