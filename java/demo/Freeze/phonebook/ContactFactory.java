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


class ContactFactory extends Ice.LocalObjectImpl implements Ice.ObjectFactory
{
    public Ice.Object
    create(String type)
    {
	assert(type.equals("::Contact"));
	return new ContactI(this);
    }

    public void
    destroy()
    {
    }

    ContactFactory()
    {
    }

    void
    setEvictor(Freeze.Evictor evictor)
    {
	_evictor = evictor;
    }

    Freeze.Evictor
    getEvictor()
    {
	return _evictor;
    }

    private Freeze.Evictor _evictor;
}
