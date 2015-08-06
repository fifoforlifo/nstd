import os, pynja, repo

@pynja.project
class test_function(repo.CppProject):
    def emit(self):
        libgtest = self.add_cpplib_dependency('gtest', 'sta')
        libnstd = self.add_cpplib_dependency('nstd', 'sta')

        self.includePaths.append(os.path.join(libgtest.projectDir, "include"))
        self.includePaths.append(os.path.join(libnstd.projectDir, "inc"))

        sources = [
            "src/test_func_ref.cpp",
            "src/test_function.cpp",
            "src/test_function_main.cpp",
        ]

        pchTask = self.make_pch("src/test_function_pch.h")
        with self.cpp_compile_ex(sources) as tasks:
            tasks.usePCH = pchTask.outputPath

        self.make_executable("test_function")
