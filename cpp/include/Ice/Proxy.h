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


#ifndef ICE_PROXY_H
#define ICE_PROXY_H

#include <IceUtil/Shared.h>
#include <IceUtil/Mutex.h>
#include <Ice/ProxyF.h>
#include <Ice/ProxyFactoryF.h>
#include <Ice/ConnectionF.h>
#include <Ice/EndpointF.h>
#include <Ice/ObjectF.h>
#include <Ice/ObjectAdapterF.h>
#include <Ice/ReferenceF.h>
#include <Ice/OutgoingAsyncF.h>
//#include <Ice/RouterF.h> // Can't include RouterF.h here, otherwise we have cyclic includes
//#include <Ice/LocatorF.h> // Can't include RouterF.h here, otherwise we have cyclic includes
#include <Ice/Current.h>

namespace IceProxy
{

namespace Ice
{

class Router;
class Locator;

}

}

namespace IceInternal
{

ICE_API void incRef(::IceProxy::Ice::Router*);
ICE_API void decRef(::IceProxy::Ice::Router*);

ICE_API void incRef(::IceProxy::Ice::Locator*);
ICE_API void decRef(::IceProxy::Ice::Locator*);

}

namespace Ice
{

typedef ::IceInternal::ProxyHandle< ::IceProxy::Ice::Router> RouterPrx;
typedef ::IceInternal::ProxyHandle< ::IceProxy::Ice::Locator> LocatorPrx;

class LocalException;

}

namespace IceProxy { namespace Ice
{

#if defined(_MSC_VER) && (_MSC_VER == 1310)
// Work around for VC++ 7.1 bug
typedef ::std::map< ::std::string, ::std::string> Context;
#endif

class ICE_API Object : public ::IceUtil::Shared, ::IceUtil::Mutex
{
public:

    bool operator==(const Object&) const;
    bool operator!=(const Object&) const;
    bool operator<(const Object&) const;
    ::Ice::Int ice_hash() const;

    bool ice_isA(const ::std::string&);
    bool ice_isA(const ::std::string&, const ::Ice::Context&);
    void ice_ping();
    void ice_ping(const ::Ice::Context&);
    ::std::vector< ::std::string> ice_ids();
    ::std::vector< ::std::string> ice_ids(const ::Ice::Context&);
    ::std::string ice_id();
    ::std::string ice_id(const ::Ice::Context&);
    bool ice_invoke(const ::std::string&, ::Ice::OperationMode, const ::std::vector< ::Ice::Byte>&,
	            ::std::vector< ::Ice::Byte>&); // Returns true if ok, false if user exception.
    bool ice_invoke(const ::std::string&, ::Ice::OperationMode, const ::std::vector< ::Ice::Byte>&,
	            ::std::vector< ::Ice::Byte>&,
		    const ::Ice::Context&); // Returns true if ok, false if user exception.
    void ice_invoke_async(const ::Ice::AMI_Object_ice_invokePtr&,
			  const ::std::string&, ::Ice::OperationMode, const ::std::vector< ::Ice::Byte>&);
    void ice_invoke_async(const ::Ice::AMI_Object_ice_invokePtr&,
			  const ::std::string&, ::Ice::OperationMode, const ::std::vector< ::Ice::Byte>&,
			  const ::Ice::Context&);

    ::Ice::Identity ice_getIdentity() const;
    ::Ice::ObjectPrx ice_newIdentity(const ::Ice::Identity&) const;

    ::Ice::Context ice_getContext() const;
    ::Ice::ObjectPrx ice_newContext(const ::Ice::Context&) const;

    ::std::string ice_getFacet() const;
    ::Ice::ObjectPrx ice_newFacet(const ::std::string&) const;

    ::Ice::ObjectPrx ice_twoway() const;
    bool ice_isTwoway() const;
    ::Ice::ObjectPrx ice_oneway() const;
    bool ice_isOneway() const;
    ::Ice::ObjectPrx ice_batchOneway() const;
    bool ice_isBatchOneway() const;
    ::Ice::ObjectPrx ice_datagram() const;
    bool ice_isDatagram() const;
    ::Ice::ObjectPrx ice_batchDatagram() const;
    bool ice_isBatchDatagram() const;

    ::Ice::ObjectPrx ice_secure(bool) const;
    ::Ice::ObjectPrx ice_compress(bool) const;
    ::Ice::ObjectPrx ice_timeout(int) const;
    ::Ice::ObjectPrx ice_router(const ::Ice::RouterPrx&) const;
    ::Ice::ObjectPrx ice_locator(const ::Ice::LocatorPrx&) const;
    ::Ice::ObjectPrx ice_collocationOptimization(bool) const;
    ::Ice::ObjectPrx ice_default() const;

    ::IceInternal::ReferencePtr __reference() const;
    void __copyFrom(const ::Ice::ObjectPrx&);
    void __handleException(const ::Ice::LocalException&, int&);
    void __rethrowException(const ::Ice::LocalException&);
    void __checkTwowayOnly(const char*) const;

    ::IceInternal::Handle< ::IceDelegate::Ice::Object> __getDelegate();

protected:

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();

    const ::Ice::Context& __defaultContext() const;

private:

    void setup(const ::IceInternal::ReferencePtr&);
    friend class ::IceInternal::ProxyFactory;

    ::IceInternal::ReferencePtr _reference;
    ::IceInternal::Handle< ::IceDelegate::Ice::Object> _delegate;
};

} }

namespace IceDelegate { namespace Ice
{

class ICE_API Object : public ::IceUtil::Shared
{
public:

    virtual bool ice_isA(const ::std::string&, const ::Ice::Context&) = 0;
    virtual void ice_ping(const ::Ice::Context&) = 0;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Context&) = 0;
    virtual ::std::string ice_id(const ::Ice::Context&) = 0;
    virtual bool ice_invoke(const ::std::string&, ::Ice::OperationMode, const ::std::vector< ::Ice::Byte>&,
			    ::std::vector< ::Ice::Byte>&, const ::Ice::Context&) = 0;
};

} }

namespace IceDelegateM { namespace Ice
{

class ICE_API Object : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual ~Object();

    virtual bool ice_isA(const ::std::string&, const ::Ice::Context&);
    virtual void ice_ping(const ::Ice::Context&);
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Context&);
    virtual ::std::string ice_id(const ::Ice::Context&);
    virtual bool ice_invoke(const ::std::string&, ::Ice::OperationMode, const ::std::vector< ::Ice::Byte>&,
			    ::std::vector< ::Ice::Byte>&, const ::Ice::Context&);

    void __copyFrom(const ::IceInternal::Handle< ::IceDelegateM::Ice::Object>&);

protected:

    ::IceInternal::ReferencePtr __reference;
    ::IceInternal::ConnectionPtr __connection;

private:

    void setup(const ::IceInternal::ReferencePtr&);
    friend class ::IceProxy::Ice::Object;
};

} }

namespace IceDelegateD { namespace Ice
{

class ICE_API Object : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual bool ice_isA(const ::std::string&, const ::Ice::Context&);
    virtual void ice_ping(const ::Ice::Context&);
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Context&);
    virtual ::std::string ice_id(const ::Ice::Context&);
    virtual bool ice_invoke(const ::std::string&, ::Ice::OperationMode, const ::std::vector< ::Ice::Byte>&,
			    ::std::vector< ::Ice::Byte>&, const ::Ice::Context&);
    void __copyFrom(const ::IceInternal::Handle< ::IceDelegateD::Ice::Object>&);

protected:

    ::IceInternal::ReferencePtr __reference;
    ::Ice::ObjectAdapterPtr __adapter;

    void __initCurrent(::Ice::Current&, const ::std::string&, ::Ice::OperationMode, const ::Ice::Context&);

private:

    void setup(const ::IceInternal::ReferencePtr&, const ::Ice::ObjectAdapterPtr&);
    friend class ::IceProxy::Ice::Object;
};

} }

namespace Ice
{

ICE_API bool proxyIdentityLess(const ObjectPrx&, const ObjectPrx&);
ICE_API bool proxyIdentityEqual(const ObjectPrx&, const ObjectPrx&);

ICE_API bool proxyIdentityAndFacetLess(const ObjectPrx&, const ObjectPrx&);
ICE_API bool proxyIdentityAndFacetEqual(const ObjectPrx&, const ObjectPrx&);

struct ProxyIdentityLess : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
	return proxyIdentityLess(lhs, rhs);
    }
};

struct ProxyIdentityEqual : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
	return proxyIdentityEqual(lhs, rhs);
    }
};

struct ProxyIdentityAndFacetLess : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
	return proxyIdentityAndFacetLess(lhs, rhs);
    }
};

struct ProxyIdentityAndFacetEqual : std::binary_function<bool, ObjectPrx&, ObjectPrx&>
{
    bool operator()(const ObjectPrx& lhs, const ObjectPrx& rhs) const
    {
	return proxyIdentityAndFacetEqual(lhs, rhs);
    }
};

}

namespace IceInternal
{

//
// checkedCast and uncheckedCast functions without facet:
//

//
// Out of line implementations
//
template<typename P> P 
checkedCastImpl(const ::Ice::ObjectPrx& b)
{
    P d = 0;
    if(b.get())
    {
	typedef typename P::element_type T;

	d = dynamic_cast<T*>(b.get());
	if(!d && b->ice_isA(T::ice_staticId()))
	{
	    d = new T;
	    d->__copyFrom(b);
	}
    }
    return d;
}

template<typename P> P 
uncheckedCastImpl(const ::Ice::ObjectPrx& b)
{
    P d = 0;
    if(b)
    {
	typedef typename P::element_type T;

	d = dynamic_cast<T*>(b.get());
	if(!d)
	{
	    d = new T;
	    d->__copyFrom(b);
	}
    }
    return d;
}

// 
// checkedCast and uncheckedCast with facet:
//

//
// Helper; last parameter = typeId
//
ICE_API ::Ice::ObjectPrx checkedCastImpl(const ::Ice::ObjectPrx&, const std::string&, const std::string&);

//
// Specializations for P = ::Ice::ObjectPrx
// We have to use inline functions for broken compilers such as VC7.
//

template<> inline ::Ice::ObjectPrx 
checkedCastImpl< ::Ice::ObjectPrx>(const ::Ice::ObjectPrx& b, const std::string& f)
{
    return checkedCastImpl(b, f, "::Ice::Object");
}

template<> inline ::Ice::ObjectPrx 
uncheckedCastImpl< ::Ice::ObjectPrx>(const ::Ice::ObjectPrx& b, const std::string& f)
{
    ::Ice::ObjectPrx d = 0;
    if(b)
    {
	d = b->ice_newFacet(f);
    }
    return d;
}

template<typename P> P 
checkedCastImpl(const ::Ice::ObjectPrx& b, const std::string& f)
{
    P d = 0;

    typedef typename P::element_type T;
    ::Ice::ObjectPrx bb = checkedCastImpl(b, f, T::ice_staticId());

    if(bb)
    {
	d = new T;
	d->__copyFrom(bb);
    }
    return d;
}

template<typename P> P 
uncheckedCastImpl(const ::Ice::ObjectPrx& b, const std::string& f)
{
    P d = 0;
    if(b)
    {
	typedef typename P::element_type T;

	::Ice::ObjectPrx bb = b->ice_newFacet(f);
	d = new T;
	d->__copyFrom(bb);
    }
    return d;
}
}

//
// checkedCast and uncheckedCast functions provided in the global namespace
// 

template<typename P, typename Y> inline P 
checkedCast(const ::IceInternal::ProxyHandle<Y>& b)
{
    Y* tag = 0;
#if defined(_MSC_VER) && (_MSC_VER < 1300)
    return ::IceInternal::checkedCastHelper<P::element_type>(b, tag);
#else
    return ::IceInternal::checkedCastHelper<typename P::element_type>(b, tag);
#endif

}
template<typename P, typename Y> inline P
uncheckedCast(const ::IceInternal::ProxyHandle<Y>& b)
{
    Y* tag = 0;
#if defined(_MSC_VER) && (_MSC_VER < 1300)
    return ::IceInternal::uncheckedCastHelper<P::element_type>(b, tag);
#else
    return ::IceInternal::uncheckedCastHelper<typename P::element_type>(b, tag);
#endif
}

template<typename P> inline P 
checkedCast(const ::Ice::ObjectPrx& b, const std::string& f)
{
    return ::IceInternal::checkedCastImpl<P>(b, f);
}

template<typename P> inline P 
uncheckedCast(const ::Ice::ObjectPrx& b, const std::string& f)
{
    return ::IceInternal::checkedCastImpl<P>(b, f);
}

#endif

