// **********************************************************************
//
// Copyright (c) 2003-2014 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package IceInternal;

final class UdpTransceiver implements Transceiver
{
    @Override
    public java.nio.channels.SelectableChannel fd()
    {
        assert(_fd != null);
        return _fd;
    }

    @Override
    public int initialize(Buffer readBuffer, Buffer writeBuffer, Ice.BooleanHolder moreData)
    {
        //
        // Nothing to do.
        //
        return SocketOperation.None;
    }

    @Override
    public int closing(boolean initiator, Ice.LocalException ex)
    {
        //
        // Nothing to do.
        //
        return SocketOperation.None;
    }

    @Override
    public void close()
    {
        assert(_fd != null);

        if(_state >= StateConnected && _instance.traceLevel() >= 1)
        {
            String s = "closing " + _instance.protocol() + " connection\n" + toString();
            _instance.logger().trace(_instance.traceCategory(), s);
        }

        try
        {
            _fd.close();
        }
        catch(java.io.IOException ex)
        {
        }
        _fd = null;
    }

    @Override
    public int write(Buffer buf)
    {
        //
        // We don't want write or send to be called on android main thread as this will cause
        // NetworkOnMainThreadException to be thrown. If that is the android main thread
        // we return false and this method will be later called from the thread pool
        //
        if(Util.isAndroidMainThread(Thread.currentThread()))
        {
            return SocketOperation.Write;
        }

        if(!buf.b.hasRemaining())
        {
            return SocketOperation.None;
        }

        assert(buf.b.position() == 0);
        assert(_fd != null && _state >= StateConnected);

        // The caller is supposed to check the send size before by calling checkSendSize
        assert(java.lang.Math.min(_maxPacketSize, _sndSize - _udpOverhead) >= buf.size());

        int ret = 0;
        while(true)
        {
            try
            {
                if(_state == StateConnected)
                {
                    ret = _fd.write(buf.b);
                }
                else
                {
                    if(_peerAddr == null)
                    {
                        throw new Ice.SocketException(); // No peer has sent a datagram yet.
                    }
                    ret = _fd.send(buf.b, _peerAddr);
                }
                break;
            }
            catch(java.nio.channels.AsynchronousCloseException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
            catch(java.net.PortUnreachableException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
            catch(java.io.InterruptedIOException ex)
            {
                continue;
            }
            catch(java.io.IOException ex)
            {
                throw new Ice.SocketException(ex);
            }
        }

        if(ret == 0)
        {
            return SocketOperation.Write;
        }

        if(_instance.traceLevel() >= 3)
        {
            String s = "sent " + ret + " bytes via " + _instance.protocol() + "\n" + toString();
            _instance.logger().trace(_instance.traceCategory(), s);
        }

        assert(ret == buf.b.limit());
        return SocketOperation.None;
    }

    @Override
    public int read(Buffer buf, Ice.BooleanHolder moreData)
    {
        if(!buf.b.hasRemaining())
        {
            return SocketOperation.None;
        }

        assert(buf.b.position() == 0);

        final int packetSize = java.lang.Math.min(_maxPacketSize, _rcvSize - _udpOverhead);
        buf.resize(packetSize, true);
        buf.b.position(0);

        int ret = 0;
        while(true)
        {
            try
            {
                java.net.SocketAddress peerAddr = _fd.receive(buf.b);
                if(peerAddr == null || buf.b.position() == 0)
                {
                    return SocketOperation.Read;
                }

                _peerAddr = (java.net.InetSocketAddress)peerAddr;
                ret = buf.b.position();
                break;
            }
            catch(java.nio.channels.AsynchronousCloseException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
            catch(java.net.PortUnreachableException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
            catch(java.io.InterruptedIOException ex)
            {
                continue;
            }
            catch(java.io.IOException ex)
            {
                throw new Ice.ConnectionLostException(ex);
            }
        }

        if(_state == StateNeedConnect)
        {
            //
            // If we must connect, we connect to the first peer that sends us a packet.
            //
            Network.doConnect(_fd, _peerAddr, null);
            _state = StateConnected;

            if(_instance.traceLevel() >= 1)
            {
                String s = "connected " + _instance.protocol() + " socket\n" + toString();
                _instance.logger().trace(_instance.traceCategory(), s);
            }
        }

        if(_instance.traceLevel() >= 3)
        {
            String s = "received " + ret + " bytes via " + _instance.protocol() + "\n" + toString();
            _instance.logger().trace(_instance.traceCategory(), s);
        }

        buf.resize(ret, true);
        buf.b.position(ret);

        return SocketOperation.None;
    }

    @Override
    public String protocol()
    {
        return _instance.protocol();
    }

    @Override
    public String toString()
    {
        if(_fd == null)
        {
            return "<closed>";
        }

        String s;
        if(_state == StateNotConnected)
        {
            java.net.DatagramSocket socket = _fd.socket();
            s = "local address = " + Network.addrToString((java.net.InetSocketAddress)socket.getLocalSocketAddress());
            if(_peerAddr != null)
            {
                s += "\nremote address = " + Network.addrToString(_peerAddr);
            }
        }
        else
        {
            s = Network.fdToString(_fd);
        }

        if(_mcastAddr != null)
        {
            s += "\nmulticast address = " + Network.addrToString(_mcastAddr);
        }
        return s;
    }

    @Override
    public Ice.ConnectionInfo getInfo()
    {
        Ice.UDPConnectionInfo info = new Ice.UDPConnectionInfo();
        if(_fd != null)
        {
            java.net.DatagramSocket socket = _fd.socket();
            info.localAddress = socket.getLocalAddress().getHostAddress();
            info.localPort = socket.getLocalPort();
            if(_state == StateNotConnected)
            {
                if(_peerAddr != null)
                {
                    info.remoteAddress = _peerAddr.getAddress().getHostAddress();
                    info.remotePort = _peerAddr.getPort();
                }
            }
            else
            {
                if(socket.getInetAddress() != null)
                {
                    info.remoteAddress = socket.getInetAddress().getHostAddress();
                    info.remotePort = socket.getPort();
                }
            }
        }
        if(_mcastAddr != null)
        {
            info.mcastAddress = _mcastAddr.getAddress().getHostAddress();
            info.mcastPort = _mcastAddr.getPort();
        }
        return info;
    }

    @Override
    public void checkSendSize(Buffer buf, int messageSizeMax)
    {
        if(buf.size() > messageSizeMax)
        {
            Ex.throwMemoryLimitException(buf.size(), messageSizeMax);
        }

        //
        // The maximum packetSize is either the maximum allowable UDP packet size, or
        // the UDP send buffer size (which ever is smaller).
        //
        final int packetSize = java.lang.Math.min(_maxPacketSize, _sndSize - _udpOverhead);
        if(packetSize < buf.size())
        {
            throw new Ice.DatagramLimitException();
        }
    }

    public final int effectivePort()
    {
        return _addr.getPort();
    }

    //
    // Only for use by UdpEndpoint
    //
    UdpTransceiver(ProtocolInstance instance, java.net.InetSocketAddress addr, java.net.InetSocketAddress sourceAddr,
                   String mcastInterface, int mcastTtl)
    {
        _instance = instance;
        _state = StateNeedConnect;
        _addr = addr;

        try
        {
            _fd = Network.createUdpSocket(_addr);
            setBufSize(instance.properties());
            Network.setBlock(_fd, false);
            //
            // NOTE: setting the multicast interface before performing the
            // connect is important for some OS such as OS X.
            //
            if(_addr.getAddress().isMulticastAddress())
            {
                configureMulticast(null, mcastInterface, mcastTtl);
            }
            Network.doConnect(_fd, _addr, sourceAddr);
            _state = StateConnected; // We're connected now

            if(_instance.traceLevel() >= 1)
            {
                String s = "starting to send " + _instance.protocol() + " packets\n" + toString();
                _instance.logger().trace(_instance.traceCategory(), s);
            }
        }
        catch(Ice.LocalException ex)
        {
            _fd = null;
            throw ex;
        }
    }

    //
    // Only for use by UdpEndpoint
    //
    UdpTransceiver(ProtocolInstance instance, String host, int port, String mcastInterface, boolean connect)
    {
        _instance = instance;
        _state = connect ? StateNeedConnect : StateNotConnected;

        try
        {
            _addr = Network.getAddressForServer(host, port, instance.protocolSupport(), instance.preferIPv6());
            _fd = Network.createUdpSocket(_addr);
            setBufSize(instance.properties());
            Network.setBlock(_fd, false);
            if(_instance.traceLevel() >= 2)
            {
                String s = "attempting to bind to " + _instance.protocol() + " socket " + Network.addrToString(_addr);
                _instance.logger().trace(_instance.traceCategory(), s);
            }
            if(_addr.getAddress().isMulticastAddress())
            {
                Network.setReuseAddress(_fd, true);
                _mcastAddr = _addr;
                if(System.getProperty("os.name").startsWith("Windows") ||
                   System.getProperty("java.vm.name").startsWith("OpenJDK"))
                {
                    //
                    // Windows does not allow binding to the mcast address itself
                    // so we bind to INADDR_ANY (0.0.0.0) instead. As a result,
                    // bi-directional connection won't work because the source
                    // address won't be the multicast address and the client will
                    // therefore reject the datagram.
                    //
                    int protocol =
                        _mcastAddr.getAddress().getAddress().length == 4 ? Network.EnableIPv4 : Network.EnableIPv6;
                    _addr = Network.getAddressForServer("", port, protocol, instance.preferIPv6());
                }
                _addr = Network.doBind(_fd, _addr);
                configureMulticast(_mcastAddr, mcastInterface, -1);

                if(port == 0)
                {
                    _mcastAddr = new java.net.InetSocketAddress(_mcastAddr.getAddress(), _addr.getPort());
                }
            }
            else
            {
                if(!System.getProperty("os.name").startsWith("Windows"))
                {
                    //
                    // Enable SO_REUSEADDR on Unix platforms to allow
                    // re-using the socket even if it's in the TIME_WAIT
                    // state. On Windows, this doesn't appear to be
                    // necessary and enabling SO_REUSEADDR would actually
                    // not be a good thing since it allows a second
                    // process to bind to an address even it's already
                    // bound by another process.
                    //
                    // TODO: using SO_EXCLUSIVEADDRUSE on Windows would
                    // probably be better but it's only supported by recent
                    // Windows versions (XP SP2, Windows Server 2003).
                    //
                    Network.setReuseAddress(_fd, true);
                }
                _addr = Network.doBind(_fd, _addr);
            }

            if(_instance.traceLevel() >= 1)
            {
                StringBuffer s = new StringBuffer("starting to receive " + _instance.protocol() + " packets\n");
                s.append(toString());

                java.util.List<String> interfaces =
                    Network.getHostsForEndpointExpand(_addr.getAddress().getHostAddress(), instance.protocolSupport(),
                                                      true);
                if(!interfaces.isEmpty())
                {
                    s.append("\nlocal interfaces: ");
                    s.append(IceUtilInternal.StringUtil.joinString(interfaces, ", "));
                }
                _instance.logger().trace(_instance.traceCategory(), s.toString());
            }
        }
        catch(Ice.LocalException ex)
        {
            _fd = null;
            throw ex;
        }
    }

    private synchronized void setBufSize(Ice.Properties properties)
    {
        assert(_fd != null);

        for(int i = 0; i < 2; ++i)
        {
            String direction;
            String prop;
            int dfltSize;
            if(i == 0)
            {
                direction = "receive";
                prop = "Ice.UDP.RcvSize";
                dfltSize = Network.getRecvBufferSize(_fd);
                _rcvSize = dfltSize;
            }
            else
            {
                direction = "send";
                prop = "Ice.UDP.SndSize";
                dfltSize = Network.getSendBufferSize(_fd);
                _sndSize = dfltSize;
            }

            //
            // Get property for buffer size and check for sanity.
            //
            int sizeRequested = properties.getPropertyAsIntWithDefault(prop, dfltSize);
            if(sizeRequested < (_udpOverhead + IceInternal.Protocol.headerSize))
            {
                _instance.logger().warning("Invalid " + prop + " value of " + sizeRequested + " adjusted to " +
                    dfltSize);
                sizeRequested = dfltSize;
            }

            if(sizeRequested != dfltSize)
            {
                //
                // Try to set the buffer size. The kernel will silently adjust
                // the size to an acceptable value. Then read the size back to
                // get the size that was actually set.
                //
                int sizeSet;
                if(i == 0)
                {
                    Network.setRecvBufferSize(_fd, sizeRequested);
                    _rcvSize = Network.getRecvBufferSize(_fd);
                    sizeSet = _rcvSize;
                }
                else
                {
                    Network.setSendBufferSize(_fd, sizeRequested);
                    _sndSize = Network.getSendBufferSize(_fd);
                    sizeSet = _sndSize;
                }

                //
                // Warn if the size that was set is less than the requested size.
                //
                if(sizeSet < sizeRequested)
                {
                    _instance.logger().warning("UDP " + direction + " buffer size: requested size of "
                                    + sizeRequested + " adjusted to " + sizeSet);
                }
            }
        }
    }

    private void configureMulticast(java.net.InetSocketAddress group, String interfaceAddr, int ttl)
    {
        try
        {
            java.net.NetworkInterface intf = null;

            if(interfaceAddr.length() != 0)
            {
                intf = java.net.NetworkInterface.getByName(interfaceAddr);
                if(intf == null)
                {
                    try
                    {
                        intf = java.net.NetworkInterface.getByInetAddress(
                            java.net.InetAddress.getByName(interfaceAddr));
                    }
                    catch(Exception ex)
                    {
                    }
                }
            }

            if(group != null)
            {
                //
                // Join multicast group.
                //
                boolean join = false;
                if(intf != null)
                {
                    _fd.join(group.getAddress(), intf);
                    join = true;
                }
                else
                {
                    //
                    // If the user doesn't specify an interface, we join to the multicast group with every
                    // interface that supports multicast and has a configured address with the same protocol
                    // as the group address protocol.
                    //
                    int protocol = group.getAddress().getAddress().length == 4 ? Network.EnableIPv4 :
                                                                                 Network.EnableIPv6;

                    java.util.List<java.net.NetworkInterface> interfaces =
                                java.util.Collections.list(java.net.NetworkInterface.getNetworkInterfaces());
                    for(java.net.NetworkInterface iface : interfaces)
                    {
                        if(!iface.supportsMulticast())
                        {
                            continue;
                        }

                        boolean hasProtocolAddress = false;
                        java.util.List<java.net.InetAddress> addresses =
                            java.util.Collections.list(iface.getInetAddresses());
                        for(java.net.InetAddress address : addresses)
                        {
                            if(address.getAddress().length == 4 && protocol == Network.EnableIPv4 ||
                               address.getAddress().length != 4 && protocol == Network.EnableIPv6)
                            {
                                hasProtocolAddress = true;
                                break;
                            }
                        }

                        if(hasProtocolAddress)
                        {
                            _fd.join(group.getAddress(), iface);
                            join = true;
                        }
                    }

                    if(!join)
                    {
                        throw new Ice.SocketException(new IllegalArgumentException(
                                                    "There aren't any interfaces that support multicast, " +
                                                    "or the interfaces that support it\n" +
                                                    "are not configured for the group protocol. " +
                                                    "Cannot join the mulitcast group."));
                    }
                }
            }
            else if(intf != null)
            {
                //
                // Otherwise, set the multicast interface if specified.
                //
                _fd.setOption(java.net.StandardSocketOptions.IP_MULTICAST_IF, intf);
            }

            if(ttl != -1)
            {
                _fd.setOption(java.net.StandardSocketOptions.IP_MULTICAST_TTL, ttl);
            }
        }
        catch(Exception ex)
        {
            throw new Ice.SocketException(ex);
        }
    }

    @Override
    protected synchronized void finalize()
        throws Throwable
    {
        try
        {
            IceUtilInternal.Assert.FinalizerAssert(_fd == null);
        }
        catch(java.lang.Exception ex)
        {
        }
        finally
        {
            super.finalize();
        }
    }

    private ProtocolInstance _instance;

    private int _state;
    private int _rcvSize;
    private int _sndSize;
    private java.nio.channels.DatagramChannel _fd;
    private java.net.InetSocketAddress _addr;
    private java.net.InetSocketAddress _mcastAddr = null;
    private java.net.InetSocketAddress _peerAddr = null;

    //
    // The maximum IP datagram size is 65535. Subtract 20 bytes for the IP header and 8 bytes for the UDP header
    // to get the maximum payload.
    //
    private final static int _udpOverhead = 20 + 8;
    private final static int _maxPacketSize = 65535 - _udpOverhead;

    private static final int StateNeedConnect = 0;
    private static final int StateConnected = 1;
    private static final int StateNotConnected = 2;
}
