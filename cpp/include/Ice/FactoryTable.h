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


#ifndef ICE_FACTORYTABLE_H
#define ICE_FACTORYTABLE_H

#include <Ice/FactoryTableDef.h>

namespace Ice
{

class ICE_API FactoryTable
{
public:

    FactoryTable();
    ~FactoryTable();
};

static FactoryTable factoryTableInitializer;	// Dummy variable to force initialization of factoryTable

extern ICE_API FactoryTableDef* factoryTable;

}

#endif
