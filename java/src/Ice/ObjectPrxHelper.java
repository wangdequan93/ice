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


package Ice;

public class ObjectPrxHelper extends ObjectPrxHelperBase
{
    public static ObjectPrx
    checkedCast(Ice.ObjectPrx b)
    {
	return b;
    }

    public static ObjectPrx
    checkedCast(Ice.ObjectPrx b, String f)
    {
	ObjectPrx d = null;
	if(b != null)
	{
	    Ice.ObjectPrx bb = b.ice_newFacet(f);
	    try
	    {
		boolean ok = bb.ice_isA("::Object");
		assert(ok);
		ObjectPrxHelper h = new ObjectPrxHelper();
		h.__copyFrom(bb);
		d = h;
	    }
	    catch(Ice.FacetNotExistException ex)
	    {
	    }
	}
	return d;
    }

    public static ObjectPrx
    uncheckedCast(Ice.ObjectPrx b)
    {
	return b;
    }

    public static ObjectPrx
    uncheckedCast(Ice.ObjectPrx b, String f)
    {
	ObjectPrx d = null;
	if(b != null)
	{
	    Ice.ObjectPrx bb = b.ice_newFacet(f);
	    ObjectPrxHelper h = new ObjectPrxHelper();
	    h.__copyFrom(bb);
	    d = h;
	}
	return d;
    }
}
