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


public final class ThroughputI extends _ThroughputDisp
{
    public
    ThroughputI()
    {
        _seq = new byte[seqSize.value];
    }

    public byte[]
    echoByteSeq(byte[] seq, Ice.Current current)
    {
        return seq;
    }

    public byte[]
    recvByteSeq(Ice.Current current)
    {
        return _seq;
    }

    public void
    sendByteSeq(byte[] seq, Ice.Current current)
    {
    }

    private byte[] _seq;
}
