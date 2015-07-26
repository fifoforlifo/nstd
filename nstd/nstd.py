import pynja
import repo


@pynja.project
class nstd(repo.CppProject):
    def emit(self):
        sources = [
            "src/nstd.cpp",
        ]
        self.cpp_compile(sources)
        self.make_static_lib("nstd")
