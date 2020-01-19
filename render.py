#!/usr/bin/env python
# coding: utf-8

import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.animation as animation
import subprocess
from tqdm import tqdm
from matplotlib.animation import FFMpegWriter

parser = argparse.ArgumentParser()
parser.add_argument('--particles', required=True)
parser.add_argument('--engine', required=True)
parser.add_argument('--output', default='movie.mp4')
parser.add_argument('--speed', default=300)
parser.add_argument('--duration', default=60, type=int)
parser.add_argument('--bitrate', default=20000, type=int)

args = parser.parse_args()

def run_simulation():
    ps = subprocess.Popen([args.engine,  args.particles, str(int(float(args.speed)))], stdout=subprocess.PIPE)
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

nfr = args.duration * 50 # Number of frames
fps = 100 # Frame per sec

ps = []
bs = []
ts = []
ss = np.arange(1,nfr,0.5)
i = 0


fig = plt.figure(figsize=(14, 14), dpi=150)
ax = fig.add_subplot(221, projection='3d')
sct, = ax.plot([], [], [], "o", markersize=2)

ax2 = fig.add_subplot(222)
hst = ax2.hist([], density=True)

ax3 = fig.add_subplot(223)
pres_plt, = ax3.plot([], [])

ax4 = fig.add_subplot(224)
tmp_plt, = ax4.plot([], [])

progress = tqdm(total=nfr * 2 - 3)

last_frm = [-1, ]

def update(ifrm, last_frm):
    if ifrm > last_frm[0]:
        bumps, pressure = step()
        bs.append(bumps)
        ps.append(pressure)
        temp = (np.nanmean(V ** 2) * mass_of_molecule / 3.0) / k
        ts.append(temp)
        last_frm[0] += 1
    progress.n = ifrm
    progress.refresh()
    sct.set_data(X, Y)
    sct.set_3d_properties(Z)
    ax.set_title('{} столкновений'.format(bs[ifrm]))

    ax2.clear()
    ax2.hist(V, density=True, bins=np.linspace(0, 2 * float(args.speed), num=30), alpha=0.75)
    ax2.set_title('Среднеквадратичная скорость: {} м/с'.format(str(round(np.sqrt(np.nanmean(V ** 2)), 2))))

    timeline = np.linspace(0, float(ifrm + 1) / 100, ifrm + 1)
    pres_plt.set_data(timeline, ps[0: ifrm + 1])
    ax3.set_xlim(0, float(ifrm + 1) / 100)
    ax3.set_ylim(np.nanmean(ps[0: ifrm + 1]) / 2, np.nanmean(ps[0: ifrm + 1]) + np.nanmean(ps[0: ifrm + 1]) / 2)
    ax3.set_title('Давление: {} Па'.format(str(ps[ifrm])))

    tmp_plt.set_data(timeline, ts[0: ifrm + 1])
    ax4.set_xlim(0, float(ifrm + 1) / 100)
    ax4.set_ylim(np.nanmean(ts[0: ifrm + 1]) / 2, np.nanmean(ts[0: ifrm + 1]) + np.nanmean(ts[0: ifrm + 1]) / 2)
    ax4.set_title('Температура: {}K'.format(str(ts[ifrm])))


ax.set_xlim(-0.5, 0.5)
ax.set_ylim(-0.5, 0.5)
ax.set_zlim(-0.5, 0.5)
ax3.set_xlabel('Время, с')
ax4.set_xlabel('Время, с')
ax3.set_ylabel('Давление, Па')
ax4.set_ylabel('Температура, К')
ani = animation.FuncAnimation(fig, update, nfr * 2 - 3, fargs=(last_frm,), interval=1000/fps)


writer = FFMpegWriter(fps=fps, metadata=dict(artist='nature'), bitrate=args.bitrate)
ani.save(args.output, writer=writer)
progress.close()
