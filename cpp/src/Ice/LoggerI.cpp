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


#include <IceUtil/Time.h>
#include <Ice/LoggerI.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

IceUtil::Mutex Ice::LoggerI::_globalMutex;

Ice::LoggerI::LoggerI(const string& prefix, bool timestamp) : 
    _timestamp(timestamp)
{
    if(!prefix.empty())
    {
	_prefix = prefix + ": ";
    }
}

void
Ice::LoggerI::trace(const string& category, const string& message)
{
    IceUtil::Mutex::Lock sync(_globalMutex);

    string s = "[ ";
    if(_timestamp)
    {
	s += IceUtil::Time::now().toString() + " ";
    }
    s += _prefix + category + ": " + message + " ]";

    string::size_type idx = 0;
    while((idx = s.find("\n", idx)) != string::npos)
    {
	s.insert(idx + 1, "  ");
	++idx;
    }
    cerr << s << endl;
}

void
Ice::LoggerI::warning(const string& message)
{
    IceUtil::Mutex::Lock sync(_globalMutex);
    if(_timestamp)
    {
	cerr << IceUtil::Time::now().toString() << " ";
    }
    cerr << _prefix << "warning: " << message << endl;
}

void
Ice::LoggerI::error(const string& message)
{
    IceUtil::Mutex::Lock sync(_globalMutex);
    if(_timestamp)
    {
	cerr << IceUtil::Time::now().toString() << " ";
    }
    cerr << _prefix << "error: " << message << endl;
}
