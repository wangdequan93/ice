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


#ifndef PHONE_BOOK_ICE
#define PHONE_BOOK_ICE

#include <Ice/Identity.ice>

exception DatabaseException
{
    string message;
};

class Contact
{
    nonmutating string getName();
    void setName(string name) throws DatabaseException;

    nonmutating string getAddress();
    void setAddress(string address);

    nonmutating string getPhone();
    void setPhone(string phone);

    void destroy() throws DatabaseException;

    string name;
    string address;
    string phone;
};

sequence<Contact*> Contacts;

interface PhoneBook
{
    Contact* createContact() throws DatabaseException;
    nonmutating Contacts findContacts(string name) throws DatabaseException;
    void setEvictorSize(int size) throws DatabaseException;
    nonmutating void shutdown();
};

#endif
