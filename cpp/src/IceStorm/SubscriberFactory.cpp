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
#include <IceStorm/SubscriberFactory.h>
#include <IceStorm/LinkSubscriber.h>
#include <IceStorm/OnewaySubscriber.h>
#include <IceStorm/OnewayBatchSubscriber.h>
#include <IceStorm/Flusher.h>
#include <IceStorm/TraceLevels.h>
#include <IceStorm/LinkProxy.h>
#include <IceStorm/OnewayProxy.h>

using namespace std;
using namespace IceStorm;

SubscriberFactory::SubscriberFactory(const Ice::CommunicatorPtr& communicator,
				     const TraceLevelsPtr& traceLevels,
				     const FlusherPtr& flusher) :
    _communicator(communicator),
    _traceLevels(traceLevels),
    _flusher(flusher)
{
}

SubscriberPtr
SubscriberFactory::createLinkSubscriber(const TopicLinkPrx& obj, Ice::Int cost)
{
    IceUtil::RecMutex::Lock sync(_proxiesMutex);

    //
    // Delivery to links is done in batch mode.
    //
    TopicLinkPrx newObj = TopicLinkPrx::uncheckedCast(obj->ice_batchOneway());

    //
    // Check if a queued proxy already exists, or create one if necessary.
    //
    QueuedProxyPtr proxy;
    map<Ice::ObjectPrx, ProxyInfo>::iterator p = _proxies.find(newObj);
    if(p != _proxies.end())
    {
        proxy = p->second.proxy;
    }
    else
    {
        proxy = new LinkProxy(newObj);
        ProxyInfo info;
        info.proxy = proxy;
        info.count = 0;
        _proxies.insert(pair<Ice::ObjectPrx, ProxyInfo>(newObj, info));
    }

    return new LinkSubscriber(this, _communicator, _traceLevels, proxy, cost);
}

SubscriberPtr
SubscriberFactory::createSubscriber(const QoS& qos, const Ice::ObjectPrx& obj)
{
    IceUtil::RecMutex::Lock sync(_proxiesMutex);

    //
    // Determine the requested reliability characteristics
    //
    Ice::ObjectPrx newObj;

    QoS::const_iterator i = qos.find("reliability");
    string reliability;
    if(i == qos.end())
    {
	reliability = "oneway";
    }
    else
    {
	reliability = i->second;
    }

    if(reliability == "batch")
    {
        if(obj->ice_isDatagram())
        {
            newObj = obj->ice_batchDatagram();
        }
        else
        {
            newObj = obj->ice_batchOneway();
        }
    }
    else // reliability == "oneway"
    {
	if(reliability != "oneway")
	{
	    if(_traceLevels->subscriber > 0)
	    {
		Ice::Trace out(_traceLevels->logger, _traceLevels->subscriberCat);
		out << reliability <<" mode not understood.";
	    }
	}
        if(obj->ice_isDatagram())
        {
            newObj = obj;
        }
        else
        {
            newObj = obj->ice_oneway();
        }
    }

    //
    // Check if a queued proxy already exists, or create one if necessary.
    //
    QueuedProxyPtr proxy;
    map<Ice::ObjectPrx, ProxyInfo>::iterator p = _proxies.find(newObj);
    if(p != _proxies.end())
    {
        proxy = p->second.proxy;
    }
    else
    {
        proxy = new OnewayProxy(newObj);
        ProxyInfo info;
        info.proxy = proxy;
        info.count = 0;
        _proxies.insert(pair<Ice::ObjectPrx, ProxyInfo>(newObj, info));
    }

    if(reliability == "batch")
    {
        return new OnewayBatchSubscriber(this, _communicator, _traceLevels, _flusher, proxy);
    }
    else
    {
        return new OnewaySubscriber(this, _traceLevels, proxy);
    }
}

void
SubscriberFactory::incProxyUsageCount(const QueuedProxyPtr& proxy)
{
    IceUtil::RecMutex::Lock sync(_proxiesMutex);

    map<Ice::ObjectPrx, ProxyInfo>::iterator p = _proxies.find(proxy->proxy());
    assert(p->second.count >= 0);
    p->second.count++;
}

void
SubscriberFactory::decProxyUsageCount(const QueuedProxyPtr& proxy)
{
    IceUtil::RecMutex::Lock sync(_proxiesMutex);

    map<Ice::ObjectPrx, ProxyInfo>::iterator p = _proxies.find(proxy->proxy());
    assert(p->second.count > 0);
    p->second.count--;
    if(p->second.count == 0)
    {
        _proxies.erase(p);
    }
}
