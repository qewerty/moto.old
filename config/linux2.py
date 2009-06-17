MOTO_GUI = 'moto-gui'

CC  = 'gcc'
CXX = 'g++'

CFLAGS  = ['-O3', '-pipe', '-ffast-math', '-Wall', '-march=i686',
           '-fopenmp']
LIBS    = ['gomp', 'GL', 'GLU', 'GLEW']

CPPFLAGS = ['-DMOTO_WITH_WOBJ_MESH_LOADER',
            # '-DMOTO_MBM_MESH_LOADER',
            # '-DMOTO_RIB_MESH_LOADER',
            ]

PKG_CONFIG = ['pkg-config gtk+-2.0 libglade-2.0 gtkglext-1.0 gio-2.0 gthread-2.0 --cflags --libs',
              'python2.5-config --cflags', 'python2.5-config --libs']

PLUGINS = []

