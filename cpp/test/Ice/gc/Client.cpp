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


#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include <TestCommon.h>
#include <Test.h>
#include <fstream>

using namespace std;

static int num = 0;

struct N : public C
{
    N()
    {
	++num;
    }

    ~N()
    {
	--num;
    }
};

typedef ::IceInternal::Handle<N> NPtr;

struct N2 : public C2
{
    N2()
    {
	++num;
    }

    ~N2()
    {
	--num;
    }
};

typedef ::IceInternal::Handle<N2> N2Ptr;

class GarbageProducer : public ::IceUtil::Thread, public ::IceUtil::Monitor< ::IceUtil::Mutex>
{
public:

    GarbageProducer()
    {
	_stop = false;
    }

    virtual void run()
    {
	::IceUtil::ThreadControl t = getThreadControl();

	while(true)
	{
	    {
		::IceUtil::Monitor< ::IceUtil::Mutex>::Lock sync(*this);
		if(_stop)
		{
		    return;
		}
	    }

	    t.yield();

	    {
		NPtr n = new N;
		n->left = n;
	    }

	    {
		NPtr n = new N;
		n->left = n;
		n->right = n;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		n1->left = n2;
		n2->left = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		n1->left = n2;
		n2->left = n1;
		n1->right = n2;
		n2->right = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		n1->left = n2;
		n2->left = n1;
		n1->right = n1;
		n2->right = n2;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		n1->left = n2;
		n2->left = n3;
		n3->left = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		n1->left = n2;
		n2->left = n3;
		n3->left = n1;
		n1->right = n2;
		n2->right = n3;
		n3->right = n1;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		n1->left = n2;
		n2->left = n3;
		n3->left = n1;
		n1->right = n3;
		n2->right = n1;
		n3->right = n2;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		NPtr n4 = new N;
		n1->left = n2;
		n2->left = n1;
		n2->right = n3;
		n3->left = n4;
	    }

	    {
		NPtr n1 = new N;
		NPtr n2 = new N;
		NPtr n3 = new N;
		NPtr n4 = new N;
		NPtr n5 = new N;
		NPtr n6 = new N;
		n1->left = n2;
		n2->left = n1;
		n2->right = n3;
		n3->left = n4;
		n4->right = n5;
		n5->right = n6;
		n6->right = n5;
	    }
	}
    }

    void stop()
    {
	{
	    ::IceUtil::Monitor< ::IceUtil::Mutex>::Lock sync(*this);
	    _stop = true;
	    notify();
	}
	getThreadControl().join();
    }

    void randomWait()
    {
	::IceUtil::Time waitTime = ::IceUtil::Time::milliSeconds(10 + rand() % 50);
	::IceUtil::Monitor< ::IceUtil::Mutex>::Lock sync(*this);
	timedWait(waitTime);
    }

private:

    bool _stop;
};

class MyApplication : public Ice::Application
{
public:

    virtual int run(int, char* []);
};

int
MyApplication::run(int argc, char* argv[])
{
    cout << "testing single instance... " << flush;
    {
	NPtr n = new N;
	test(num == 1);
	Ice::collectGarbage();
	test(num == 1);
    }
    test(num == 0);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing single instance cycle... " << flush;
    {
	NPtr n = new N;
	n->left = n;
	test(num == 1);
	Ice::collectGarbage();
	test(num == 1);
    }
    test(num == 1);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing single instance cycle with double pointers... " << flush;
    {
	NPtr n = new N;
	n->left = n;
	n->right = n;
	test(num == 1);
	Ice::collectGarbage();
	test(num == 1);
    }
    test(num == 1);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing double instance cycle... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	n1->left = n2;
	n2->left = n1;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing double instance cycle with double pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	n1->left = n2;
	n2->left = n1;
	n1->right = n2;
	n2->right = n1;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing double instance cycle with looped pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	n1->left = n2;
	n2->left = n1;
	n1->right = n1;
	n2->right = n2;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing triple instance cycle... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	n1->left = n2;
	n2->left = n3;
	n3->left = n1;
	test(num == 3);
	Ice::collectGarbage();
	test(num == 3);
    }
    test(num == 3);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing triple instance cycle with double pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	n1->left = n2;
	n2->left = n3;
	n3->left = n1;
	n1->right = n2;
	n2->right = n3;
	n3->right = n1;
	test(num == 3);
	Ice::collectGarbage();
	test(num == 3);
    }
    test(num == 3);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing triple instance cycle with opposing pointers... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	n1->left = n2;
	n2->left = n3;
	n3->left = n1;
	n1->right = n3;
	n2->right = n1;
	n3->right = n2;
	test(num == 3);
	Ice::collectGarbage();
	test(num == 3);
    }
    test(num == 3);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing cycle with trailing instances... " << flush;
    NPtr n;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	NPtr n4 = new N;
	n1->left = n2;
	n2->left = n1;
	n2->right = n3;
	n3->left = n4;
	n = n3;
	test(num == 4);
	Ice::collectGarbage();
	test(num == 4);
    }
    test(num == 4);
    Ice::collectGarbage();
    test(num == 2);
    n = 0;
    test(num == 0);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing cycle with trailing instances and trailing cycle... " << flush;
    {
	NPtr n1 = new N;
	NPtr n2 = new N;
	NPtr n3 = new N;
	NPtr n4 = new N;
	NPtr n5 = new N;
	NPtr n6 = new N;
	n1->left = n2;
	n2->left = n1;
	n2->right = n3;
	n3->left = n4;
	n4->right = n5;
	n5->right = n6;
	n6->right = n5;
	n = n4;
	test(num == 6);
	Ice::collectGarbage();
	test(num == 6);
    }
    test(num == 6);
    Ice::collectGarbage();
    test(num == 3);
    n = 0;
    test(num == 2);
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing sequence element cycle... " << flush;
    {
	CSeq cs;
	cs.push_back(new N);
	cs.push_back(new N);
	cs[0]->left = cs[1];
	cs[1]->left = cs[0];
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing dictionary element cycle... " << flush;
    {
	CDict cd;
	NPtr n1 = new N;
	NPtr n2 = new N;
	n1->left = n2;
	n2->left = n1;
	cd[0] = n1;
	cd[1] = n2;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing sequence of struct cycle... " << flush;
    {
	SSeq ss;
	S s;

	ss.push_back(s);
	ss.push_back(s);
	ss[0].theC = new N;
	ss[1].theC = new N;
	ss[0].theC->left = ss[1].theC;
	ss[1].theC->left = ss[0].theC;
	test(num == 2);
	Ice::collectGarbage();
	test(num == 2);
    }
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing sequence of struct of dictionary cycle... " << flush;
    {
	N2Ptr n2 = new N2;
	S2 s2;

	n2->theS2Seq.push_back(s2);
	n2->theS2Seq.push_back(s2);
	n2->theS2Seq[0].theC2Dict[0] = n2;
	n2->theS2Seq[0].theC2Dict[1] = n2;
	n2->theS2Seq[1].theC2Dict[0] = n2;
	n2->theS2Seq[1].theC2Dict[1] = n2;
	test(num == 1);
	Ice::collectGarbage();
	test(num == 1);
    }
    Ice::collectGarbage();
    test(num == 0);
    cout << "ok" << endl;

    cout << "testing for race conditions... " << flush;
    ::IceUtil::Time t = ::IceUtil::Time::now();
    int seed = argc > 1 ? atoi(argv[1]) : static_cast<int>(t.toMilliSeconds());
    ofstream file("seed");
    file << seed << "\n";
    srand(seed);

    typedef ::IceUtil::Handle<GarbageProducer> GarbageThreadPtr;
    GarbageThreadPtr garbageThread = new GarbageProducer();
    garbageThread->start();

    for(int i = 0; i < 50; ++i)
    {
	if(interrupted())
	{
	    break;
	}
	garbageThread->randomWait();
	Ice::collectGarbage();
    }

    garbageThread->stop();
    Ice::collectGarbage();
    if(!interrupted())
    {
	test(num == 0);
	cout << "ok" << endl;
	return EXIT_SUCCESS;
    }
    else
    {
	return 130; // SIGINT + 128
    }
}

int
main(int argc, char* argv[])
{
    MyApplication app;
    return app.main(argc, argv);
}
