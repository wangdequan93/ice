// **********************************************************************
//
// Copyright (c) 2003 - 2004
// ZeroC, Inc.
// North Palm Beach, FL, USA
//
// All Rights Reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************


#ifndef CALLBACK_ICE
#define CALLBACK_ICE

exception CallbackException
{
    double someValue;
    string someString;
};

class CallbackReceiver
{
    void callback();

    void callbackEx()
	throws CallbackException;
};

class Callback
{
    void initiateCallback(CallbackReceiver* proxy);

    void initiateCallbackEx(CallbackReceiver* proxy)
	throws CallbackException;

    void shutdown();
};

#endif
