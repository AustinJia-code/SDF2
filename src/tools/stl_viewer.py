"""
Live STL viewer, reloads on file update.
python stl_viewer.py [path/to/file.stl]

Controls:
    Left drag       - rotate
    Right drag      - zoom
    Middle drag     - pan
    r               - reset camera
    q               - quit
"""

import sys
import os
import time
import argparse

try:
    from vedo import Plotter, Mesh
except ImportError:
    sys.exit ("Missing dependency: pip install vedo")

"""
Load an STL mesh from path
"""
def load_mesh (
    path: str
) -> Mesh | None:
    # Retry for a second if file is mid-write
    for attempt in range (10):
        try:
            m = Mesh (path)
            if m.npoints > 0:
                return m
        except Exception:
            pass

        time.sleep (0.1)

    return None


"""
Main
"""
def main ():
    parser = argparse.ArgumentParser (description = "Live STL viewer")
    parser.add_argument ("stl", help = "Path to the STL file to watch")
    parser.add_argument (
        "--interval", type = float, default = 0.5,
        help = "Poll interval in seconds (default: 0.5)")
    args = parser.parse_args ()

    path = os.path.abspath (args.stl)
    if not os.path.exists (path):
        sys.exit (f"File not found: {path}")

    mesh = load_mesh (path)
    if mesh is None:
        sys.exit ("Failed to load STL file.")

    plt = Plotter (title = f"Viewing {os.path.basename(path)}", axes = 1)

    state = {
        "mtime": os.path.getmtime (path),
        "mesh": mesh,
        "first": True,
    }

    """
    Check for file updates and reload if modified
    """
    def on_timer (event):
        try:
            mtime = os.path.getmtime (path)
        except FileNotFoundError:
            return
        if mtime == state["mtime"]:
            return
        state["mtime"] = mtime
        new_mesh = load_mesh (path)
        if new_mesh is None:
            return
        plt.remove (state["mesh"]).add (new_mesh)
        state["mesh"] = new_mesh
        print(f"Reloaded  ({new_mesh.npoints} points, {new_mesh.ncells} faces)")

    plt.add_callback ("timer", on_timer)
    plt.timer_callback ("create", dt = int (args.interval * 1000))

    print (f"Watching {path}  (q or close window to quit)")
    plt.show (mesh, resetcam = True, interactive = True)


"""
Entry
"""
if __name__ == "__main__":
    main ()
