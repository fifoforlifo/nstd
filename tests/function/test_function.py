import os, pynja, repo

@pynja.project
class test_function(repo.CppProject):
    def emit(self):
        libgtest = self.add_cpplib_dependency('gtest', 'sta')
        libnstd = self.add_cpplib_dependency('nstd', 'sta')

        self.includePaths.append(os.path.join(libgtest.projectDir, "include"))
        self.includePaths.append(os.path.join(libnstd.projectDir, "inc"))

        sources = [
            "src/test_function.cpp",
        ]
        self.cpp_compile(sources)
        self.make_executable("test_function")
