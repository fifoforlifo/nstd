import os, pynja, repo

@pynja.project
class bench_func(repo.CppProject):
    def emit(self):
        libnstd = self.add_cpplib_dependency('nstd', 'sta')

        self.includePaths.append(os.path.join(libnstd.projectDir, "inc"))

        sources = [
            "src/bench_func.cpp",
        ]

        with self.cpp_compile_ex(sources) as tasks:
            pass

        self.make_executable("bench_func")
