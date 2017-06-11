from distutils.core import setup, Extension

# Emulator core (libgb) sources
sources = ['mem.c', 'cpu.c', 'boot.c', 'apu.c', 'cart.c', 'gameboy.c', 'lcd.c']
libgb_sources = map(lambda x: "src/core/"+x, sources)
libgb = ('gb', {'sources': libgb_sources})

pygb = Extension('pygb',
    sources = ['src/extension/pygb.c'],
    include_dirs = ['include'],
    libraries = ['gb'],
    library_dirs = []
    )

setup(name = 'pygb',
    version = '1.0',
    description = 'Game Boy emulator',
    libraries = [libgb],
    ext_modules = [pygb])