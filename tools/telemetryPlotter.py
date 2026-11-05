import collections
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import serial

ser = serial.Serial("/dev/tty.usbmodem103", 460800, timeout=0.05)
N = 600
t, rc, rk1, rk2, rcmd = (collections.deque(maxlen=N) for _ in range(5))

# --- GLOBAL CONFIGURATION FLAGS ---
ENFORCE_MIN_SPAN = True  # True: Stabilizes flatlines | False: Raw autoscale
LOCK_GLOBAL_Y_SCALE = (
    True  # True: Unified Y-axis across all 3 plots | False: Independent Y-axes
)

MIN_Y_SPAN = 2.0  # Minimum degrees window height for flatlines
Y_PADDING = 0.10  # 10% safety margin for high activity

# Create 3 vertically stacked subplots that share the same X-axis timeline
fig, axs = plt.subplots(3, 1, sharex=True, figsize=(8, 8))
fig.subplots_adjust(hspace=0.3)

labels = ("Complementary", "Kalman 1-state", "Kalman 2-state")
colors = ("#1f77b4", "#ff7f0e", "#2ca02c")
lines = []
cmd_lines = []

for ax, label, color in zip(axs, labels, colors):
    (ln,) = ax.plot([], [], label=label, color=color)
    (cmd_ln,) = ax.plot(
        [], [], label="Command", color="k", linestyle="--", alpha=0.7
    )
    lines.append(ln)
    cmd_lines.append(cmd_ln)
    ax.set_ylabel("roll (deg)")
    ax.legend(loc="upper left")

axs[-1].set_xlabel("time (s)")


def update(_):
    while ser.in_waiting:
        p = ser.readline().decode(errors="ignore").strip().split(",")
        # Expecting at least 8 fields based on the updated C++ snprintf
        if len(p) >= 8 and p[0].isdigit():
            t.append(int(p[0]) / 1000)
            rc.append(int(p[1]) / 100)
            rk1.append(int(p[2]) / 100)
            rk2.append(int(p[3]) / 100)
            # p[4], p[5], p[6] are pitch telemetry; p[7] is commandRoll
            rcmd.append(int(p[7]) / 100)

    for ln, cmd_ln, ys in zip(lines, cmd_lines, (rc, rk1, rk2)):
        ln.set_data(t, ys)
        if rcmd:
            cmd_ln.set_data(t, rcmd)

    if t and rcmd:
        axs[0].set_xlim(t[0], t[-1] + 0.1)

        # 1. Determine baseline bounds based on our layout strategy flag
        if LOCK_GLOBAL_Y_SCALE:
            global_min = min(min(rc), min(rk1), min(rk2), min(rcmd))
            global_max = max(max(rc), max(rk1), max(rk2), max(rcmd))
            global_span = global_max - global_min

        # 2. Iterate and apply scale rules to each subplot axis frame
        for ax, ys in zip(axs, (rc, rk1, rk2)):
            y_min = global_min if LOCK_GLOBAL_Y_SCALE else min(min(ys), min(rcmd))
            y_max = global_max if LOCK_GLOBAL_Y_SCALE else max(max(ys), max(rcmd))
            current_span = (
                global_span if LOCK_GLOBAL_Y_SCALE else (y_max - y_min)
            )

            # 3. Apply standard Flatline-Noise mitigation
            if ENFORCE_MIN_SPAN and (current_span < MIN_Y_SPAN):
                y_center = (y_max + y_min) / 2
                ax.set_ylim(
                    y_center - MIN_Y_SPAN / 2, y_center + MIN_Y_SPAN / 2
                )
            else:
                pad = (current_span * Y_PADDING) if current_span > 0 else 0.5
                ax.set_ylim(y_min - pad, y_max + pad)

    return lines + cmd_lines


ani = animation.FuncAnimation(fig, update, interval=50, blit=False)
plt.show()