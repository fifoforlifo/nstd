#!/usr/bin/python3.3
import sys
import os

rootDir = sys.path[0]
sys.path.append(os.path.join(rootDir, "build"))

import pynja
import repo


build_cpp = True
build_java = True


def generate_ninja_build(projectMan):
    cpp_variants = []
    deploy_variants = []

    isTargetWindows = (os.name == 'nt')

    if os.name == 'nt':
        if build_cpp:
            def toolchain_assign_winsdk(toolchain, msvcVer):
                if msvcVer < 10:
                    toolchain.winsdkVer = 71
                    toolchain.winsdkDir = pynja.rootPaths.winsdk71
                elif msvcVer < 14:
                    toolchain.winsdkVer = 80
                    toolchain.winsdkDir = pynja.rootPaths.winsdk80
                elif msvcVer == 14:
                    toolchain.winsdkVer = 81
                    toolchain.winsdkDir = pynja.rootPaths.winsdk81
                    toolchain.ucrtVer = pynja.rootPaths.ucrt100Ver
                    toolchain.ucrtDir = pynja.rootPaths.ucrt100
                else:
                    raise Exception('unsupported MSVC version')

            def make_msvc_tool_chain(msvcVer, installDir, arch):
                name = "msvc{msvcVer}-{arch}".format(**locals())
                toolchain = pynja.MsvcToolChain(name, installDir, arch, msvcVer)
                toolchain_assign_winsdk(toolchain, msvcVer)
                return toolchain

            def make_clang_msvc_tool_chain(msvcVer, installDir, arch, llvmDir):
                name = "clang{msvcVer}-{arch}".format(**locals())
                toolchain = pynja.ClangMsvcToolChain(name, installDir, arch, msvcVer, llvmDir)
                toolchain_assign_winsdk(toolchain, msvcVer)
                return toolchain

            if os.path.exists(pynja.rootPaths.msvc12):
                projectMan.add_toolchain(make_msvc_tool_chain(12, pynja.rootPaths.msvc12, "x86"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc12-x86-dbg-dcrt"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc12-x86-rel-dcrt"))

                projectMan.add_toolchain(make_msvc_tool_chain(12, pynja.rootPaths.msvc12, "amd64"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc12-amd64-dbg-dcrt"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc12-amd64-rel-dcrt"))

                if os.path.exists(pynja.rootPaths.llvmDir):
                    projectMan.add_toolchain(make_clang_msvc_tool_chain(12, pynja.rootPaths.msvc12, "x86", pynja.rootPaths.llvmDir))
                    cpp_variants.append(repo.cpp.CppVariant("windows-clang12-x86-dbg-dcrt"))
                    cpp_variants.append(repo.cpp.CppVariant("windows-clang12-x86-rel-dcrt"))

                    projectMan.add_toolchain(make_clang_msvc_tool_chain(12, pynja.rootPaths.msvc12, "amd64", pynja.rootPaths.llvmDir))
                    cpp_variants.append(repo.cpp.CppVariant("windows-clang12-amd64-dbg-dcrt"))
                    cpp_variants.append(repo.cpp.CppVariant("windows-clang12-amd64-rel-dcrt"))

            if os.path.exists(pynja.rootPaths.msvc14):
                projectMan.add_toolchain(make_msvc_tool_chain(14, pynja.rootPaths.msvc14, "x86"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc14-x86-dbg-dcrt"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc14-x86-rel-dcrt"))

                projectMan.add_toolchain(make_msvc_tool_chain(14, pynja.rootPaths.msvc14, "amd64"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc14-amd64-dbg-dcrt"))
                cpp_variants.append(repo.cpp.CppVariant("windows-msvc14-amd64-rel-dcrt"))

            if os.path.exists(pynja.rootPaths.mingw64):
                projectMan.add_toolchain(pynja.GccToolChain("mingw64-x86", pynja.rootPaths.mingw64, isTargetWindows))
                cpp_variants.append(repo.cpp.CppVariant("windows-mingw64-x86-dbg-dcrt"))
                cpp_variants.append(repo.cpp.CppVariant("windows-mingw64-x86-rel-dcrt"))

                projectMan.add_toolchain(pynja.GccToolChain("mingw64-amd64", pynja.rootPaths.mingw64, isTargetWindows))
                cpp_variants.append(repo.cpp.CppVariant("windows-mingw64-amd64-dbg-dcrt"))
                cpp_variants.append(repo.cpp.CppVariant("windows-mingw64-amd64-rel-dcrt"))

            if os.path.exists(pynja.rootPaths.android_ndk_r8d):
                projectMan.add_toolchain(pynja.AndroidGccToolChain("android_arm_gcc-aarch32", pynja.rootPaths.android_ndk_r8d, "4.7", 14, "armeabi", prefix="arm-linux-androideabi-"))
                cpp_variants.append(repo.cpp.CppVariant("android-android_arm_gcc-aarch32-dbg-dcrt"))
                cpp_variants.append(repo.cpp.CppVariant("android-android_arm_gcc-aarch32-rel-dcrt"))

    elif os.name == 'posix':
        if build_cpp:
            projectMan.add_toolchain(pynja.GccToolChain("gcc-x86", "/usr", isTargetWindows))
            cpp_variants.append(repo.cpp.CppVariant("linux-gcc-x86-dbg-dcrt"))

            projectMan.add_toolchain(pynja.GccToolChain("gcc-amd64", "/usr", isTargetWindows))
            cpp_variants.append(repo.cpp.CppVariant("linux-gcc-amd64-dbg-dcrt"))

            projectMan.add_toolchain(pynja.ClangToolChain("clang-amd64", "/home/lolo/Downloads/clang+llvm-3.2-x86_64-linux-ubuntu-12.04", isTargetWindows))
            cpp_variants.append(repo.cpp.CppVariant("linux-clang-amd64-dbg-dcrt"))

    else:
        raise Exception("Not implemented")

    for config in ['dbg', 'rel']:
        for toolchain_name in repo.CppVariant.fieldDefs[1*2 + 1]:
            if 'android' not in toolchain_name:
                for arch in ['x86', 'amd64']:
                    deploy_variants.append(repo.DeployVariant("%s-%s-%s" % (toolchain_name, arch, config)))
            else:
                for arch in ['aarch32']:
                    deploy_variants.append(repo.DeployVariant("%s-%s-%s" % (toolchain_name, arch, config)))

    projectMan.emit_rules()
    projectMan.ninjaFile.write("\n");
    projectMan.ninjaFile.write("#############################################\n");
    projectMan.ninjaFile.write("# Begin files.\n");
    projectMan.ninjaFile.write("\n");

    for variant in deploy_variants:
        projectMan.get_project("tests", variant)
    
    projectMan.emit_deploy_targets()
    projectMan.emit_phony_targets()
    projectMan.emit_regenerator_target(currentScriptPath)



################################################################################
#   Main script

print("generating with rootDir=%s" % pynja.rootDir)
repo.init()
currentScriptPath = os.path.join(pynja.rootDir, os.path.basename(__file__))
# TODO: fix core pynja and remove these __main__ hacks
pynja.root_paths._scriptPathsAbs['__main__'] = currentScriptPath
pynja.root_paths._scriptPathsRel['__main__'] = '.'
pynja.root_paths._scriptRelToAbs['.'] = currentScriptPath

# Import subdirectories, to define all projects.
pynja.import_subdir('imports')
pynja.import_subdir('nstd')
pynja.import_subdir('tests')

# Generate the build.
pynja.regenerate_build(generate_ninja_build, pynja.rootPaths.built, pynja.rootPaths.codeBrowsing)
