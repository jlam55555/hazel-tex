import numpy as np
import matplotlib.pyplot as plt
from common import IMG_DIR

data = np.array([
    # unique-hole-closures
    [
        # without type annotation
        [2792, 4709, 7869, 12921, 20665],
        # with type annotation
        [3265, 4863, 7814, 12775, 20390],
    ],
    # rec-closures
    [
        [2481, 4128, 6392, 11214, 17957],
        [2488, 3960, 6395, 10513, 17331]
    ],
    # dev
    [
        [7734, 15123, 22526, 35231, 59094],
        [7397, 11404, 21282, 32167, 48254]
    ],
])

def run():
    global data
    data = np.log(data)
    xs = np.array([28, 29, 30, 31, 32])

    plt.plot(xs, data[0,0,:])
    plt.plot(xs, data[0,1,:])
    plt.plot(xs, data[1,0,:])
    plt.plot(xs, data[1,1,:])
    plt.plot(xs, data[2,0,:])
    plt.plot(xs, data[2,1,:])

    plt.legend(['unique-hole-closures, unannot', 'unique-hole-closures, annot',
                'rec-closures, unannot', 'rec-closures, annot',
                'dev, unannot', 'dev, annot'])
    plt.ylabel('Log time (s)')
    plt.xlabel('n')
    plt.title('fib(n) evaluation times in Hazel')
    plt.grid()

    plt.savefig(IMG_DIR + 'subst_evalenv_fib_perf.png')
