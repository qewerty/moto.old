import sys, os
from tools.mototools import create_options
from tools import msg

SourceSignatures('MD5')
TargetSignatures('content')

platform = sys.platform

config = ARGUMENTS.get('config', 'config/%s.py' % platform)
if not os.path.exists(config):
    msg.error('Config file "%s" does not exist.' % config)
    sys.exit(-1)
if os.path.isdir(config):
    msg.error('Config file "%s" is a directory.' % config)
    sys.exit(-1)
msg.info('Using config file "%s"' % config)

build_dir = os.path.join('build', os.path.basename(config).split(os.extsep)[0])

cpppath = ['.', '..']

opts = create_options(config, ARGUMENTS)
env = Environment(options = opts,
                  ENV = {'PATH': os.environ['PATH']},
                  CPPPATH=cpppath)
Export('env')

pkg_config = env['PKG_CONFIG']
if pkg_config:
    if type(pkg_config) is str:
        env.ParseConfig(pkg_config)
    elif type(pkg_config) in (tuple, list):
        for c in pkg_config:
            env.ParseConfig(c)

Help(opts.GenerateHelpText(env))

env.SConscript('src/SConscript',
               build_dir=build_dir)

def make_tags(target, source, env):
    if env['PLATFORM'] is not 'posix':
        # TODO: print smth
        return

    filename = '.pkg-config~'
    os.system(pkg_config + ' > "%s"' % filename)
    f = file(filename)
    includes = f.read().replace('-I', '').split(' ')
    f.close()
    os.unlink(filename)

    cmd = 'ctags-exuberant -R src %s' % \
        (' '.join([i for i in includes if os.path.exists(i)]))
    print cmd
    os.system(cmd)

env.Command('tags', None, make_tags)
