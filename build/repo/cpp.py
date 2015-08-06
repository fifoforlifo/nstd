import os
import pynja


class CppVariant(pynja.Variant):
    if os.name == 'nt':
        fieldDefs = [
            "os",           [ "windows", "android" ],
            "toolchain",    [ "msvc12", "clang12", "msvc14", "mingw64", "android_arm_gcc"  ],
            "arch",         [ "x86", "amd64", "aarch32" ],
            "config",       [ "dbg", "rel" ],
            "crt",          [ "scrt", "dcrt" ],
        ]
    elif os.name == 'posix':
        fieldDefs = [
            "os",           [ "linux" ],
            "toolchain",    [ "gcc", "clang" ],
            "arch",         [ "x86", "amd64" ],
            "config",       [ "dbg", "rel" ],
            "crt",          [ "scrt", "dcrt" ],
        ]
    else:
        raise Exception("Not implemented")

    def __init__(self, string):
        super().__init__(string, self.get_field_defs())

    def get_field_defs(self):
        return CppVariant.fieldDefs

class CppLibVariant(pynja.Variant):
    fieldDefs = list(CppVariant.fieldDefs)
    fieldDefs.extend([
        "linkage",      [ "sta", "dyn" ],
    ])

    def __init__(self, string):
        super().__init__(string, self.get_field_defs())

    def get_field_defs(self):
        return CppLibVariant.fieldDefs


class CppProject(pynja.CppProject):
    def __init__(self, projectMan, variant):
        super().__init__(projectMan, variant)

        if not (isinstance(variant, CppVariant) or isinstance(variant, CppLibVariant)):
            raise Exception("expecting CppVariant or CppLibVariant")

        # android-specific
        if isinstance(self.toolchain, pynja.AndroidGccToolChain):
            self.android_select_stl('gnu-libstdc++', linkDynamic=True)


    def get_toolchain(self):
        toolchainName = "%s-%s" % (self.variant.toolchain, self.variant.arch)
        toolchain = self.projectMan.get_toolchain(toolchainName)
        if not toolchain:
            raise Exception("Could not find toolchain " + toolchainName)
        return toolchain

    def set_gcc_machine_arch(self, task):
        if ("gcc" in self.variant.toolchain) or ("mingw" in self.variant.toolchain):
            if self.variant.arch == "x86":
                task.addressModel = "-m32"
            elif self.variant.arch == "amd64":
                task.addressModel = "-m64"

    def set_cpp_compile_options(self, task):
        super().set_cpp_compile_options(task)
        task.extraDeps.extend(self._forcedDeps)
        task.phonyTarget = os.path.basename(task.sourcePath)

        self.set_gcc_machine_arch(task)
        task.std = 'gnu++11'

        task.debugLevel = 2
        if self.variant.config == "dbg":
            task.optLevel = 0
        elif self.variant.config == "rel":
            task.optLevel = 3
            if not task.createPCH:
                task.lto = self.toolchain.ltoSupport
            if isinstance(self.toolchain, pynja.MsvcToolChain):
                task.extraOptions.append("/Oy")

        task.warningsAsErrors = True

        if "msvc" in self.variant.toolchain:
            task.dynamicCRT = (self.variant.crt == 'dcrt')
            task.extraOptions.append('/wd4996')
        if isinstance(self.toolchain, pynja.ClangToolChain):
            task.extraOptions.append("-fcolor-diagnostics")
        if isinstance(self.toolchain, pynja.GccToolChain) or isinstance(self.toolchain, pynja.ClangMsvcToolChain):
            task.extraOptions.append("-Wno-deprecated")

        task.defines.append('_CRT_SECURE_NO_WARNINGS')
        task.defines.append('_CRT_SECURE_NO_DEPRECATE')


    # library creation

    def make_library(self, name):
        if self.variant.linkage == "sta":
            return self.make_static_lib(name)
        else:
            return self.make_shared_lib(name)

    def set_shared_lib_options(self, task):
        if self.variant.config == 'rel':
            task.lto = self.toolchain.ltoSupport


    def set_shared_lib_options(self, task):
        super().set_shared_lib_options(task)
        self.set_gcc_machine_arch(task)
        task.keepDebugInfo = True

    def set_executable_options(self, task):
        super().set_executable_options(task)
        self.set_gcc_machine_arch(task)
        task.keepDebugInfo = True
        if self.variant.config == 'rel':
            task.lto = self.toolchain.ltoSupport


    # Convenience functions that assumes that the current project
    # has a CppVariant or CppLibVariant, and that the target
    # project expects a CppLibVariant.

    def get_cpplib_project(self, projName, linkage=None):
        if linkage == None:
            linkage = self.variant.linkage
        variantStr = "%s-%s-%s-%s-%s-%s" % (
            self.variant.os,
            self.variant.toolchain,
            self.variant.arch,
            self.variant.config,
            self.variant.crt,
            linkage
        )
        variant = CppLibVariant(variantStr)
        project = self.get_project(projName, variant)
        return project

    def add_cpplib_dependency(self, projName, linkage="dyn"):
        project = self.get_cpplib_project(projName, linkage)
        self.add_lib_dependency(project)
        return project


    # boost

    def add_boost_lib_dependency(self, name, linkShared=True):
        boostBuild = self.get_project("boost_build", self.variant)
        basepath = boostBuild.calc_lib_basepath(name, linkShared)
        if 'msvc' in self.variant.toolchain:
            if linkShared:
                self.add_input_lib(basepath + ".lib")
                self.add_runtime_dependency(basepath + ".dll")
            else:
                self.add_input_lib(basepath + ".lib")
        else:
            raise Exception("TODO: gcc boost dependencies")

