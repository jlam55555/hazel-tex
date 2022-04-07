import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from common import IMG_DIR

plt.rcParams.update({
    'text.usetex': True
})

data_dev = np.array([
    [ 334, 394, 509, 539, 658, 677, 339, 305, 302, 339, 336 ],
    [ 478, 599, 695, 835, 1116, 1107, 524, 442, 452, 452, 455 ],
    [ 775, 929, 1214, 1332, 1518, 1686, 744, 700, 729, 794, 708 ],
    [ 1233, 1502, 1874, 2310, 2398, 2723, 1171, 1189, 1134, 1104, 1231 ],
    [ 2019, 2391, 2939, 3399, 3872, 4417, 1841, 1747, 1761, 1773, 1780 ],
])

# evalenv
data_eev = np.array([
    [ 255, 267, 276, 242, 245, 243, 330, 384, 417, 435, 519 ],
    [ 406, 374, 376, 358, 366, 330, 497, 576, 573, 593, 660 ],
    [ 578, 558, 559, 591, 561, 569, 775, 857, 911, 912, 1037 ],
    [ 851, 883, 871, 864, 888, 908, 1209, 1363, 1469, 1473, 1684 ],
    [ 1318, 1388, 1382, 1398, 1399, 1415, 1935, 2262, 2302, 2356, 2492 ],
])

data_renumbering = np.array([
    [1, 0, 0, 0, 0, 1, 0 ],
    [2, 0, 0, 0, 0, 1, 0 ],
    [3, 1, 2, 0, 0, 1, 0 ],
    [4, 1, 1, 1, 1, 0, 0 ],
    [5, 1, 1, 2, 0, 3, 0 ],
    [6, 5, 1, 3, 1, 0, 0 ],
    [7, 4, 5, 6, 2, 2, 0 ],
    [8, 3, 3, 14, 0, 0, 0 ],
    [9, 6, 18, 33, 1, 0, 1 ],
    [10, 14, 29, 61, 0, 0, 0 ],
    [11, 13, 41, 91, 3, 2, 0 ],
    [12, 25, 145, 203, 2, 0, 1 ],
    [13, 65, 578, 383, 2, 0, 0 ],
    [14, 147, 2399, 924, 1, 3, 1 ],
    [15, 226, 16597, 1603, 3, 0, 1 ],
    [16, -1, -1, -1, 1, 0, 1 ],
    [17, -1, -1, -1, 2, 1, 1 ],
    [18, -1, -1, -1, 0, 3, 1 ],
    [19, -1, -1, -1, 0, 0, 1 ],
    [20, -1, -1, -1, 3, 4, 0 ],
    [21, -1, -1, -1, 2, 0, 1 ],
    [22, -1, -1, -1, 0, 2, 1 ],
    [23, -1, -1, -1, 0, 3, 1 ],
    [24, -1, -1, -1, 0, 6, 1 ],
    [25, -1, -1, -1, 1, 4, 1 ],
    [26, -1, -1, -1, 1, 2, 1 ],
])

data_far = np.array([
    [7, 12, 17, 58, 4762964, 4762966, 4762966, 4762967],
    [7, 21, 17, 69, 4762964, 12, 21, 13]
])

def run():
    global data_far

    n = np.arange(1, 9)
    plt.figure()
    plt.stem(n, data_far[0, :], 'r')
    plt.gcf().tight_layout()
    plt.grid()
    plt.ylabel('Calls to evaluate')
    plt.xlabel('Step number')
    plt.semilogy()
    plt.savefig(f'{IMG_DIR}perf_no_far.pdf')

    plt.figure()
    plt.stem(n, data_far[1, :], 'b')
    plt.gcf().tight_layout()
    plt.grid()
    plt.ylabel('Calls to evaluate')
    plt.xlabel('Step number')
    plt.semilogy()
    plt.savefig(f'{IMG_DIR}perf_far.pdf')

    global data_renumbering

    data_renumbering = data_renumbering.T

    n = data_renumbering[0, :]
    renum_dev = data_renumbering[1:4, :15]
    renum_eev = data_renumbering[4:, :]

    # add an extra row with the overall time
    renum_dev = np.concatenate([renum_dev, np.sum(renum_dev, axis=0, keepdims=True)])
    renum_eev = np.concatenate([renum_eev, np.sum(renum_eev, axis=0, keepdims=True)])

    plt.figure()
    plt.plot(n[:15], (renum_dev+.01).T)
    plt.xlabel('Number of holes')
    plt.ylabel('Time (ms)')
    plt.ylim([1, 20000])
    plt.xlim([0, 26])
    plt.grid()
    plt.legend(['evaluate', 'postprocessing', 'equality check', 'overall'])
    plt.gcf().tight_layout()
    plt.semilogy()
    plt.savefig(f'{IMG_DIR}perf_renum_dev.pdf')

    plt.figure()
    plt.plot(n, (renum_eev+.01).T)
    plt.xlabel('Number of holes')
    plt.ylabel('Time (ms)')
    plt.ylim([1, 20000])
    plt.xlim([0, 26])
    plt.grid()
    plt.legend(['evaluate', 'postprocessing', 'equality check', 'overall'])
    plt.gcf().tight_layout()
    plt.semilogy()
    plt.savefig(f'{IMG_DIR}perf_renum_eev.pdf')

    global data_dev, data_eev

    n = np.arange(22, 27)

    reg_dev = data_dev[:, 0, np.newaxis]
    reg_eev = data_eev[:, 0, np.newaxis]

    extravars_n = np.arange(0, 12, 2)
    extravars_dev = np.hstack((reg_dev, data_dev[:, 6:]))
    extravars_eev = np.hstack((reg_eev, data_eev[:, 6:]))

    extrabrch_n = np.arange(0, 12, 2)
    extrabrch_dev = np.hstack((reg_dev, data_dev[:, 1:6]))
    extrabrch_eev = np.hstack((reg_eev, data_eev[:, 1:6]))

    print(n, extravars_n, extrabrch_n)
    print(reg_dev, reg_eev)
    print(extravars_dev, extravars_eev)
    print(extrabrch_dev, extrabrch_eev)

    # regular variables
    plt.figure()
    plt.plot(n, reg_dev, 'r')
    plt.plot(n, reg_eev, 'b')
    plt.legend(['dev', 'e-e'])
    plt.xlabel('n')
    plt.ylabel('Time (ms)')
    plt.gcf().tight_layout()
    plt.grid()
    plt.semilogy()
    plt.savefig(f'{IMG_DIR}perf_fib.pdf')

    prefix = lambda pref, var: [pref + str(x) for x in var]

    # extra vars
    plt.figure()
    plt.plot(extravars_n, (extravars_dev / reg_dev).T)
    plt.plot(extravars_n, (extravars_eev / reg_eev).T)
    plt.xlabel('Extra global variables')
    plt.ylabel('Relative elapsed time (normalized to 0 extra variables)')
    plt.grid()
    plt.gcf().tight_layout()

    # color the plot
    # https://stackoverflow.com/a/35971096/2397327
    series = n.size
    cms = [plt.cm.Reds, plt.cm.Blues]
    colors = [cms[0](i) for i in np.linspace(0, 1, series)] + \
        [cms[1](i) for i in np.linspace(0, 1, series)]
    for i, j in enumerate(plt.gca().lines):
        j.set_color(colors[i])

    plt.legend(prefix('dev n=', n) + prefix('e-e n= ', n))
    plt.savefig(f'{IMG_DIR}perf_fib_more_vars.pdf')

    # extra branch
    plt.figure()
    plt.plot(extrabrch_n, (extrabrch_dev / reg_dev).T)
    plt.plot(extrabrch_n, (extrabrch_eev / reg_eev).T)
    plt.xlabel('Variables in unused branch')
    plt.ylabel('Relative elapsed (normalized to 0 extra variables)')
    plt.gcf().tight_layout()
    plt.grid()

    # color the plot
    # https://stackoverflow.com/a/35971096/2397327
    series = n.size
    cms = [plt.cm.Reds, plt.cm.Blues]
    colors = [cms[0](i) for i in np.linspace(0, 1, series)] + \
        [cms[1](i) for i in np.linspace(0, 1, series)]
    for i, j in enumerate(plt.gca().lines):
        j.set_color(colors[i])

    plt.legend(prefix('dev n=', n) + prefix('e-e n=', n))
    plt.savefig(f'{IMG_DIR}perf_fib_more_branches.pdf')
