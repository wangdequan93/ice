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


sequence<Foo> FooSequence;
dictionary<Foo, Bar> FooBarDictionary;
interface BarIntf extends Foo { void op(); };
class BarClass1 extends Foo { long l; };
class BarClass2 implements Foo1, Foo2, Foo3 { long l; };
class BarClass3 extends Foo implements Foo1, Foo2, Foo3 { long l; };
