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


package IceInternal;

public final class RouterInfo
{
    RouterInfo(Ice.RouterPrx router)
    {
        _router = router;
        _routingTable = new Ice.RoutingTable();

        assert(_router != null);
    }

    synchronized public void
    destroy()
    {
	_clientProxy = null;
	_serverProxy = null;
	_adapter = null;
	_routingTable.clear();
    }

    public boolean
    equals(java.lang.Object obj)
    {
        RouterInfo rhs = null;
        try
        {
            rhs = (RouterInfo)obj;
        }
        catch(ClassCastException ex)
        {
            return false;
        }

        return _router.equals(rhs._router);
    }

    public Ice.RouterPrx
    getRouter()
    {
        //
        // No mutex lock necessary, _router is immutable.
        //
        return _router;
    }

    public synchronized Ice.ObjectPrx
    getClientProxy()
    {
        if(_clientProxy == null) // Lazy initialization.
        {
            _clientProxy = _router.getClientProxy();
            if(_clientProxy == null)
            {
                throw new Ice.NoEndpointException();
            }
            _clientProxy = _clientProxy.ice_router(null); // The client proxy cannot be routed.
        }

        return _clientProxy;
    }

    public synchronized void
    setClientProxy(Ice.ObjectPrx clientProxy)
    {
        _clientProxy = clientProxy.ice_router(null); // The client proxy cannot be routed.
    }

    public Ice.ObjectPrx
    getServerProxy()
    {
        if(_serverProxy == null) // Lazy initialization.
        {
            _serverProxy = _router.getServerProxy();
            if(_serverProxy == null)
            {
                throw new Ice.NoEndpointException();
            }
            _serverProxy = _serverProxy.ice_router(null); // The server proxy cannot be routed.
        }

        return _serverProxy;
    }

    public void
    setServerProxy(Ice.ObjectPrx serverProxy)
    {
        _serverProxy = serverProxy.ice_router(null); // The server proxy cannot be routed.
    }

    public void
    addProxy(Ice.ObjectPrx proxy)
    {
        //
        // No mutex lock necessary, _routingTable is immutable, and
        // RoutingTable is mutex protected.
        //
        if(_routingTable.add(proxy)) // Only add the proxy to the router if it's not already in the routing table.
        {
            _router.addProxy(proxy);
        }
    }

    public synchronized void
    setAdapter(Ice.ObjectAdapter adapter)
    {
        _adapter = adapter;
    }

    public synchronized Ice.ObjectAdapter
    getAdapter()
    {
        return _adapter;
    }

    private final Ice.RouterPrx _router;
    private Ice.ObjectPrx _clientProxy;
    private Ice.ObjectPrx _serverProxy;
    private final Ice.RoutingTable _routingTable;
    private Ice.ObjectAdapter _adapter;
}
