import numpy as np
import matplotlib.pyplot as plt
from common import IMG_DIR

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



def run():
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
    plt.plot(n, reg_dev)
    plt.plot(n, reg_eev)
    plt.legend(['dev', 'eval-environment'])
    plt.title('Regular')
    plt.show()

    prefix = lambda pref, var: [pref + str(x) for x in var]

    # extra vars
    plt.plot(extravars_n, (extravars_dev / reg_dev).T)
    plt.plot(extravars_n, (extravars_eev / reg_eev).T)
    plt.legend(prefix('dev ', n) + prefix('eval-environment ', n))
    plt.title('Extra vars')
    plt.show()

    # extra branch
    plt.plot(extrabrch_n, (extrabrch_dev / reg_dev).T)
    plt.plot(extrabrch_n, (extrabrch_eev / reg_eev).T)
    plt.legend(prefix('dev ', n) + prefix('eval-environment ', n))
    plt.title('Extra brch')
    plt.show()

    # data = np.log(data)
    # xs = np.array([28, 29, 30, 31, 32])

    # plt.plot(xs, data[0,0,:])
    # plt.plot(xs, data[0,1,:])
    # plt.plot(xs, data[1,0,:])
    # plt.plot(xs, data[1,1,:])
    # plt.plot(xs, data[2,0,:])
    # plt.plot(xs, data[2,1,:])

    # plt.legend(['unique-hole-closures, unannot', 'unique-hole-closures, annot',
    #             'rec-closures, unannot', 'rec-closures, annot',
    #             'dev, unannot', 'dev, annot'])
    # plt.ylabel('Log time (s)')
    # plt.xlabel('n')
    # plt.title('fib(n) evaluation times in Hazel')
    # plt.grid()

    # plt.savefig(IMG_DIR + 'subst_evalenv_fib_perf.pdf')
