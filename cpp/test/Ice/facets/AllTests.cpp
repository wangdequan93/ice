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
#include <TestCommon.h>
#include <Test.h>

using namespace std;

class EmptyI : virtual public Empty
{
};

GPrx
allTests(const Ice::CommunicatorPtr& communicator)
{
    cout << "testing facet registration exceptions... " << flush;
    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("FacetExceptionTestAdapter");
    Ice::ObjectPtr obj = new EmptyI;
    adapter->add(obj, Ice::stringToIdentity("d"));
    adapter->addFacet(obj, Ice::stringToIdentity("d"), "facetABCD");
    bool gotException = false;
    try
    {
	adapter->addFacet(obj, Ice::stringToIdentity("d"), "facetABCD");
    }
    catch(Ice::AlreadyRegisteredException&)
    {
	gotException = true;
    }
    test(gotException);
    adapter->removeFacet(Ice::stringToIdentity("d"), "facetABCD");
    gotException = false;
    try
    {
	adapter->removeFacet(Ice::stringToIdentity("d"), "facetABCD");
    }
    catch(Ice::NotRegisteredException&)
    {
	gotException = true;
    }
    test(gotException);
    cout << "ok" << endl;

    adapter->deactivate();

    cout << "testing stringToProxy... " << flush;
    string ref = "d:default -p 12345 -t 10000";
    Ice::ObjectPrx db = communicator->stringToProxy(ref);
    test(db);
    cout << "ok" << endl;

    cout << "testing checked cast... " << flush;
    DPrx d = DPrx::checkedCast(db);
    test(d);
    test(d == db);
    cout << "ok" << endl;

    cout << "testing non-facets A, B, C, and D... " << flush;
    test(d->callA() == "A");
    test(d->callB() == "B");
    test(d->callC() == "C");
    test(d->callD() == "D");
    cout << "ok" << endl;

    cout << "testing facets A, B, C, and D... " << flush;
    DPrx df = DPrx::checkedCast(d, "facetABCD");
    test(df);
    test(df->callA() == "A");
    test(df->callB() == "B");
    test(df->callC() == "C");
    test(df->callD() == "D");
    cout << "ok" << endl;

    cout << "testing facets E and F... " << flush;
    FPrx ff = FPrx::checkedCast(d, "facetEF");
    test(ff);
    test(ff->callE() == "E");
    test(ff->callF() == "F");
    cout << "ok" << endl;

    cout << "testing facet G... " << flush;
    GPrx gf = GPrx::checkedCast(ff, "facetGH");
    test(gf);
    test(gf->callG() == "G");
    cout << "ok" << endl;

    cout << "testing whether casting preserves the facet... " << flush;
    HPrx hf = HPrx::checkedCast(gf);
    test(hf);
    test(hf->callG() == "G");
    test(hf->callH() == "H");
    cout << "ok" << endl;

    return gf;
}
