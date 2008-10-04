from SCons.Options import Options

def create_options(filename, args):
    opts = Options(filename, args)
    opts.AddOptions(
        ('MOTO_GUI', 'Name of the target file of moto gui application', 'moto-gui'),
        ('CFLAGS', 'C-compiler flags', ''),
        ('CPPFLAGS', 'Defines', ''),
        ('LIBS', 'Libs to link with', ''),
        ('PKG_CONFIG', 'On posix just pkg-config call', ''),
        )
    return opts
