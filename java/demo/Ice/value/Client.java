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


public class Client
{
    private static void
    readline(java.io.BufferedReader in)
    {
        try
        {
            in.readLine();
        }
        catch(java.io.IOException ex)
        {
            ex.printStackTrace();
        }
    }

    private static int
    run(String[] args, Ice.Communicator communicator)
    {
        Ice.Properties properties = communicator.getProperties();
        final String refProperty = "Value.Initial";
        String ref = properties.getProperty(refProperty);
        if(ref.length() == 0)
        {
            System.err.println("property `" + refProperty + "' not set");
            return 1;
        }

        Ice.ObjectPrx base = communicator.stringToProxy(ref);
        InitialPrx initial = InitialPrxHelper.checkedCast(base);
        if(initial == null)
        {
            System.err.println("invalid object reference");
            return 1;
        }

        java.io.BufferedReader in = new java.io.BufferedReader(new java.io.InputStreamReader(System.in));

        System.out.println();
        System.out.println("Let's first transfer a simple object, for a class without");
        System.out.println("operations, and print its contents. No factory is required");
        System.out.println("for this.");
        System.out.println("[press enter]");
        readline(in);

        Simple simple = initial.getSimple();
        System.out.println("==> " + simple.message);

        System.out.println();
        System.out.println("Ok, this worked. Now let's try to transfer an object for a class");
        System.out.println("with operations as type Ice.Object. Because no factory is installed,");
        System.out.println("the class will be sliced to Ice.Object.");
        System.out.println("[press enter]");
        readline(in);

	Ice.Object obj = initial.getPrinterAsObject();
	System.out.println("The type ID of the received object is \"" + obj.ice_id(null) + "\"");
	assert(obj.ice_id(null).equals("::Ice::Object"));

        System.out.println();
        System.out.println("Yes, this worked. Now let's try to transfer an object for a class");
        System.out.println("with operations as type Printer, without installing a factory first.");
        System.out.println("This should give us a `no factory' exception.");
        System.out.println("[press enter]");
        readline(in);

        PrinterHolder printer = new PrinterHolder();
        PrinterPrxHolder printerProxy = new PrinterPrxHolder();
	boolean gotException = false;
        try
        {
            initial.getPrinter(printer, printerProxy);
        }
        catch(Ice.NoObjectFactoryException ex)
        {
            System.out.println("==> " + ex);
	    gotException = true;
        }
	assert(gotException);

        System.out.println();
        System.out.println("Yep, that's what we expected. Now let's try again, but with");
        System.out.println("installing an appropriate factory first. If successful, we print");
        System.out.println("the object's content.");
        System.out.println("[press enter]");
        readline(in);

        Ice.ObjectFactory factory = new ObjectFactory();
        communicator.addObjectFactory(factory, "::Printer");

        initial.getPrinter(printer, printerProxy);
        System.out.println("==> " + printer.value.message);

        System.out.println();
        System.out.println("Cool, it worked! Let's try calling the printBackwards() method");
        System.out.println("on the object we just received locally.");
        System.out.println("[press enter]");
        readline(in);

        System.out.print("==> ");
        printer.value.printBackwards(null);

        System.out.println();
        System.out.println("Now we call the same method, but on the remote object. Watch the");
        System.out.println("server's output.");
        System.out.println("press enter]");
        readline(in);

        printerProxy.value.printBackwards();

        System.out.println();
        System.out.println("Next, we transfer a derived object from the server as a base");
        System.out.println("object. Since we haven't yet installed a factory for the derived");
        System.out.println("class, the derived class (DerivedPrinter) is sliced");
        System.out.println("to its base class (Printer).");
        System.out.println("[press enter]");
        readline(in);

        Printer derivedAsBase = initial.getDerivedPrinter();
	System.out.println("The type ID of the received object is \"" + derivedAsBase.ice_id(null) + "\"");
	assert(derivedAsBase.ice_id(null).equals("::Printer"));

        System.out.println();
        System.out.println("Now we install a factory for the derived class, and try again.");
        System.out.println("Because we receive the derived object as a base object,");
	System.out.println("we need to do a class cast to get from the base to the derived object.");
        System.out.println("[press enter]");
        readline(in);

        communicator.addObjectFactory(factory, "::DerivedPrinter");

        derivedAsBase = initial.getDerivedPrinter();
        DerivedPrinter derived = (DerivedPrinter)derivedAsBase;

        System.out.println("==> class cast to derived object succeded");
	System.out.println("The type ID of the received object is \"" + derived.ice_id(null) + "\"");

        System.out.println();
        System.out.println("Let's print the message contained in the derived object, and");
        System.out.println("call the operation printUppercase() on the derived object");
        System.out.println("locally.");
        System.out.println("[press enter]");
        readline(in);

        System.out.println("==> " + derived.derivedMessage);
        System.out.print("==> ");
        derived.printUppercase(null);

        System.out.println();
        System.out.println("Finally, we try the same again, but instead of returning the");
        System.out.println("derived object, we throw an exception containing the derived");
        System.out.println("object.");
        System.out.println("[press enter]");
        readline(in);

        try
        {
            initial.throwDerivedPrinter();
        }
        catch(DerivedPrinterException ex)
        {
            derived = ex.derived;
            assert(derived != null);
        }

        System.out.println("==> " + derived.derivedMessage);
        System.out.print("==> ");
        derived.printUppercase(null);

        System.out.println();
        System.out.println("That's it for this demo. Have fun with Ice!");

        return 0;
    }

    public static void
    main(String[] args)
    {
        int status = 0;
        Ice.Communicator communicator = null;

        try
        {
            Ice.Properties properties = Ice.Util.createProperties(args);
            properties.load("config");
            communicator = Ice.Util.initializeWithProperties(args, properties);
            status = run(args, communicator);
        }
        catch(Ice.LocalException ex)
        {
            ex.printStackTrace();
            status = 1;
        } finally {
	    if(communicator != null)
	    {
                communicator.destroy();
            }
	}

        System.exit(status);
    }
}
