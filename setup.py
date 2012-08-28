from distutils.core import setup
from distutils.extension import Extension

setup(
    name = "glxcontext",
    version = "0.1",
    author = "Stephan Wenger",
    author_email = "wenger@cg.cs.tu-bs.de",
    description = "Create a GLX context.",
    license = "MIT",
    ext_modules = [
        Extension("glxcontext", ["glxcontext.cpp"],
            libraries=["boost_python", "X11", "GL"],
            define_macros=[("PYTHON_WRAPPER", None)],
        )
    ]
)

