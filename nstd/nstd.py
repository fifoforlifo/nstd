import pynja, repo, os

@pynja.project
class nstd(repo.CppProject):
    def emit(self):
        self.includePaths.append(os.path.join(self.projectDir, 'inc'))

        sources = [
            "src/byte_pool_default.cpp",
            "src/nstd.cpp",
        ]
        self.cpp_compile(sources)
        self.make_static_lib("nstd")
