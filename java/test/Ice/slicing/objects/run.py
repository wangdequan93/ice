#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003
# ZeroC, Inc.
# Billerica, MA, USA
#
# All Rights Reserved.
#
# Ice is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License version 2 as published by
# the Free Software Foundation.
#
# **********************************************************************

import os, sys

for toplevel in [".", "..", "../..", "../../..", "../../../.."]:
    toplevel = os.path.normpath(toplevel)
    if os.path.exists(os.path.join(toplevel, "config", "TestUtil.py")):
        break
else:
    raise "can't find toplevel directory!"

sys.path.append(os.path.join(toplevel, "config"))
import TestUtil

name = os.path.join("Ice", "slicing", "objects")
testdir = os.path.join(toplevel, "test", name)
nameAMD = os.path.join("Ice", "slicing", "objectsAMD")
testdirAMD = os.path.join(toplevel, "test", nameAMD)

print "tests with regular server."
TestUtil.clientServerTestWithClasspath(os.path.join(testdir, "sclasses"), os.path.join(testdir, "cclasses"))

print "tests with AMD server."
TestUtil.clientServerTestWithClasspath(os.path.join(testdirAMD, "classes"), os.path.join(testdir, "cclasses"))

sys.exit(0)
