import argparse
import csv
from pathlib import Path

import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(
        description="Plot position, velocity, and acceleration."
    )
    parser.add_argument("csv_file", type=Path)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--title", default="Static test")
    args = parser.parse_args()

    columns = [
        "time_ms",
        "z_pz",
        "z_az",
        "kf_pz",
        "kf_vz",
        "kf_az",
    ]

    try:
        with args.csv_file.open(
            newline="", encoding="utf-8-sig"
        ) as file:
            reader = csv.DictReader(file, skipinitialspace=True)

            missing = set(columns) - set(reader.fieldnames or [])
            if missing:
                raise ValueError(
                    "Missing columns: " + ", ".join(sorted(missing))
                )

            records = []

            for line_number, row in enumerate(reader, start=2):
                try:
                    records.append([
                        float(row[name]) for name in columns
                    ])
                except (ValueError, TypeError) as error:
                    raise ValueError(
                        f"Invalid or incomplete row at line {line_number}"
                    ) from error

        if len(records) < 2:
            raise ValueError("At least two samples are required.")

        data = np.asarray(records, dtype=float)
        timestamps_ms = data[:, 0]

        if (
            not np.isfinite(timestamps_ms).all()
            or np.any(timestamps_ms < 0)
            or np.any(timestamps_ms >= 2**32)
            or np.any(timestamps_ms != np.floor(timestamps_ms))
        ):
            raise ValueError(
                "Timestamps must be uint32 millisecond values."
            )

        intervals_ms = np.diff(timestamps_ms)

        # Handle uint32 HAL tick-counter rollover.
        intervals_ms[intervals_ms < 0] += 2**32

        if np.any(intervals_ms <= 0):
            raise ValueError("Consecutive timestamps must differ.")

    except (OSError, ValueError) as error:
        parser.error(str(error))

    # All samples are plotted. First sample is at zero seconds.
    intervals_s = intervals_ms / 1000.0
    time_s = np.concatenate(([0.0], np.cumsum(intervals_s)))

    values = data[:, 1:].copy()

    invalid = ~np.isfinite(values)
    if invalid.any():
        print(
            f"Warning: {invalid.sum()} nonfinite values "
            "will appear as gaps."
        )
    values[invalid] = np.nan

    z_pz = values[:, 0]
    z_az = values[:, 1]
    kf_pz = values[:, 2]
    kf_vz = values[:, 3]
    kf_az = values[:, 4]

    # Print the actual filtered velocity range.
    finite_vz = kf_vz[np.isfinite(kf_vz)]

    if finite_vz.size:
        print(f"KF velocity minimum: {finite_vz.min():.8f} m/s")
        print(f"KF velocity maximum: {finite_vz.max():.8f} m/s")
        print(
            "KF velocity standard deviation: "
            f"{finite_vz.std():.8f} m/s"
        )

    # Altitude-derived average velocity over each sample interval.
    # Used only for graphing, not as a filter measurement.
    vz_baro = np.diff(z_pz) / intervals_s

    # Plot interval-average velocity at the interval midpoint.
    velocity_time_s = 0.5 * (time_s[:-1] + time_s[1:])

    fig, axes = plt.subplots(
        4,
        1,
        figsize=(13, 11),
        sharex=True,
        constrained_layout=True,
    )

    axes[0].plot(
        time_s,
        z_pz,
        label="Altitude measurement",
        color="tab:orange",
        linewidth=1.2,
        alpha=0.8,
    )
    axes[0].plot(
        time_s,
        kf_pz,
        label="KF position",
        color="tab:blue",
        linewidth=1.5,
    )
    axes[0].set_title("Vertical position")
    axes[0].set_ylabel("Position (m)")

    # Raw altitude-derived velocity on its own axis.
    axes[1].plot(
        velocity_time_s,
        vz_baro,
        label="Altitude-derived velocity (interval average)",
        color="tab:orange",
        linewidth=1.2,
        alpha=0.7,
    )
    axes[1].axhline(
        0, color="black", linewidth=0.7, alpha=0.5
    )
    axes[1].set_title("Raw altitude-derived vertical velocity")
    axes[1].set_ylabel("Velocity (m/s)")

    # Filtered velocity on a separate, independently scaled axis.
    axes[2].plot(
        time_s,
        kf_vz,
        label="KF velocity",
        color="tab:blue",
        linewidth=1.5,
    )
    axes[2].axhline(
        0, color="black", linewidth=0.7, alpha=0.5
    )
    axes[2].set_title("Filtered vertical velocity")
    axes[2].set_ylabel("Velocity (m/s)")

    axes[3].plot(
        time_s,
        z_az,
        label="Acceleration measurement",
        color="tab:orange",
        linewidth=1.2,
        alpha=0.8,
    )
    axes[3].plot(
        time_s,
        kf_az,
        label="KF acceleration",
        color="tab:blue",
        linewidth=1.5,
    )
    axes[3].set_title("Vertical acceleration")
    axes[3].set_ylabel("Acceleration (m/s²)")

    for ax in axes:
        ax.set_xlabel("Time (s)")
        ax.tick_params(axis="x", labelbottom=True)
        ax.set_xlim(0, time_s[-1])
        ax.grid(True, alpha=0.3)
        ax.legend()

    mean_interval_ms = float(np.mean(intervals_ms))
    mean_rate_hz = 1000.0 / mean_interval_ms

    fig.suptitle(
        f"Measurements vs KF: {args.title}\n"
        f"{len(data)} samples | "
        f"Mean interval: {mean_interval_ms:.2f} ms | "
        f"Mean rate: {mean_rate_hz:.2f} Hz"
    )

    output = args.output or args.csv_file.with_name(
        args.csv_file.stem + "_plots.png"
    )

    try:
        output.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(output, dpi=180)
    except OSError as error:
        parser.error(f"Could not save plot: {error}")
    finally:
        plt.close(fig)

    print(f"Samples: {len(data)}")
    print(f"Duration: {time_s[-1]:.3f} s")
    print(f"Mean interval: {mean_interval_ms:.3f} ms")
    print(f"Mean sampling rate: {mean_rate_hz:.3f} Hz")
    print(
        f"Interval range: {intervals_ms.min():.3f}"
        f"–{intervals_ms.max():.3f} ms"
    )
    print(f"Saved: {output}")


if __name__ == "__main__":
    main()