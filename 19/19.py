#!/usr/bin/env python3

import datetime
import re
import sys

p = re.compile('(\d\d\d\d-\d\d-\d\d).(\d\d:\d\d:\d\d)')

for l in sys.stdin:
    a, d, t, z = p.split(l)
    T = d + " " + t
    dt = datetime.datetime.strptime(T, "%Y-%m-%d %H:%M:%S")
    ts = str(dt.timestamp())

    sys.stdout.write("".join((a, ts, z)))
