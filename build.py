#! /usr/bin/env python

import os, sys
from os.path import join

ROOT = os.path.dirname(os.path.realpath(__file__))
SCONSDIR = join(ROOT, 'tools', 'scons')

if __name__ == '__main__':
    sys.path.insert(0, join(SCONSDIR, 'engine'))
    import SCons.Script
    SCons.Script.main()
