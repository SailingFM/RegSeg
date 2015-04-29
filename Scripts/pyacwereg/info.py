""" This file contains defines parameters for nipy that we use to fill
settings in setup.py, the nipy top-level docstring, and for building the
docs.  In setup.py in particular, we exec this file, so it cannot import nipy
"""


# nipy version information.  An empty _version_extra corresponds to a
# full release.  '.dev' as a _version_extra string means this is a development
# version
_version_major = 1
_version_minor = 5
_version_micro = 4
_version_extra = ''


def get_pyacwereg_gitversion():
    """pyacwereg version as reported by the last commit in git

    Returns
    -------
    None or str
      Version of pyacwereg according to git.
    """
    import os
    import subprocess
    try:
        import pyacwereg
        gitpath = os.path.realpath(os.path.join(os.path.dirname(pyacwereg.__file__),
                                                os.path.pardir))
    except:
        gitpath = os.getcwd()
    gitpathgit = os.path.join(gitpath, '.git')
    if not os.path.exists(gitpathgit):
        return None
    ver = None
    try:
        o, _ = subprocess.Popen('git describe', shell=True, cwd=gitpath,
                                stdout=subprocess.PIPE).communicate()
    except Exception:
        pass
    else:
        ver = o.strip().split('-')[-1]
    return ver

if '.dev' in _version_extra:
    gitversion = get_pyacwereg_gitversion()
    if gitversion:
        _version_extra = '.' + gitversion + '-' + 'dev'

# Format expected by setup.py and doc/source/conf.py: string of form "X.Y.Z"
__version__ = "%s.%s.%s%s" % (_version_major,
                              _version_minor,
                              _version_micro,
                              _version_extra)

CLASSIFIERS = ["Development Status :: 5 - Production/Stable",
               "Environment :: Console",
               "Intended Audience :: Science/Research",
               "License :: OSI Approved :: BSD License",
               "Operating System :: OS Independent",
               "Programming Language :: Python",
               "Topic :: Scientific/Engineering"]

description = 'Neuroimaging in Python: Pipelines and Interfaces'

# Note: this long_description is actually a copy/paste from the top-level
# README.txt, so that it shows up nicely on PyPI.  So please remember to edit
# it only in one place and sync it correctly.
long_description = \
    """
========================================================
RegSeg
========================================================


"""

# versions
NIBABEL_MIN_VERSION = '1.0'
NETWORKX_MIN_VERSION = '1.0'
NUMPY_MIN_VERSION = '1.3'
SCIPY_MIN_VERSION = '0.7'
TRAITS_MIN_VERSION = '4.0'
DATEUTIL_MIN_VERSION = '1.0'
NOSE_MIN_VERSION = '1.0'
DIPY_MIN_VERSION = '0.8'
NIPYPE_MIN_VERSION = '0.10'

NAME = 'pyacwereg'
MAINTAINER = "Oscar Esteban"
MAINTAINER_EMAIL = "code@oscaresteban.es"
DESCRIPTION = description
LONG_DESCRIPTION = long_description
URL = "https://github.com/oesteban/RegSeg"
DOWNLOAD_URL = "https://github.com/oesteban/RegSeg/tags"
LICENSE = "BSD license"
CLASSIFIERS = CLASSIFIERS
AUTHOR = "Oscar Esteban"
AUTHOR_EMAIL = "code@oscaresteban.es"
PLATFORMS = "OS Independent"
MAJOR = _version_major
MINOR = _version_minor
MICRO = _version_micro
ISRELEASE = _version_extra == ''
VERSION = __version__
REQUIRES = ["nibabel>=1.0", "numpy>=1.3", "scipy>=0.7", "dipy>=0.8",
            "nipype>=0.10"]
STATUS = 'stable'
