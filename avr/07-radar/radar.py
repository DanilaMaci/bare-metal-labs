"""
Radar display for the HC-SR04 + servo sweep.

Reads "angle,distance" lines (one per reading) and draws them on a polar plot
with a fading tail, like a radar screen.

Fake data:  python3 radar.py
Hardware:   python3 radar.py /dev/cu.usbserial-A50285BI
"""

import math
import random
import sys
import time
from collections import deque

import matplotlib.pyplot as plt

# ---------------------------------------------------------------- settings

MAX_RANGE_CM = 400        # sensor's usable range, sets the outer edge of the plot
SWEEP_STEP = 5            # degrees between readings, must match the firmware
TAIL_LENGTH = 36         # points kept on screen (72 x 5 deg = one full sweep)
BAUD = 9600               # must match UBRR0L in the firmware

GREEN = '#33ff33'
DIM_GREEN = '#55ff55'
BACKGROUND = '#001a00'
GRID = '#227722'


# ------------------------------------------------------------ data sources
# Both of these are generators: they produce (angle, distance) pairs one at a
# time and pause in between, the way a live sensor behaves. Because they have
# the same shape, nothing else in the program cares which one it is given.


def fake_readings():
    """Pretend radar: a far wall with one object sitting at about 50 degrees."""
    while True:
        # sweep up 0..180 then back down, same as the servo does
        up = list(range(0, 181, SWEEP_STEP))
        down = list(range(180 - SWEEP_STEP, 0, -SWEEP_STEP))
        for angle in up + down:
            distance = 80 if 40 <= angle <= 60 else 350
            distance += random.randint(-4, 4)      # sensor noise
            yield angle, distance
            time.sleep(0.05)


def serial_readings(port):
    """Real radar: read 'angle,distance' lines from the microcontroller."""
    import serial                                  # only needed in this mode

    # timeout=1 means readline() gives up after a second instead of hanging
    # forever, so a disconnected board doesn't freeze the display
    ser = serial.Serial(port, BAUD, timeout=1)

    while True:
        # errors='ignore' drops corrupted bytes rather than crashing; the first
        # characters after the board resets are often garbage
        line = ser.readline().decode('ascii', errors='ignore').strip()

        parts = line.split(',')
        if len(parts) != 2:
            continue                               # partial or junk line

        try:
            yield int(parts[0]), int(parts[1])
        except ValueError:
            continue                               # not numbers (e.g. an 'X' line)


# ------------------------------------------------------------------- plot

def setup_axes(ax):
    """Apply the look of the plot.

    Called again after every ax.clear(), because clear() wipes these settings
    along with the data.
    """
    ax.set_facecolor(BACKGROUND)
    ax.set_ylim(0, MAX_RANGE_CM)
    ax.set_thetamin(0)                             # draw only the top half
    ax.set_thetamax(180)

    ax.set_yticks([100, 200, 300, 400])
    ax.set_yticklabels(['1 m', '2 m', '3 m', '4 m'], color=DIM_GREEN, fontsize=8)
    ax.set_xticks([math.radians(d) for d in range(0, 181, 30)])
    ax.tick_params(colors=DIM_GREEN)
    ax.grid(color=GRID, linewidth=0.6)
    ax.spines['polar'].set_color(DIM_GREEN)


def main():
    # A serial port on the command line means use the hardware; otherwise fall
    # back to fake data, so the display can be worked on anywhere.
    if len(sys.argv) > 1:
        source = serial_readings(sys.argv[1])
        print(f"reading from {sys.argv[1]}")
    else:
        source = fake_readings()
        print("no port given - using fake data")

    fig = plt.figure(figsize=(9, 5), facecolor=BACKGROUND)
    ax = fig.add_subplot(projection='polar')
    setup_axes(ax)

    plt.ion()                                      # don't block on show()
    plt.show()

    # A deque with maxlen drops its oldest item automatically once full, so the
    # tail manages its own length. Position in the deque is therefore age.
    points = deque(maxlen=TAIL_LENGTH)

    for angle, distance in source:
        points.append((angle, distance))

        ax.clear()
        setup_axes(ax)

        n = len(points)
        for i, (a, d) in enumerate(points):
            # newest point sits at the end of the deque, so (i+1)/n rises to 1.0
            # alpha is opacity: 1.0 solid, 0.0 invisible
            alpha = (i + 1) / n
            newest = (i == n - 1)

            ax.scatter(
                math.radians(a), d,
                c=GREEN,
                s=60 if newest else 18,            # make the newest point bigger
                alpha=alpha,
                edgecolors='none',
            )

        # sweep line at the current angle, like a radar's rotating beam
        ax.plot([math.radians(angle)] * 2, [0, MAX_RANGE_CM],
                color=GREEN, alpha=0.35, linewidth=1)

        ax.set_title(f"{angle:>3}deg   {distance:>3} cm",
                     color=DIM_GREEN, fontfamily='monospace', pad=18)

        plt.pause(0.001)                           # let matplotlib redraw


if __name__ == '__main__':
    main()