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


#ifndef HELLO_I_H
#define HELLO_I_H

#include <Hello.h>

class HelloI : public Hello
{
public:

    virtual void sayHello(const Ice::Current&) const;
    virtual void shutdown(const Ice::Current&);
};

#endif
