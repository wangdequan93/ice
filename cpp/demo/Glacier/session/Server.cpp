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
#include <Glacier/Glacier.h>
#include <HelloSessionI.h>

using namespace std;

class PermissionsVerifierI : public Glacier::PermissionsVerifier
{
public:

    virtual bool
    checkPermissions(const string& userId, const string& password, string&, const Ice::Current&) const
    {
	return userId == password;
    }
};

int
run(int argc, char* argv[], const Ice::CommunicatorPtr& communicator)
{
    Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("HelloSessionManager");

    //
    // Create the SessionManager.
    //
    Ice::ObjectPtr object = new HelloSessionManagerI(adapter);
    adapter->add(object, Ice::stringToIdentity("HelloSessionManager"));

    //
    // Create the PermissionsVerifier.
    //
    Ice::ObjectPtr verifier = new PermissionsVerifierI;
    adapter->add(verifier, Ice::stringToIdentity("PermissionsVerifier"));

    adapter->activate();
    communicator->waitForShutdown();
    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    int status;
    Ice::CommunicatorPtr communicator;

    try
    {
	Ice::PropertiesPtr properties = Ice::createProperties(argc, argv);
        properties->load("config");
	communicator = Ice::initializeWithProperties(argc, argv, properties);
	status = run(argc, argv, communicator);
    }
    catch(const Ice::Exception& ex)
    {
	cerr << ex << endl;
	status = EXIT_FAILURE;
    }

    if(communicator)
    {
	try
	{
	    communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
	    cerr << ex << endl;
	    status = EXIT_FAILURE;
	}
    }

    return status;
}
