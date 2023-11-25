"""清理生成"""
from pathlib import Path
import shutil
import sys


def clean_all():
    dir = Path("./build/")
    for sub in dir.iterdir():
        if sub.is_dir():
            shutil.rmtree(sub)
        else:
            if sub.name != ".gitkeep":
                sub.unlink()
    shutil.rmtree("./bin/")


def clean_build():
    dir = Path("./bin/")
    for sub in dir.iterdir():
        if sub.suffix == ".lib" or sub.suffix == ".exp" or sub.suffix == ".pdb":
            sub.unlink()


if len(sys.argv) > 1:
    if sys.argv[1] == "--all":
        clean_all()
    elif sys.argv[1] == "--build":
        clean_build()
else:
    clean_all()
