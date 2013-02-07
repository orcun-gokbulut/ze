//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - qmfcapp.cpp
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

/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

// Implementation of the QMfcApp classes

#ifdef QT3_SUPPORT
#undef QT3_SUPPORT
#endif

#ifdef UNICODE
#undef UNICODE
#endif

#include "qmfcapp.h"

#include <QtCore/QEventLoop>
#include <QtCore/QAbstractEventDispatcher>
#include <QtGui/QWidget>

#ifdef QTWINMIGRATE_WITHMFC
#include <afxwin.h>
#else
#include <QtCore/qt_windows.h>
#endif

#ifdef QTWINMIGRATE_WITHMFC
CWinApp *QMfcApp::mfc_app = 0;
char **QMfcApp::mfc_argv = 0;
int QMfcApp::mfc_argc = 0;
#endif

/*! \class QMfcApp qmfcapp.h
    \brief The QMfcApp class provides merging of the MFC and Qt event loops.

    QMfcApp is responsible for driving both the Qt and MFC event loop.
    It replaces the standard MFC event loop provided by
    CWinApp::Run(), and is used instead of the QApplication parent
    class.

    To replace the MFC event loop reimplement the CWinApp::Run()
    function in the CWinApp subclass usually created by the MFC
    Application Wizard, and use either the static run() function, or
    an instance of QMfcApp created earlier through the static
    instance() function or the constructor.

    The QMfcApp class also provides a static API pluginInstance() that
    drives the Qt event loop when loaded into an MFC or Win32 application.
    This is useful for developing Qt based DLLs or plugins, or if the
    MFC application's event handling can not be modified.
*/

static int modalLoopCount = 0;

HHOOK hhook;
LRESULT CALLBACK QtFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (qApp) {
        // don't process deferred-deletes while in a modal loop
        if (modalLoopCount)
            qApp->sendPostedEvents();
        else
            qApp->sendPostedEvents(0, -1);
    }

    return CallNextHookEx(hhook, nCode, wParam, lParam);
}

/*!
    Inform Qt that a modal loop is about to be entered, and that DeferredDelete
    events should not be processed. Call this function before calling Win32
    or MFC functions that enter a modal event loop (i.e. MessageBox).

    This is only required if the Qt UI code hooks into an existing Win32
    event loop using QMfcApp::pluginInstance.

    \sa exitModalLoop()
*/
void QMfcApp::enterModalLoop()
{
    ++modalLoopCount;
}

/*!
    Inform Qt that a modal loop has been exited, and that DeferredDelete
    events should not be processed. Call this function after the blocking
    Win32 or MFC function (i.e. MessageBox) returned.

    This is only required if the Qt UI code hooks into an existing Win32
    event loop using QMfcApp::pluginInstance.

    \sa enterModalLoop()
*/
void QMfcApp::exitModalLoop()
{
    --modalLoopCount;
    Q_ASSERT(modalLoopCount >= 0);
}

/*!
    If there is no global QApplication object (i.e. qApp is null) this
    function creates a QApplication instance and returns true;
    otherwise it does nothing and returns false.

    The application installs an event filter that drives the Qt event
    loop while the MFC or Win32 application continues to own the event
    loop.

    Use this static function if the application event loop code can not be
    easily modified, or when developing a plugin or DLL that will be loaded
    into an existing Win32 or MFC application. If \a plugin is non-null then
    the function loads the respective DLL explicitly to avoid unloading from
    memory.

    \code
    BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved)
    {
	if (dwReason == DLL_PROCESS_ATTACH)
	    QMfcApp::pluginInstance(hInstance);

	return TRUE;
    }
    \endcode

    Set \a plugin to 0 when calling this function from within the same executable
    module.

    If this function is used, call enterModalLoop and exitModalLoop whenever you
    call a Win32 or MFC function that opens a local event loop.

    \code
    void Dialog::someSlot()
    {
        QMfcApp::enterModalLoop();
        MessageBox(...);
        QMfcApp::exitModalLoop();
    }
    \endcode
*/
bool QMfcApp::pluginInstance(Qt::HANDLE plugin)
{
    if (qApp)
	return FALSE;

    QT_WA({
	hhook = SetWindowsHookExW(WH_GETMESSAGE, QtFilterProc, 0, GetCurrentThreadId());
    }, {
	hhook = SetWindowsHookExA(WH_GETMESSAGE, QtFilterProc, 0, GetCurrentThreadId());
    });

    int argc = 0;
    (void)new QApplication(argc, 0);

    if (plugin) {
	char filename[256];
	if (GetModuleFileNameA((HINSTANCE)plugin, filename, 255))
	    LoadLibraryA(filename);
    }

    return TRUE;
}

#ifdef QTWINMIGRATE_WITHMFC
/*!
    Runs the event loop for both Qt and the MFC application object \a
    mfcApp, and returns the result. This function calls \c instance()
    if no QApplication object exists and deletes the object it
    created.

    Calling this static function in a reimplementation of
    CWinApp::Run() is the simpliest way to use the QMfcApp class:

    \code
    int MyMfcApp::Run()
    {
        return QMfcApp::run(this);
    }
    \endcode

    Since a QApplication object must exist before Qt widgets can be
    created you cannot use this function if you want to use Qt-based
    user interface elements in, for example, the InitInstance()
    function of CWinApp. In such cases, create an instance of
    QApplication explicitly using instance() or the constructor.

    \sa instance()
*/
int QMfcApp::run(CWinApp *mfcApp)
{
    bool ownInstance = !qApp;
    if (ownInstance)
	instance(mfcApp);
    int result = qApp->exec();

    if (mfcApp) {
        int mfcRes = mfcApp->ExitInstance();
        if (mfcRes && !result)
            result = mfcRes;
    }

    if (ownInstance)
	delete qApp;

    return result;
}

/*!
    Creates an instance of QApplication, passing the command line of
    \a mfcApp to the QApplication constructor, and returns the new
    object. The returned object must be destroyed by the caller.

    Use this static function if you want to perform additional
    initializations after creating the application object, or if you
    want to create Qt GUI elements in the InitInstance()
    reimplementation of CWinApp:

    \code
    BOOL MyMfcApp::InitInstance()
    {
	// standard MFC initialization
	// ...

	// This sets the global qApp pointer
	QMfcApp::instance(this);

	// Qt GUI initialization
    }

    BOOL MyMfcApp::Run()
    {
	int result = QMfcApp::run(this);
	delete qApp;
	return result;
    }
    \endcode

    \sa run()
*/
QApplication *QMfcApp::instance(CWinApp *mfcApp)
{
    mfc_app = mfcApp;
    if (mfc_app) {
#if defined(UNICODE)
	QString exeName((QChar*)mfc_app->m_pszExeName, wcslen(mfc_app->m_pszExeName));
	QString cmdLine((QChar*)mfc_app->m_lpCmdLine, wcslen(mfc_app->m_lpCmdLine));
#else
        QString exeName = QString::fromLocal8Bit(mfc_app->m_pszExeName);
	QString cmdLine = QString::fromLocal8Bit(mfc_app->m_lpCmdLine);
#endif
	QStringList arglist = QString(exeName + " " + cmdLine).split(' ');

	mfc_argc = arglist.count();
	mfc_argv = new char*[mfc_argc+1];
	int a;
	for (a = 0; a < mfc_argc; ++a) {
	    QString arg = arglist[a];
	    mfc_argv[a] = new char[arg.length()+1];
	    qstrcpy(mfc_argv[a], arg.toLocal8Bit().data());
	}
	mfc_argv[a] = 0;
    }

    return new QMfcApp(mfcApp, mfc_argc, mfc_argv);
}


static bool qmfc_eventFilter(void *message)
{
    long result = 0;
    return static_cast<QMfcApp*>(qApp)->winEventFilter((MSG*)message, &result);
}

/*!
    Creates an instance of QMfcApp. \a mfcApp must point to the
    existing instance of CWinApp. \a argc and \a argv are passed on
    to the QApplication constructor.

    Use the static function instance() to automatically use the
    command line passed to the CWinApp.

    \code
    QMfcApp *qtApp;

    BOOL MyMfcApp::InitInstance()
    {
	// standard MFC initialization

	int argc = ...
	char **argv = ...

	qtApp = new QMfcApp(this, argc, argv);

	// Qt GUI initialization
    }

    BOOL MyMfcApp::Run()
    {
	int result = qtApp->exec();
	delete qtApp;
	qtApp = 0;

	return result;
    }
    \endcode

    \sa instance() run()
*/
QMfcApp::QMfcApp(CWinApp *mfcApp, int &argc, char **argv)
: QApplication(argc, argv), idleCount(0), doIdle(FALSE)
{
    mfc_app = mfcApp;
    QAbstractEventDispatcher::instance()->setEventFilter(qmfc_eventFilter);
    setQuitOnLastWindowClosed(false);
}
#endif

/*!
    Destroys the QMfcApp object, freeing all allocated resources.
*/
QMfcApp::~QMfcApp()
{
    if (hhook) {
	UnhookWindowsHookEx(hhook);
	hhook = 0;
    }

#ifdef QTWINMIGRATE_WITHMFC
    for (int a = 0; a < mfc_argc; ++a) {
	char *arg = mfc_argv[a];
	delete[] arg;
    }
    delete []mfc_argv;

    mfc_argc = 0;
    mfc_argv = 0;
    mfc_app = 0;
#endif
}

/*!
    \reimp
*/
bool QMfcApp::winEventFilter(MSG *msg, long *result)
{
    static bool recursion = false;
    if (recursion)
        return false;

    recursion = true;

    QWidget *widget = QWidget::find(msg->hwnd);
    HWND toplevel = 0;
    if (widget) {
        HWND parent = widget->winId();
        while(parent) {
            toplevel = parent;
            parent = GetParent(parent);
        }
        HMENU menu = toplevel ? GetMenu(toplevel) : 0;
        if (menu && GetFocus() == msg->hwnd) {
            if (msg->message == WM_SYSKEYUP && msg->wParam == VK_MENU) {
                // activate menubar on Alt-up and move focus away
                SetFocus(toplevel);
                SendMessage(toplevel, msg->message, msg->wParam, msg->lParam);
                widget->setFocus();
                recursion = false;
                return TRUE;
            } else if (msg->message == WM_SYSKEYDOWN && msg->wParam != VK_MENU) {
                SendMessage(toplevel, msg->message, msg->wParam, msg->lParam);
                SendMessage(toplevel, WM_SYSKEYUP, VK_MENU, msg->lParam);
                recursion = false;
                return TRUE;
            }
        }
    }
#ifdef QTWINMIGRATE_WITHMFC
    else if (mfc_app) {
        MSG tmp;
        while (doIdle && !PeekMessage(&tmp, 0, 0, 0, PM_NOREMOVE)) {
            if (!mfc_app->OnIdle(idleCount++))
                doIdle = FALSE;
        }
        if (mfc_app->IsIdleMessage(msg)) {
            doIdle = TRUE;
            idleCount = 0;
        }
    }
    if (mfc_app && mfc_app->PreTranslateMessage(msg)) {
        recursion = false;
	return TRUE;
    }
#endif

    recursion = false;
    return QApplication::winEventFilter(msg, result);
}