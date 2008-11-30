MOTO_GUI = 'moto-gui'

CC  = 'gcc'
CXX = 'g++'

CFLAGS  = ['-O3', '-pipe', '-ffast-math', '-Wall',
           '-march=i686', '-msse', '-msse2', '-mfpmath=sse',
           '-fopenmp']
LIBS    = ['gomp', 'GL', 'GLU', 'GLEW']

CPPFLAGS = ['-DMOTO_WITH_WOBJ_MESH_LOADER',
            # '-DMOTO_MBM_MESH_LOADER',
            # '-DMOTO_RIB_MESH_LOADER',
            ]

PKG_CONFIG = 'pkg-config gtk+-2.0 gtkglext-1.0 gio-2.0 gthread-2.0 --cflags --libs'

PLUGINS = []

