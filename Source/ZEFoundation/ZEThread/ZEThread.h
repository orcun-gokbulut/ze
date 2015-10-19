//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEThread.h
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

#ifndef __ZE_THREAD_H__
#define __ZE_THREAD_H__

#include "ZEDS/ZEDelegate.h"
#include "ZEDS/ZEString.h"

#ifdef ZE_PLATFORM_UNIX
#include <pthread.h>
#endif

enum ZEThreadStatus
{
    ZE_TS_NONE          = 1,
	ZE_TS_STARING		= 2,
    ZE_TS_RUNNING       = 4,
    ZE_TS_WAITING       = 8,
    ZE_TS_TERMINATED    = 16,
    ZE_TS_EXITING       = 32,
    ZE_TS_DONE          = 64
};
#define MASK(Value)(Value)
#define ZE_TS_ALIVE_MASK    MASK(ZE_TS_STARING | ZE_TS_RUNNING | ZE_TS_WAITING | ZE_TS_EXITING)
#define ZE_TS_DEAD_MASK     MASK(ZE_TS_NONE | ZE_TS_TERMINATED | ZE_TS_DONE)

class ZEThread;

typedef ZEDelegate<void (ZEThread*, void*)> ZEThreadFunction;

class ZEThread
{
    private:
        ZEString            Name;
        #ifdef ZE_PLATFORM_WINDOWS
            static unsigned long __stdcall
                            ThreadFunction(void* Thread);
            void*			Handle;
        #elif defined(ZE_PLATFORM_UNIX)
            static void*    ThreadFunction(void* Thread);
            pthread_t       Thread;
        #endif

		ZEThreadStatus		Status;
        ZEThreadFunction    Function;
        void*				Parameter;

	public:
        void                SetName(const ZEString& Name);
        const ZEString&     GetName();

		ZEThreadStatus		GetStatus();
        bool                IsAlive();

        void                SetFunction(ZEThreadFunction Function);
        ZEThreadFunction&   GetFunction();

		void				SetParameter(void* Parameter);
		void*				GetParameter();

		void				Run(void* Parameter);
		void				Terminate();
        void                Exit();

        // Threads Function
        bool                ControlPoint();

		void				Sleep(ZEUInt Milliseconds);
        void                Wait();
        bool                Wait(ZEUInt Milliseconds);

                            ZEThread();
		virtual 			~ZEThread();

        static ZEThread*    GetCurrentThread();
};

#endif
