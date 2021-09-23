import numpy as np

numbers = range(0,10000000)
seeds = np.random.choice(numbers, size = 1000000, replace = False)
np.savetxt("TopologySeedFile", seeds, fmt='%09d')
