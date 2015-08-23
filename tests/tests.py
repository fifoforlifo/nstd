import os, pynja, repo

pynja.import_subdir_file('function/test_function.py')
pynja.import_subdir_file('type_erasure/test_type_erasure.py')
pynja.import_subdir_file('vector/test_vector.py')
pynja.import_subdir('bench_func')

@pynja.project
class tests(repo.DeployProject):
    def emit(self):
        productDir = os.path.join(pynja.rootPaths.bin, 'tests', str(self.variant))

        if os.name == 'nt':
            variant_os = 'windows'
        elif os.name == 'posix':
            variant_os = 'linux'
        else:
            raise Exception("Unsupported OS %s" % os.name)

        primaryVariantStr = "%s-%s-%s-%s-dcrt" % (variant_os, self.variant.toolchain, self.variant.arch, self.variant.config)
        primaryVariant = repo.CppVariant(primaryVariantStr)

        test_names = [
            'test_function',
            'test_type_erasure',
            'test_vector',
            'bench_func',
        ]
        for test_name in test_names:
            self.add_runtime_dependency_project(self.get_project(test_name, primaryVariant))

        self.deploy(productDir)

        # do this last, to capture referenced projects
        self.projectMan.add_cb_project_root(self)
