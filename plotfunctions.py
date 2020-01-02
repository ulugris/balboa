from matplotlib import pyplot as plt
from numpy import degrees, sin, cos

fg1 = plt.figure()
ax1 = fg1.add_axes([0.1, 0.1, 0.85, 0.85])

whl = plt.Circle((0, 0), radius=1, lw=3, ec='tab:red', fill=False)
dts = plt.Line2D((0, 0), (0, 1), ls='', color='black', marker=".", ms=10)
chs = plt.Line2D((0, 0), (0, 0), lw=3)


def plot_response(T, X, Xo=None):
    fig = plt.figure()

    aa = fig.add_axes([0, 0, 1, 1])
    aa.plot([0, T[-1]], [0, 0], lw=1, alpha=0.25, color='tab:orange')
    if Xo is not None:
        aa.plot(T, [degrees(a) for a in Xo[0]], color='tab:orange', ls=':')
    aa.plot(T, [degrees(a) for a in X[0]], color='tab:orange')        
    aa.set_ylabel(r'$\theta$ (deg)', color='tab:orange')

    aa.set_xlabel('Tiempo (s)')
    aa.set_xlim([0, T[-1]])

    ax = aa.twinx()
    ax.plot([0, T[-1]], [0, 0], lw=1, alpha=0.25, color='tab:blue')
    if Xo is not None:
        ax.plot(T, Xo[2], color='tab:blue', ls=':')
    ax.plot(T, X[2], color='tab:blue')
    ax.set_ylabel(r'$\dot x$ (m/s)', color='tab:blue')

    plt.show()


def plot_voltage(T, V):
    fig = plt.figure()

    ax = fig.add_axes([0, 0, 1, 1])

    ax.plot(T, V, color='tab:blue')
    ax.set_ylabel(r'$V$ (V)')

    ax.set_xlabel('Tiempo (s)')
    ax.set_xlim([0, T[-1]])

    plt.show()


def init():
    plt.rcParams['animation.html'] = 'html5'

    ax1.plot((-0.1, 0.55), (0, 0), color='black', lw=1)

    ax1.axis('scaled')
    ax1.set_xlim((-0.1, 0.55))
    ax1.set_ylim((-0.025, 0.30))

    ax1.add_patch(whl)
    ax1.add_line(chs)
    ax1.add_line(dts)

    return whl, dts, chs


def anim(i, Y, dt, r, l):
    if i == 0:
        anim.x = 0
    else:
        anim.x += 0.5*(Y[1, i] + Y[1, i-1])*dt

    whl.set_radius(r)
    whl.set_center((anim.x, r))

    dts.set_xdata((anim.x, anim.x - r*sin(anim.x/r)))
    dts.set_ydata((r, r - r*cos(anim.x/r)))

    chs.set_xdata((anim.x, anim.x + l*sin(Y[0, i])))
    chs.set_ydata((r, r + l*cos(Y[0, i])))

    return whl, dts, chs
