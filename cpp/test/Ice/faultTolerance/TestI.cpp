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


#include <Ice/Ice.h>
#include <TestI.h>

TestI::TestI(const Ice::ObjectAdapterPtr& adapter) :
    _adapter(adapter)
{
}

void
TestI::shutdown(const Ice::Current&)
{
    _adapter->getCommunicator()->shutdown();
}

void
TestI::abort(const Ice::Current&)
{
    exit(0);
}

void
TestI::idempotentAbort(const Ice::Current&)
{
    exit(0);
}

void
TestI::nonmutatingAbort(const Ice::Current&) const
{
    exit(0);
}

Ice::Int
TestI::pid(const Ice::Current&)
{
#ifdef _WIN32
    return _getpid();
#else
    return getpid();
#endif
}
