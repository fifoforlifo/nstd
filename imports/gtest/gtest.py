import os, pynja, repo

@pynja.project
class gtest(repo.CppProject):
    def emit(self):
        self.includePaths.append(self.projectDir)
        self.includePaths.append(os.path.join(self.projectDir, "include"))
        if isinstance(self.toolchain, pynja.ClangMsvcToolChain):
            self.defines.append("GTEST_ENABLE_CATCH_EXCEPTIONS_=0")
            self.defines.append("GTEST_HAS_SEH=0")

        sources = [
            "src/gtest_main.cc",
            "src/gtest-all.cc",
        ]
        with self.cpp_compile_ex(sources) as tasks:
            tasks.warningsAsErrors = False
            
        self.make_static_lib("gtest")
