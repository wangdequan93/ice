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


#include <PhoneBookI.h>
#include <IceUtil/UUID.h>

using namespace std;
using namespace Ice;

ContactI::ContactI(const ContactFactoryPtr& contactFactory) :
    _factory(contactFactory)
{
}

string
ContactI::getName(const Ice::Current&) const
{
    IceUtil::RWRecMutex::RLock sync(*this);
    return name;
}

void
ContactI::setName(const string& newName, const Ice::Current&)
{
    IceUtil::RWRecMutex::WLock sync(*this);
    name = newName;
}

string
ContactI::getAddress(const Ice::Current&) const
{
    IceUtil::RWRecMutex::RLock sync(*this);
    return address;
}

void
ContactI::setAddress(const string& newAddress, const Ice::Current&)
{
    IceUtil::RWRecMutex::WLock sync(*this);
    address = newAddress;
}

string
ContactI::getPhone(const Ice::Current&) const
{
    IceUtil::RWRecMutex::RLock sync(*this);
    return phone;
}

void
ContactI::setPhone(const string& newPhone, const Ice::Current&)
{
    IceUtil::RWRecMutex::WLock sync(*this);
    phone = newPhone;
}

void
ContactI::destroy(const Ice::Current& c)
{
    IceUtil::RWRecMutex::RLock sync(*this);
    try
    {
	_factory->getEvictor()->remove(c.id);
    }
    catch(const Freeze::DatabaseException& ex)
    {
	DatabaseException e;
	e.message = ex.message;
	throw e;
    }
}

PhoneBookI::PhoneBookI(const Freeze::EvictorPtr& evictor, const ContactFactoryPtr& contactFactory,
		       const NameIndexPtr& index) :
    _evictor(evictor),
    _contactFactory(contactFactory),
    _index(index)
{
}

class IdentityToContact
{
public:

    IdentityToContact(const ObjectAdapterPtr& adapter) :
	_adapter(adapter)
    {
    }

    ContactPrx operator()(const Identity& ident)
    {
	return ContactPrx::uncheckedCast(_adapter->createProxy(ident));
    }

private:

    ObjectAdapterPtr _adapter;
};

ContactPrx
PhoneBookI::createContact(const Ice::Current& c)
{
    //
    // Get a new unique identity.
    //
    Identity ident;
    ident.name = IceUtil::generateUUID();
    ident.category = "contact";

    //
    // Create a new Contact Servant.
    //
    ContactIPtr contact = new ContactI(_contactFactory);
    
    //
    // Create a new Ice Object in the evictor, using the new identity
    // and the new Servant.
    //
    _evictor->add(contact, ident);

    
    //
    // Turn the identity into a Proxy and return the Proxy to the
    // caller.
    //
    return IdentityToContact(c.adapter)(ident);
}

Contacts
PhoneBookI::findContacts(const string& name, const Ice::Current& c) const
{
    try
    {
	vector<Identity> identities = _index->find(name);
	
	Contacts contacts;
	contacts.reserve(identities.size());
	transform(identities.begin(), identities.end(), back_inserter(contacts), IdentityToContact(c.adapter));

	return contacts;
    }
    catch(const Freeze::DatabaseException& ex)
    {
	DatabaseException e;
	e.message = ex.message;
	throw e;
    }
}

void
PhoneBookI::setEvictorSize(Int size, const Ice::Current&)
{
    _evictor->setSize(size);
}

void
PhoneBookI::shutdown(const Ice::Current& c) const
{
    c.adapter->getCommunicator()->shutdown();
}

