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


#ifndef ICE_LOGGER_I_H
#define ICE_LOGGER_I_H

#include <IceUtil/Mutex.h>
#include <Ice/Logger.h>

namespace Ice
{

class LoggerI : public Logger
{
public:
    LoggerI(const std::string&, bool);

    virtual void trace(const std::string&, const std::string&);
    virtual void warning(const std::string&);
    virtual void error(const std::string&);

private:

    std::string _prefix;
    std::string _emptyPrefix;
    bool _timestamp;

    //
    // A global mutex is used to avoid garbled output with multiple
    // communicators.
    //
    static IceUtil::Mutex _globalMutex;
};

}

#endif
