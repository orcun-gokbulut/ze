//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFlags.h
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 YiÄŸit OrÃ§un GÃ–KBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "YiÄŸit  OrÃ§un  GÃ–KBULUT") and
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
  Name: YiÄŸit OrÃ§un GÃ–KBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#ifndef __ZE_FLAGS_H__
#define __ZE_FLAGS_H__

template <typename Type>
class ZEFlags
{
	public:
		Type Value;

		inline void RaiseBit(size_t Index)
		{
			Value |= (0x1 << Index);
		}

		inline void UnraiseBit(size_t Index)
		{
			Value &= ~(0x1 << Index);
		}

		inline void SetBit(size_t Index, bool Raise)
		{
			if (Raise)
				RaiseBit(Index);
			else
				UnraiseBit(Index);
		}

		inline bool GetBit(size_t Index)
		{
			return (Value & (0x1 << Index)) != 0;
		}

		inline void SetFlags(int Flags, bool Raise)
		{
			if (Raise)
				RaiseFlags(Flags);
			else
				UnraiseFlags(Flags);
		}

		inline void RaiseFlags(int Flags)
		{
			Value |= Value;
		}

		inline void UnraiseFlags(Type Flags)
		{
			Value &= ~Flags;

		}

		inline void GetFlags(int Flags)
		{
			return (Value & Flags) == Flags;
		}

		inline bool operator[](size_t Index) const
		{
			return GetBit(Index);
		}

		inline operator Type() const
		{
			return Value;
		}

		inline const Type& operator=(const Type& Value)
		{
			this->Value = Value;
			return *this;
		}

		inline ZEFlags()
		{
			Value = 0;
		}

		inline ZEFlags(const Type& Value)
		{
			this->Value = Value;
		}

		inline ZEFlags(const ZEFlags& Value)
		{
			this->Value = Value;
		}
};

#endif
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  ZEFlags.h - Zinek Engine v0.5.3 Source Code                                 
// -------------------------------------------------------------------------------- //
//  Copyright (c) 2007-2010 Y. Orçun GÖKBULUT. All rights reserved.                 //
//                                                                                  //
//                 READ TERMS BELLOW BEFORE TAKING ANY ACTION !                     //
//                                                                                  //
//  These coded instructions, statements, and computer programs contain             //
//  unpublished proprietary information belongs to Y. Orçun GÖKBULUT and they are   //
//  protected by international copyright laws. This file is intellectual property   //
//  of the Y. Orçun GÖKBULUT. Storing this file, using this file, changing content  //
//  of this file, copying or duplicating this file, compiling this file, publishing // 
//  this file to anykind media or network, editing  this file, selling this file,   //
//  booking this file, reading this file are probited without notice and written    //
//  permision of Y. Orçun GÖKBULUT.                                                 //
//  These terms at above can only be changed by Y. Orçun GÖKBULUT.                  //
//                                                                                  //
//  Contact Information:                                                            //
//     Adress : Zinek Code House & Game Studio                                      //                                             
//              Aydinlar Mahallesi.                                                 //
//              Mimar Sokak. 33/5                                                   //
//              06450  Ankara/TURKEY                                                //
//     Phone  : +90 (533) 734 21 22                                                 //
//     E-Mail : contact@zinekengine.com                                             //
//     WWW    : http://www.zinekengine.com                                          //
//                                                                                  //
//*                                                                                *//
//  DESCRIPTION         :                                                           //
//  AUTHOR(S)           : Y. Orçun GÖKBULUT                                         //
//*                                                                                *//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __ZE_FLAGS_H__
#define __ZE_FLAGS_H__

template <typename Type>
class ZEFlags
{
	public:
		Type Value;

		inline void RaiseBit(size_t Index)
		{
			Value |= (0x1 << Index);
		}

		inline void UnraiseBit(size_t Index)
		{
			Value &= ~(0x1 << Index);
		}

		inline void SetBit(size_t Index, bool Raise)
		{
			if (Raise)
				RaiseBit(Index);
			else
				UnraiseBit(Index);
		}

		inline bool GetBit(size_t Index)
		{
			return (Value & (0x1 << Index)) != 0;
		}

		inline void SetFlags(int Flags, bool Raise)
		{
			if (Raise)
				RaiseFlags(Flags);
			else
				UnraiseFlags(Flags);
		}

		inline void RaiseFlags(int Flags)
		{
			Value |= Value;
		}

		inline void UnraiseFlags(Type Flags)
		{
			Value &= ~Flags;

		}

		inline void GetFlags(int Flags)
		{
			return (Value & Flags) == Flags;
		}

		inline bool operator[](size_t Index) const
		{
			return GetBit(Index);
		}

		inline operator Type() const
		{
			return Value;
		}

		inline const Type& operator=(const Type& Value)
		{
			this->Value = Value;
			return *this;
		}

		inline ZEFlags()
		{
			Value = 0;
		}

		inline ZEFlags(const Type& Value)
		{
			this->Value = Value;
		}

		inline ZEFlags(const ZEFlags& Value)
		{
			this->Value = Value;
		}
};

#endif
