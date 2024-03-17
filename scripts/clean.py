# encoding: utf-8
"""清理生成"""
from pathlib import Path
import shutil
import sys

path_build = Path("./build/")
path_bin = Path("./bin/")


def clean_all():
    for sub in path_build.iterdir():
        if sub.is_dir():
            shutil.rmtree(sub)
        else:
            if sub.name != ".gitkeep":
                sub.unlink()
    if path_bin.exists():
        shutil.rmtree("./bin/")


def clean_build():
    for sub in path_bin.iterdir():
        if sub.suffix == ".lib" or sub.suffix == ".exp" or sub.suffix == ".pdb":
            sub.unlink()


if len(sys.argv) > 1:
    if sys.argv[1] == "--all":
        clean_all()
    elif sys.argv[1] == "--build":
        clean_build()
else:
    clean_all()
