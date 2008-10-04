from SCons.Options import Options

def create_options(filename, args):
    opts = Options(filename, args)
    opts.AddOptions(
        ('MOTO_GUI', 'Name of the target file of moto gui application', 'moto-gui'),
        ('PLUGINS', 'List of plugins to build (from src/plugins)', ''),
        ('CC', 'The C compiler', ''),
        ('CXX', 'The C++ compiler', ''),
        ('CFLAGS', 'C-compiler flags', ''),
        ('CPPFLAGS', 'Defines', ''),
        ('LIBS', 'Libs to link with', ''),
        ('PKG_CONFIG', 'On posix just pkg-config call, on windows should be empty', ''),
        )
    return opts
