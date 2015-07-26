# declarations in this file are imported into the repo namespace

import sys
import os
import pynja
from .root_dir import finder


pynja.set_root_dir(finder.get_root_dir())
pynja.rootDir = pynja.root_paths.rootDir


def init():
    # output paths
    pynja.rootPaths.out = os.path.join(pynja.rootDir, "_out")
    pynja.rootPaths.built = os.path.join(pynja.rootPaths.out, "built")
    pynja.rootPaths.codeBrowsing = os.path.join(pynja.rootPaths.out, "cb")
    pynja.rootPaths.bin = os.path.join(pynja.rootPaths.out, "bin")

    if (os.name == 'nt'):
        # you can customize these paths to point at a location in source control
        if pynja.io.is_64bit_os():
            pynja.rootPaths.msvc9 = r"C:\Program Files (x86)\Microsoft Visual Studio 9.0"
            pynja.rootPaths.msvc10 = r"C:\Program Files (x86)\Microsoft Visual Studio 10.0"
            pynja.rootPaths.msvc11 = r"C:\Program Files (x86)\Microsoft Visual Studio 11.0"
            pynja.rootPaths.msvc12 = r"C:\Program Files (x86)\Microsoft Visual Studio 12.0"
            pynja.rootPaths.msvc14 = r"C:\Program Files (x86)\Microsoft Visual Studio 14.0"
            pynja.rootPaths.llvmDir = r"C:\Program Files (x86)\LLVM"
        else:
            raise Exception("32-bit OS, seriously?")

        pynja.rootPaths.winsdk71 = r'C:\Program Files\Microsoft SDKs\Windows\v7.1'
        pynja.rootPaths.winsdk80 = r'C:\Program Files (x86)\Windows Kits\8.0'
        pynja.rootPaths.winsdk81 = r'C:\Program Files (x86)\Windows Kits\8.1'
        pynja.rootPaths.ucrt100 = r'C:\Program Files (x86)\Windows Kits\10'
        pynja.rootPaths.ucrt100Ver = "10.0.10150.0"

        pynja.rootPaths.mingw = r"C:\MinGW"
        pynja.rootPaths.mingw64 = r"C:\MinGW64"

        pynja.rootPaths.android_ndk_r8d = r"C:\Software\Android\android-ndk-r8d"
