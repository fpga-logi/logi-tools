from distutils.core import setup, Extension
 
module1 = Extension('logi', sources = ['logi.c', '../../c/logibonelib.c'], include_dirs=['../../c'])
 
setup (name = 'PackageName',
        version = '1.0',
        description = 'This is a demo package',
        ext_modules = [module1])
