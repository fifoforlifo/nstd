import pynja
import repo


@pynja.project
class test_function(repo.CppProject):
    def emit(self):
        libnstd = self.add_cpplib_dependency('nstd', 'sta')
        sources = [
            "test_function.cpp",
        ]
        self.cpp_compile(sources)
        self.make_executable("test_function")
