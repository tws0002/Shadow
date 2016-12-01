#!/usr/bin/python
# -*- coding: utf-8 -*-
"""

"""

import os
import sys

ARNOLD_DIR = 'D:\\RD\\Arnold-4.2.15.1-windows'

os.environ['PATH'] += (';' + os.path.join(ARNOLD_DIR, 'bin'))
sys.path.append(os.path.join(ARNOLD_DIR, 'python'))
ARNOLD_PLUGIN_PATH = os.path.join(ARNOLD_DIR, "plugin")

