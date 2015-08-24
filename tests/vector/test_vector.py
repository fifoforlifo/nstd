import os, pynja, repo

@pynja.project
class test_vector(repo.CppProject):
    def emit(self):
        libgtest = self.add_cpplib_dependency('gtest', 'sta')
        libnstd = self.add_cpplib_dependency('nstd', 'sta')

        self.includePaths.append(os.path.join(libgtest.projectDir, "include"))
        self.includePaths.append(os.path.join(libnstd.projectDir, "inc"))

        sources = [
            "src/test_utility.cpp",
            "src/test_vector_main.cpp",
        ]

        with self.cpp_compile_ex(sources) as tasks:
            pass

        self.make_executable("test_vector")
