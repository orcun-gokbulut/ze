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

#include "ZEdefinitions.h"
#include "ZEvector.h"
#include "ZEmatrix.h"

class ZEQuaternion
{
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
		static void						Create(ZEQuaternion& Output, float Angle, const ZEVector3& Axis);
		static void						CreateIdentity(ZEQuaternion& Output);
		
		static void						CreateFromEuler(ZEQuaternion& Output, float Pitch, float Yawn, float Roll);
		static void						CreateFromMatrix(ZEQuaternion& Output, const ZEMatrix4x4& Matrix);
		static void						CreateFromDirection(ZEQuaternion& Output, const ZEVector3& Direction, const ZEVector3& Up = ZEVector3(0.0f, 1.0f, 0.0));
		static void						Product(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B);
		static void						VectorProduct(ZEVector3& Output, const ZEQuaternion& Quaternion, const ZEVector3& Vector);

		ZEQuaternion					Conjugate() const;
		static void						Conjugate(ZEQuaternion& Output, const ZEQuaternion& Quaternion);

		static void						Slerp(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B, float Factor);

		ZEQuaternion					Normalize() const;
		static void						Normalize(ZEQuaternion& Output, const ZEQuaternion& Quaternion);

		static void						ConvertToRotationMatrix(ZEMatrix4x4& Output, const ZEQuaternion& Quaternion);
		static void						ConvertToEulerAngles(float &Yaw, float &Pitch, float &Roll, const ZEQuaternion& Quaternion);
		static void						ConvertToLookAndUp(ZEVector3& Look, ZEVector3& Up, const ZEQuaternion& Quaternion);

		ZEVector3						operator*(const ZEVector3& Vector) const;
		ZEQuaternion					operator*(const ZEQuaternion& Other) const;
		ZEQuaternion&					operator*=(const ZEQuaternion& Other);

		bool							operator==(const ZEQuaternion& RightOperand) const;
		bool							operator!=(const ZEQuaternion& RightOperand) const;

		float							operator[](size_t Index) const;
		float&							operator[](size_t Index);

										ZEQuaternion(float w, float x, float y, float z);
										ZEQuaternion(float Angle, const ZEVector3& Axis);
										ZEQuaternion();
};


#endif




