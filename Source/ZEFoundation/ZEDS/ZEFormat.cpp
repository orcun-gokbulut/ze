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
#include "ZEFormatArgument.h"
#include "ZEStringWriter.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEError.h"
//#include "ZEMeta/ZEObject.h"

#define ZE_FPS_IMMEDIATE					0
#define ZE_FPS_ARGUMENT_INDEX_START			1
#define ZE_FPS_ARGUMENT_INDEX				2
#define ZE_FPS_ARGUMENT_INDEX_END			3
#define ZE_FPS_ARGUMENT_FORMATTING			4
#define ZE_FPS_APPLY						5

ZEString ZEFormat::FormatInner(const char* Input, const ZEValue** Arguments, ZESize Count)
{
	ZEStringWriter Writer;

	int State = ZE_FPS_IMMEDIATE;

	const char* CurrentInput = Input;
	ZESize ArgumentIndex = -1;
	
	char ArgumentIndexString[4];
	char ArgumentIndexStringIndex = 0;

	char ArgumentFormat[255];
	ZESize ArgumentFormatIndex = 0;

	while(*CurrentInput != '\0')
	{
		switch(State)
		{
			case ZE_FPS_IMMEDIATE:
				if (*CurrentInput == '{')
				{
					if (*(CurrentInput + 1) != '\0' && *(CurrentInput + 1) == '{')
					{
						Writer.Append('{');
						CurrentInput++;
					}
					else
						State = ZE_FPS_ARGUMENT_INDEX_START;
				}
				else if (*CurrentInput == '}')
				{
					if (*(CurrentInput + 1) != '\0' && *(CurrentInput + 1) == '}')
					{
						Writer.Append('}');
						CurrentInput++;
					}
					else
					{
						zeError("Formatting error. Mismatch '}' character.");
						return "!FORMATING_ERROR!";
					}
				}
				else
				{
					Writer.Append(*CurrentInput);
				}

				CurrentInput++;
				break;

			case ZE_FPS_ARGUMENT_INDEX_START:
				if (*CurrentInput == ' ' || *CurrentInput == '\t')
					CurrentInput++;
				else if (isdigit(*CurrentInput))
					State = ZE_FPS_ARGUMENT_INDEX;
				else
				{
					zeError("Formatting error. Argument index must be integer.");
					return "!FORMATING_ERROR!";
				}
				break;
			
			case ZE_FPS_ARGUMENT_INDEX:
				if (*CurrentInput == ' ' || *CurrentInput == '\t' || *CurrentInput == ':' || *CurrentInput == '}')
				{
					ArgumentIndexString[ArgumentIndexStringIndex] = '\0';
					ArgumentIndex = atoi(ArgumentIndexString);
					ArgumentIndexStringIndex = 0;

					if (ArgumentIndex >= Count)
					{
						zeError("Formatting error. Argument index is out of bounds.");
						return "!FORMATING_ERROR!";
					}

					if (*CurrentInput == ':')
						State = ZE_FPS_ARGUMENT_FORMATTING;
					else if (*CurrentInput == '}')
						State = ZE_FPS_APPLY;
					else 
						State = ZE_FPS_ARGUMENT_INDEX_END;

				}
				else if (isdigit(*CurrentInput))
				{
					if (ArgumentIndexStringIndex >= 3)
					{
						zeError("Formatting error. Argument index is too big.");
						return false;
					}

					ArgumentIndexString[ArgumentIndexStringIndex] = *CurrentInput;
					ArgumentIndexStringIndex++;
				}
				else
				{
					zeError("Formatting Error. Argument index must be integer.");
					return "!FORMATING_ERROR!";
				}
				CurrentInput++;
				break;

			case ZE_FPS_ARGUMENT_INDEX_END: // Argument Index End
				if (*CurrentInput == ' ' || *CurrentInput == '\t')
				{

				}
				else if (*CurrentInput == '}')
				{
					State = ZE_FPS_APPLY;
				}
				else if (*CurrentInput == ':')
				{
					State = ZE_FPS_ARGUMENT_FORMATTING;
					CurrentInput++;
				}
				else
				{
					zeError("Formatting error. Expected ':' or '}' after argument index.");
					return "!FORMATING_ERROR!";
				}

				CurrentInput++;
				break;

			case ZE_FPS_ARGUMENT_FORMATTING:
				if (*CurrentInput == '}')
				{
					State = ZE_FPS_APPLY;
				}
				else
				{
					ArgumentFormat[ArgumentFormatIndex] = *CurrentInput;
					ArgumentFormatIndex++;
				}

				CurrentInput++;
				break;
		}

		if (State == ZE_FPS_APPLY)
		{
			ArgumentFormat[ArgumentFormatIndex] = '\0';

			ZEString PrintOutput;
			if (!ZEFormatArgument::FormatVariant(Writer, *Arguments[ArgumentIndex], ArgumentFormat))
				Writer.Append("!FORMATING_ARGUMENT_ERROR!");

			ArgumentIndex = -1;
			ArgumentFormatIndex = 0;

			State = ZE_FPS_IMMEDIATE;
		}
	}

	if (State != 0)
	{
		zeError("Formatting error. There are non-closed argument available in the format.");
		return "!FORMATING_ERROR!";
	}

	ZEString Output;
	Writer.AppendEnd();
	Writer.Output(Output);

	return Output;
}
