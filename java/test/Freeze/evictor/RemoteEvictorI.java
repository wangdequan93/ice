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


public final class RemoteEvictorI extends Test._RemoteEvictorDisp
{
    static class Initializer extends Ice.LocalObjectImpl implements Freeze.ServantInitializer
    {
        public void
        initialize(Ice.ObjectAdapter adapter, Ice.Identity ident, String facet, Ice.Object servant)
        {
	    if(facet.length() == 0)
	    {
		ServantI servantImpl =  (ServantI) ((Test._ServantTie) servant).ice_delegate();
		servantImpl.init(_remoteEvictor, _evictor);
	    }
	    else
	    {
		ServantI servantImpl =  (ServantI) ((Test._FacetTie) servant).ice_delegate();
		servantImpl.init(_remoteEvictor, _evictor);
	    }
        }

        void init(RemoteEvictorI remoteEvictor, Freeze.Evictor evictor)
        {
            _remoteEvictor = remoteEvictor;
            _evictor = evictor;
        }

        private RemoteEvictorI _remoteEvictor;
        private Freeze.Evictor _evictor;
    }


    RemoteEvictorI(Ice.ObjectAdapter adapter, String envName, String category)
    {
	_adapter = adapter;
        _category = category;
	_evictorAdapter = _adapter.getCommunicator().
	    createObjectAdapterWithEndpoints(Ice.Util.generateUUID(), "default");

	Initializer initializer = new Initializer();

	_evictor = Freeze.Util.createEvictor(_evictorAdapter, envName, category, initializer, null, true);
	initializer.init(this, _evictor);
        
        _evictorAdapter.addServantLocator(_evictor, category);
        _evictorAdapter.activate();
    }

    public void
    setSize(int size, Ice.Current current)
    {
        _evictor.setSize(size);
    }

    public Test.ServantPrx
    createServant(int id, int value, Ice.Current current)
	throws Test.AlreadyRegisteredException, Test.EvictorDeactivatedException
    {
        Ice.Identity ident = new Ice.Identity();
        ident.category = _category;
        ident.name = "" + id;
	Test._ServantTie tie = new Test._ServantTie();
	tie.ice_delegate(new ServantI(tie, this, _evictor, value));
	try
	{
	    return Test.ServantPrxHelper.uncheckedCast(_evictor.add(tie, ident));
	}
	catch(Ice.AlreadyRegisteredException e)
	{
	    throw new Test.AlreadyRegisteredException();
	}
	catch(Ice.ObjectAdapterDeactivatedException e)
	{
	    throw new Test.EvictorDeactivatedException();
	}
	catch(Freeze.EvictorDeactivatedException e)
	{
	    throw new Test.EvictorDeactivatedException();
	}
	catch(Ice.LocalException e)
	{
	    System.out.println("Throwing " + e.toString());
	    throw e;
	}
    }

    public Test.ServantPrx
    getServant(int id, Ice.Current current)
    {
        Ice.Identity ident = new Ice.Identity();
        ident.category = _category;
        ident.name = "" + id;
        return Test.ServantPrxHelper.uncheckedCast(_evictorAdapter.createProxy(ident));
    }

    public void
    saveNow(Ice.Current current)
    {
	_evictor.getIterator("", 1);
    }

    public void
    deactivate(Ice.Current current)
    {
        _evictorAdapter.deactivate();
        _evictorAdapter.waitForDeactivate();
        _adapter.remove(Ice.Util.stringToIdentity(_category));
    }
    
    public void
    destroyAllServants(String facet, Ice.Current current)
    {
	//
	// Only for test purpose: don't use such a small value in 
	// a real application!
	//
	int batchSize = 2;

	Freeze.EvictorIterator p = _evictor.getIterator(facet, batchSize);
	while(p.hasNext())
	{
	    _evictor.remove((Ice.Identity) p.next());
	}
    }

    private Ice.ObjectAdapter _adapter;
    private String _category;
    private Freeze.Evictor _evictor;
    private Ice.ObjectAdapter _evictorAdapter;
}
