#!/usr/bin/env python
# coding: utf-8

# In[2]:


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.animation as animation
import subprocess
from tqdm import tqdm
from matplotlib.animation import FFMpegWriter

def run_simulation():
    ps = subprocess.Popen('/Users/nikita/CLionProjects/physicsSimulation/cmake-build-debug/physics_simulation', stdout=subprocess.PIPE)
    for c in iter(lambda: ps.stdout.readline(), b''):  # replace '' with b'' for Python 3
        yield float(c.decode('ascii'))


sim_res = run_simulation()
atom_cnt = int(next(sim_res))
mass_of_molecule = next(sim_res)
k = next(sim_res)

X = np.zeros(atom_cnt)
Y = np.zeros(atom_cnt)
Z = np.zeros(atom_cnt)
V = np.zeros(atom_cnt)



def step():
    for i in range(atom_cnt):
        x = next(sim_res)
        y = next(sim_res)
        z = next(sim_res)
        v_x = next(sim_res)
        v_y = next(sim_res)
        v_z = next(sim_res)
        a_x = next(sim_res)
        a_y = next(sim_res)
        a_z = next(sim_res)

        X[i] = x
        Y[i] = y
        Z[i] = z
        V[i] = np.sqrt(v_x ** 2 + v_y ** 2 + v_z ** 2)
    bumps = next(sim_res)
    pressure = next(sim_res)
    return bumps, pressure

nfr = 3000 # Number of frames
fps = 100 # Frame per sec
xs = []
ys = []
zs = []
vs = []
ps = []
bs = []

ss = np.arange(1,nfr,0.5)
i = 0
for s in tqdm(ss):
    #print(i)
    i += 1
    bumps, pressure = step()
    xs.append(np.copy(X))
    ys.append(np.copy(Y))
    zs.append(np.copy(Z))
    vs.append(np.copy(V))
    bs.append(bumps)
    ps.append(pressure)

ts = np.zeros(len(vs))
for i in range(len(vs)):
    ts[i] = (np.nanmean(vs[i] ** 2) * mass_of_molecule / 3.0) / k


fig = plt.figure(figsize=(20, 20), dpi=150)
ax = fig.add_subplot(221, projection='3d')
sct, = ax.plot([], [], [], "o", markersize=2)

ax2 = fig.add_subplot(222)
hst = ax2.hist([], density=True)

ax3 = fig.add_subplot(223)
pres_plt, = ax3.plot([], [])

ax4 = fig.add_subplot(224)
tmp_plt, = ax4.plot([], [])

def update(ifrm, xa, ya, za, va, ba, pa, ta):
    print(ifrm)
    sct.set_data(xa[ifrm], ya[ifrm])
    sct.set_3d_properties(za[ifrm])
    ax.set_title('{} столкновений'.format(ba[ifrm]))

    ax2.clear()
    ax2.hist(va[ifrm], density=True, bins=np.linspace(0, 1000, num=30), alpha=0.75)
    ax2.set_title('Среднеквадратичная скорость: {} м/с'.format(str(round(np.sqrt(np.nanmean(va[ifrm] ** 2)), 2))))

    timeline = np.linspace(0, float(ifrm + 1) / 100, ifrm + 1)
    pres_plt.set_data(timeline, pa[0: ifrm + 1])
    ax3.set_xlim(0, float(ifrm + 1) / 100)
    ax3.set_ylim(np.mean(pa[0: ifrm + 1]) / 2, np.mean(pa[0: ifrm + 1]) + np.mean(pa[0: ifrm + 1]) / 2)
    ax3.set_title('Давление: {} Па'.format(str(pa[ifrm])))

    tmp_plt.set_data(timeline, ta[0: ifrm + 1])
    ax4.set_xlim(0, float(ifrm + 1) / 100)
    ax4.set_ylim(np.mean(ta[0: ifrm + 1]) / 2, np.mean(ta[0: ifrm + 1]) + np.mean(ta[0: ifrm + 1]) / 2)
    ax4.set_title('Температура: {}K'.format(str(ta[ifrm])))


ax.set_xlim(-0.5, 0.5)
ax.set_ylim(-0.5, 0.5)
ax.set_zlim(-0.5, 0.5)
ax3.set_xlabel('Время, с')
ax4.set_xlabel('Время, с')
ax3.set_ylabel('Давление, Па')
ax4.set_ylabel('Температура, К')
ani = animation.FuncAnimation(fig, update, nfr * 2 - 3, fargs=(xs, ys, zs, vs, bs, ps, ts), interval=1000/fps)


writer = FFMpegWriter(fps=fps, metadata=dict(artist='Me'), bitrate=20000)
ani.save("movie.mp4", writer=writer)
