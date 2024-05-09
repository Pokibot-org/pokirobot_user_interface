CLK = 1000000
TARGET = 192000*256

Nmin = 50
Nmax = 432
Qmin = 2
Qmax = 15
Rmin = 1
Rmax = 32

dmin = 999999999
config = (0, 0, 0)

for n in range(Nmin, Nmax+1):
    for q in range(Qmin, Qmax+1):
        for r in range(Rmin, Rmax+1):
            if CLK*n >= 100000000:
                f = ((CLK * n) / q) / r
                d = abs(f-TARGET)
                if d < dmin:
                    dmin = d
                    config = (n, q, r)
print(config)
