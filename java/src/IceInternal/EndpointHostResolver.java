// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

public class EndpointHostResolver
{
    EndpointHostResolver(Instance instance)
    {
        _instance = instance;
        _protocol = instance.protocolSupport();
        _preferIPv6 = instance.preferIPv6();
        try
        {
            _threadName = Util.createThreadName(_instance.initializationData().properties, "Ice.HostResolver");
            _executor = java.util.concurrent.Executors.newFixedThreadPool(1,
                            Util.createThreadFactory(_instance.initializationData().properties, _threadName));
            updateObserver();
        }
        catch(RuntimeException ex)
        {
            String s = "cannot create thread for endpoint host resolver thread:\n" + Ex.toString(ex);
            _instance.initializationData().logger.error(s);
            throw ex;
        }
    }

    public java.util.List<Connector> resolve(String host, int port, Ice.EndpointSelectionType selType,
                                             IPEndpointI endpoint)
    {
        //
        // Try to get the addresses without DNS lookup. If this doesn't
        // work, we retry with DNS lookup (and observer).
        //
        NetworkProxy networkProxy = _instance.networkProxy();
        if(networkProxy == null)
        {
            java.util.List<java.net.InetSocketAddress> addrs =
                Network.getAddresses(host, port, _protocol, selType, _preferIPv6);
            if(!addrs.isEmpty())
            {
                return endpoint.connectors(addrs, null);
            }
        }

        Ice.Instrumentation.CommunicatorObserver obsv = _instance.getObserver();
        Ice.Instrumentation.Observer observer = null;
        if(obsv != null)
        {
            observer = obsv.getEndpointLookupObserver(endpoint);
            if(observer != null)
            {
                observer.attach();
            }
        }

        java.util.List<Connector> connectors = null;
        try
        {
            if(networkProxy != null)
            {
                networkProxy = networkProxy.resolveHost();
            }

            connectors = endpoint.connectors(Network.getAddresses(host, port, _protocol, selType, _preferIPv6),
                                             networkProxy);
        }
        catch(Ice.LocalException ex)
        {
            if(observer != null)
            {
                observer.failed(ex.ice_name());
            }
            throw ex;
        }
        finally
        {
            if(observer != null)
            {
                observer.detach();
            }
        }
        return connectors;
    }

    synchronized public void resolve(final String host, final int port, final Ice.EndpointSelectionType selType, final IPEndpointI endpoint,
            final EndpointI_connectors callback)
    {
        //
        // TODO: Optimize to avoid the lookup if the given host is a textual IPv4 or IPv6
        // address. This requires implementing parsing of IPv4/IPv6 addresses (Java does
        // not provide such methods).
        //

        assert(!_destroyed);

        final Ice.Instrumentation.ThreadObserver threadObserver = _observer;
        final Ice.Instrumentation.Observer observer = getObserver(endpoint);
        if(observer != null)
        {
            observer.attach();
        }

        _executor.execute(new Runnable()
            {
                @Override
                public void run()
                {
                    synchronized(EndpointHostResolver.this)
                    {
                        if(_destroyed)
                        {
                            Ice.CommunicatorDestroyedException ex = new Ice.CommunicatorDestroyedException();
                            if(observer != null)
                            {
                                observer.failed(ex.ice_name());
                                observer.detach();
                            }
                            callback.exception(ex);
                            return;
                        }
                    }

                    try
                    {
                        if(threadObserver != null)
                        {
                            threadObserver.stateChanged(Ice.Instrumentation.ThreadState.ThreadStateIdle,
                                                        Ice.Instrumentation.ThreadState.ThreadStateInUseForOther);
                        }

                        NetworkProxy networkProxy = _instance.networkProxy();
                        if(networkProxy != null)
                        {
                            networkProxy = networkProxy.resolveHost();
                        }

                        callback.connectors(endpoint.connectors(Network.getAddresses(host,
                                                                                         port,
                                                                                         _protocol,
                                                                                         selType,
                                                                                         _preferIPv6),
                                                                    networkProxy));

                        if(threadObserver != null)
                        {
                            threadObserver.stateChanged(Ice.Instrumentation.ThreadState.ThreadStateInUseForOther,
                                                        Ice.Instrumentation.ThreadState.ThreadStateIdle);
                        }

                        if(observer != null)
                        {
                            observer.detach();
                        }
                    }
                    catch(Ice.LocalException ex)
                    {
                        if(observer != null)
                        {
                            observer.failed(ex.ice_name());
                            observer.detach();
                        }
                        callback.exception(ex);
                    }
                }
            });
    }

    synchronized public void destroy()
    {
        assert(!_destroyed);
        _destroyed = true;

        //
        // Shutdown the executor. No new tasks will be accepted.
        // Existing tasks will execute.
        //
        _executor.shutdown();
    }

    public void joinWithThread()
        throws InterruptedException
    {
        // Wait for the executor to terminate.
        try
        {
            _executor.awaitTermination(Long.MAX_VALUE, java.util.concurrent.TimeUnit.NANOSECONDS);
        }
        finally
        {
            if(_observer != null)
            {
                _observer.detach();
            }
        }
    }

    synchronized public void updateObserver()
    {
        Ice.Instrumentation.CommunicatorObserver obsv = _instance.getObserver();
        if(obsv != null)
        {
            _observer = obsv.getThreadObserver("Communicator", _threadName,
                                               Ice.Instrumentation.ThreadState.ThreadStateIdle,
                                               _observer);
            if(_observer != null)
            {
                _observer.attach();
            }
        }
    }

    private Ice.Instrumentation.Observer
    getObserver(IPEndpointI endpoint)
    {
        Ice.Instrumentation.CommunicatorObserver obsv = _instance.getObserver();
        if(obsv != null)
        {
            return obsv.getEndpointLookupObserver(endpoint);
        }
        return null;
    }

    private final Instance _instance;
    private final int _protocol;
    private final boolean _preferIPv6;
    private boolean _destroyed;
    private Ice.Instrumentation.ThreadObserver _observer;
    private String _threadName;
    private java.util.concurrent.ExecutorService _executor;
}
