import os, pynja, repo

@pynja.project
class test_function(repo.CppProject):
    def emit(self):
        libnstd = self.add_cpplib_dependency('nstd', 'sta')

        self.includePaths.append(os.path.join(libnstd.projectDir, "inc"))

        sources = [
            "src/test_function.cpp",
        ]
        self.cpp_compile(sources)
        self.make_executable("test_function")
