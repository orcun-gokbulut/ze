//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Quaternion.h
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

#define MapQuaternion(A, B)	(A).x = (B).x; (A).y = (B).y; (A).z = (B).z; (A).w = (B).w 
#define MapQuarternionToWXYZ(Q, _w, _x, _y, _z) (Q).w = (_w); (Q).x = (_x); (Q).y = (_y); (Q).z = (_z)

class ZEVector3;
class ZEMatrix3x3;
class ZEMatrix4x4;

class ZEQuaternion
{
	public:
		static const ZEQuaternion		Zero;
		static const ZEQuaternion		Identity;

		float							w, x, y, z;

		static void						Create(ZEQuaternion& Output, float w, float x, float y, float z);
		static void						Create(ZEQuaternion& Output, float Angle, const ZEVector3& Axis);
		static void						Create(ZEQuaternion& Output, float Pitch, float Yawn, float Roll);
		static void						Create(ZEQuaternion& Output, const ZEVector3& Direction, const ZEVector3& Right);
		static void						CreateIdentity(ZEQuaternion& Output);

		static void						Product(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B);
		static void						Transform(ZEVector3& Output, const ZEVector3& Vector, const ZEQuaternion& Quaternion);

		void							Conjugate();
		static void						Conjugate(ZEQuaternion& Output, const ZEQuaternion& Quaternion);

		static void						Slerp(ZEQuaternion& Output, const ZEQuaternion& A, const ZEQuaternion& B, float Factor);

		float							Length() const;
		static float					Length(ZEQuaternion& Quaternion);

		void							Normalize();
		static void						Normalize(ZEQuaternion& Output, const ZEQuaternion& Quaternion);

		static void						ConvertToRotationMatrix(ZEMatrix4x4& Output, const ZEQuaternion& Quaternion);
		static void						ConvertToRotationMatrix(ZEMatrix3x3& Output, const ZEQuaternion& Quaternion);
		static void						ConvertToEulerAngles(float &Yaw, float &Pitch, float &Roll, const ZEQuaternion& Quaternion);
		static void						ConvertToLookAndUp(ZEVector3& Look, ZEVector3& Up, const ZEQuaternion& Quaternion);

		ZEQuaternion					operator*(const ZEQuaternion& Other) const;
		ZEQuaternion&					operator*=(const ZEQuaternion& Other);

										ZEQuaternion(float w, float x, float y, float z);
										ZEQuaternion(float Angle, const ZEVector3& Axis);
										ZEQuaternion();
};


#endif
