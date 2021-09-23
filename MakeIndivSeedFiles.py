import numpy as np

for i in range(10): # modify based on number of IndivSeedFiles you want
    numbers = range(0,10000000)
    seeds = np.random.choice(numbers, size = 1000000, replace = False)
    np.savetxt("IndivSeedFile{}".format(i), seeds, fmt='%09d')
