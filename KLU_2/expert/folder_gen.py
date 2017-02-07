import numpy as np
import os
import sys

step = float(sys.argv[1])
p = np.arange(0.0, 1.0 + step, step)
for i in range(0, len(p) - 1):
	name = '{0:.2f}'.format(p[i]) + '-' + '{0:.2f}'.format(p[i + 1])
	os.mkdir(name)
