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


#ifndef SERVERPRIVATE_ICE
#define SERVERPRIVATE_ICE

#include <Test.ice>

class SBSUnknownDerived extends SBase
{
    string sbsud;
};

class SUnknown
{
    string su;
};

class D2 extends B
{
    string sd2;
    B pd2;
};

class D4 extends B
{
    B p1;
    B p2;
};

exception UnknownDerivedException extends BaseException
{
    string sude;
    D2 pd2;
};

#endif
