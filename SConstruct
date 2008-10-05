import sys, os
from tools.mototools import create_options
from tools import msg

SourceSignatures('MD5')
TargetSignatures('content')

platform = sys.platform

config = ARGUMENTS.get('config', 'config/%s-release.py' % platform)
if not os.path.exists(config):
    msg.error('Config file "%s" does not exist.' % config)
    sys.exit(-1)
if os.path.isdir(config):
    msg.error('Config file "%s" is a directory.' % config)
    sys.exit(-1)
msg.info('Using config file "%s"' % config)

build_dir = os.path.join('build', os.path.basename(config).split(os.extsep)[0])

cpppath = ['..']

opts = create_options(config, ARGUMENTS)
env = Environment(options = opts,
                  ENV = {'PATH': os.environ['PATH']},
                  CPPPATH=cpppath)
Export('env')

pkg_config = env['PKG_CONFIG']
if pkg_config:
    env.ParseConfig(pkg_config)

Help(opts.GenerateHelpText(env))

env.SConscript('src/SConscript',
               build_dir=build_dir)
