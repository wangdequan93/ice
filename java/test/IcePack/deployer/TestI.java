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


public class TestI extends _TestDisp
{
    public
    TestI(Ice.ObjectAdapter adapter, Ice.Properties properties)
    {
        _adapter = adapter;
	_properties = properties;
    }

    public void
    shutdown(Ice.Current current)
    {
        _adapter.getCommunicator().shutdown();
    }

    public String
    getProperty(String name, Ice.Current current)
    {
	return _properties.getProperty(name);
    }

    private Ice.ObjectAdapter _adapter;
    private Ice.Properties _properties;
}
