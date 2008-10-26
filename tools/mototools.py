import os

from SCons.Options import Options, ListOption

PLUGIN_PREFIX = 'moto-'

def is_a_plugin_dir(dirname):
    return os.path.basename(dirname).startswith(PLUGIN_PREFIX)

def plugin_name(path):
    return os.path.basename(path).replace(PLUGIN_PREFIX, '')

def find_plugins():
    plugins_dir = \
        os.path.realpath(os.path.join(os.path.dirname(os.path.realpath(__file__)),
            '..', 'src', 'plugins'))
    plugins = [plugin_name(f) for f in os.listdir(plugins_dir) if os.path.isdir(os.path.join(plugins_dir, f)) and is_a_plugin_dir(os.path.join(plugins_dir, f))]
    return plugins

def create_options(filename, args):
    opts = Options(filename, args)
    opts.AddOptions(
        ('MOTO_GUI',    'Name of the target file of moto gui application', 'moto-gui'),
        ListOption('PLUGINS', 'List of plugins to build (from %s%ssrc%splugins)' % (os.pardir, os.sep, os.sep), [],
            find_plugins()),
        ('CC',          'The C compiler', ''),
        ('CXX',         'The C++ compiler', ''),
        ('CFLAGS',      'C-compiler flags', ''),
        ('CXXFLAGS',    'C++-compiler flags', ''),
        ('CCFLAGS',     'C/C++-compiler flags', ''),
        ('CPPFLAGS',    'Defines', ''),
        ('LIBS',        'Libs to link with', ''),
        ('PKG_CONFIG',  'On posix just pkg-config call, on windows should be empty', ''),
        )
    return opts
