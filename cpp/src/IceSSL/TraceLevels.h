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


#ifndef ICE_SSL_TRACE_LEVELS_H
#define ICE_SSL_TRACE_LEVELS_H

#include <IceUtil/Shared.h>
#include <Ice/ProtocolPluginFacadeF.h>
#include <IceSSL/TraceLevelsF.h>

namespace IceSSL
{

class TraceLevels : public ::IceUtil::Shared
{
public:

    TraceLevels(const IceInternal::ProtocolPluginFacadePtr&);
    virtual ~TraceLevels();

    const int network;
    const char* networkCat;

    const int security;
    const char* securityCat;
};

}

#endif
