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
#include <IceStorm/OnewaySubscriber.h>
#include <IceStorm/SubscriberFactory.h>
#include <IceStorm/TraceLevels.h>

using namespace IceStorm;
using namespace std;

OnewaySubscriber::OnewaySubscriber(const SubscriberFactoryPtr& factory, const TraceLevelsPtr& traceLevels,
                                   const QueuedProxyPtr& obj) :
    Subscriber(traceLevels, obj->proxy()->ice_getIdentity()),
    _factory(factory), _obj(obj)
{
    _factory->incProxyUsageCount(_obj);
}

OnewaySubscriber::~OnewaySubscriber()
{
    _factory->decProxyUsageCount(_obj);
}

bool
OnewaySubscriber::persistent() const
{
    return false;
}

void
OnewaySubscriber::unsubscribe()
{
    IceUtil::Mutex::Lock sync(_stateMutex);
    _state = StateUnsubscribed;

    if(_traceLevels->subscriber > 0)
    {
	Ice::Trace out(_traceLevels->logger, _traceLevels->subscriberCat);
	out << "Unsubscribe " << id();
    }
}

void
OnewaySubscriber::replace()
{
    IceUtil::Mutex::Lock sync(_stateMutex);
    _state = StateReplaced;

    if(_traceLevels->subscriber > 0)
    {
	Ice::Trace out(_traceLevels->logger, _traceLevels->subscriberCat);
	out << "Replace " << id();
    }
}

void
OnewaySubscriber::publish(const EventPtr& event)
{
    try
    {
        _obj->publish(event);
    }
    catch(const Ice::LocalException& e)
    {
	IceUtil::Mutex::Lock sync(_stateMutex);

        //
        // It's possible that the subscriber was unsubscribed, or
        // marked invalid by another thread. Don't display a
        // diagnostic in this case.
        //
        if(_state == StateActive)
        {
            if(_traceLevels->subscriber > 0)
            {
                Ice::Trace out(_traceLevels->logger, _traceLevels->subscriberCat);
                out << id() << ": publish failed: " << e;
            }
            _state = StateError;
        }
    }
}
