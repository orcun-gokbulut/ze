//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFormat.cpp
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

#include "ZEFormat.h"

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMeta/ZEObject.h"

#include "ZEFormatOverloads.cpp"
/*
ZESSize ZEFormat::PrintVariant(char* Buffer, const ZEVariant& Argument, const char* ArgumentFormat)
{
	switch(Argument.GetType())
	{
		default:
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
			return -1;
			
		case ZE_VRT_FLOAT:
			return Print(Buffer, Argument.GetFloat(), ArgumentFormat);
			
		case ZE_VRT_DOUBLE:
			return Print(Buffer, Argument.GetDouble(), ArgumentFormat);

		case ZE_VRT_INTEGER_8:
			return Print(Buffer, Argument.GetInt8(), ArgumentFormat);

		case ZE_VRT_INTEGER_16:
			return Print(Buffer, Argument.GetInt16(), ArgumentFormat);

		case ZE_VRT_INTEGER_32:
			return Print(Buffer, Argument.GetInt32(), ArgumentFormat);

		case ZE_VRT_INTEGER_64:
			return Print(Buffer, Argument.GetInt64(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_8:
			return Print(Buffer, Argument.GetUInt8(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_16:
			return Print(Buffer, Argument.GetUInt16(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_32:
			return Print(Buffer, Argument.GetUInt32(), ArgumentFormat);

		case ZE_VRT_UNSIGNED_INTEGER_64:
			return Print(Buffer, Argument.GetUInt64(), ArgumentFormat);

		case ZE_VRT_BOOLEAN:
			return Print(Buffer, Argument.GetBoolean(), ArgumentFormat);

		case ZE_VRT_STRING:
			return Print(Buffer, Argument.GetString(), ArgumentFormat);

		case ZE_VRT_QUATERNION:
			return Print(Buffer, Argument.GetQuaternion(), ArgumentFormat);

		case ZE_VRT_VECTOR2:
			return Print(Buffer, Argument.GetVector2(), ArgumentFormat);

		case ZE_VRT_VECTOR3:
			return Print(Buffer, Argument.GetVector3(), ArgumentFormat);

		case ZE_VRT_VECTOR4:
			return Print(Buffer, Argument.GetVector4(), ArgumentFormat);

		case ZE_VRT_MATRIX3X3:
			return Print(Buffer, Argument.GetMatrix3x3(), ArgumentFormat);

		case ZE_VRT_MATRIX4X4:
			return Print(Buffer, Argument.GetMatrix4x4(), ArgumentFormat);

		case ZE_VRT_CLASS:
			return Print(Buffer, Argument.GetClass(), ArgumentFormat);
	}
}

#define ZE_FPS_IMMEDIATE
#define ZE_FPS_ARGUMENT_INDEX_START
#define ZE_FPS_ARGUMENT_INDEX
#define ZE_FPS_ARGUMENT_INDEX_END
#define ZE_FPS_ARGUMENT_FORMATTING_START
#define ZE_FPS_ARGUMENT_FORMATTING
#define ZE_FPS_ARGUMENT_FORMATTING_END

ZEString ZEFormat::Format(const char* Input, const ZEVariant* Arguments, ZESize Count)
{
	ZEString
	char* CurrentOutput;
	const char* CurrentInput = Input;
	int State = 0;
	ZESize ArgumentIndex = -1;
	
	char ArgumentIndexString[2];
	char ArgumentIndexStringIndex = 0;

	char ArgumentFormat[255];
	ZESize ArgumentFormatIndex = 0;

	while(*CurrentInput != '\0')
	{
		switch(State)
		{
			case ZE_FPS_IMMEDIATE: // Immediate Mode
				if (*CurrentInput == '{')
				{
					if (*(CurrentInput + 1) != '\0' && *(CurrentInput + 1) != '{')
					{
						*CurrentOutput == '{';
						CurrentOutput++;
						CurrentInput++;
					}
					State = 2;
				}
				else
					*CurrentOutput = *CurrentInput;

				CurrentInput++;
				break;

			case ZE_FPS_ARGUMENT_INDEX_START: // Argument Index Start
				if (*CurrentInput == ' ' || *CurrentInput == '\t')
					CurrentInput++;
				else if (isdigit(*CurrentInput))
					State = 2;
				else
					zeError("Formatting error. Argument index must be integer.");
				break;
			
			case ZE_FPS_ARGUMENT_INDEX: // Argument Index
				if (*CurrentInput == ' ' || *CurrentInput == '\t' || *CurrentInput == ':' || *CurrentInput == '}')
				{
					if (*CurrentInput == '}')

					ArgumentIndexString[ArgumentIndexStringIndex] = '\0';
					ArgumentIndex = atoi(ArgumentIndexString);
					ArgumentIndexStringIndex = 0;

					if (*CurrentInput == ':')
						State = 4;
				}
				else if (isdigit(*CurrentInput))
				{
					if (ArgumentIndex >= 3)
					{
						zeError("Formatting error. Argument index is too big.");
						return ZEString();
					}

					ArgumentIndexString[ArgumentIndex] = *CurrentInput;
					ArgumentIndex++;
					CurrentInput++;

				}
				else
				{
					zeError("Formatting Error. Argument index must be integer.");
					return ZEString();
				}
				break;

			case ZE_FPS_ARGUMENT_INDEX_END: // Argument Index End
				if (*CurrentInput == ' ' || *CurrentInput == '\t')
					CurrentInput++;
				else if (*CurrentInput == ':')
				{
					State = 4;
					CurrentInput++;
				}
				else
				{
					zeError("Formatting error. Expected ':' after argument index.");
					return ZEString();
				}
				break;

			case ZE_FPS_ARGUMENT_FORMATTING_START: // Formatting String Mode
				if (*CurrentInput == '}')
				{
					if (ArgumentIndex == -1)
					{
						zeError("Formatting error. Argument index is missing.");
						return ZEString();
					}

					if (ArgumentIndex >= Count)
					{
						zeError("Formatting error. Argument index is out of bounds.");
						return ZEString();
					}
					
					// Do Stuff
					ArgumentFormat[ArgumentFormatIndex] = '\0';
					
					ZESSize BytesWritten = PrintVariant(CurrentOutput, Arguments[ArgumentIndex], ArgumentFormat);
					if (BytesWritten == -1)
					{
						zeError("Formatting error. Argument format is invalid.");
						return ZEString();
					}

					ArgumentIndex = -1;
					ArgumentFormatIndex = 0;

					CurrentOutput += BytesWritten;
				}
				break;
			case ZE_FPS_ARGUMENT_FORMATTING
				break;

			case ZE_FPS_ARGUMENT_FORMATTING_END
				break;
		}

		if (State != 0)
		*CurrentOutput = '\0';

	}

}
// {parameter index : formatting string} {{ }}
*/
