#!/usr/bin/env -S uv run --no-dev
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

import os
import pathlib
import platform
import subprocess
import sys
import spam

ROOT = pathlib.Path(os.path.abspath(__file__)).parent


def run():
    env = dict(os.environ)
    env["PYTHONUNBUFFERED"] = "1"
    python = sys.executable
    system = platform.system()

    if system == "Darwin" or system == "Linux":
        args = [
            python,
            "build-main.py",
            *sys.argv[1:],
        ]
        make_dir = ROOT / "cpython-unix"
        os.chdir(make_dir)
        return os.execve(python, args, env)
    elif system == "Windows" and sys.version_info[:2] == (3, 14):
        args = [
            python,
            "test_numpy.py",
            *sys.argv[1:],
        ]
        cwd = str(ROOT / "cpython-windows")

        # print(args)
        # print(cwd)
        # print(env)

        # return subprocess.run(args, cwd=cwd, env=env, check=True, bufsize=0)
        print(spam.is_positive(1))

        # # https://github.com/numpy/numpy/issues
        # import numpy as np
        # print(np.__version__)
        # # print(np.__all__)
        # print(np.pi)
        # print(np.e)
        # print(np.cos(np.array([np.pi/2,np.radians(90)])))
        # print(np.max(np.array([1,2,3,4])))
    else:
        raise Exception(f"Unsupported host system: {system}")


if __name__ == "__main__":
    try:
        run()
    except subprocess.CalledProcessError as e:
        sys.exit(e.returncode)