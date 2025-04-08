from conan import ConanFile
from conan.tools.files import copy
import os

class FastNoiseLiteConan(ConanFile):
    name = "fastnoise-lite"
    version = "1.1.1"
    description = "FastNoiseLite is a noise generation library"
    license = "MIT"
    url = "https://github.com/Auburn/FastNoiseLite"
    homepage = "https://github.com/Auburn/FastNoiseLite"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "include/*"
    package_type = "header-library"
    no_copy_source = True

    def package(self):
        copy(self, "FastNoiseLite.h", src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"))

    def package_id(self):
        self.info.clear()  # header-only
