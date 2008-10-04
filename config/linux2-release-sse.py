MOTO_GUI = 'moto-gui'

CFLAGS  = ['-O2', '-pipe', '-ffast-math', '-Wall', '-march=i686', '-msse', '-msse2', '-mfpmath=sse,387']
LIBS    = ['GL', 'GLU', 'GLEW']

PKG_CONFIG = 'pkg-config gtk+-2.0 gtkglext-1.0 gio-2.0 gthread-2.0 --cflags --libs'

