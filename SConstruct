import os

PLATFORM = os.uname()[0].lower()

libs = ['lua', 
        'cairo',
        'cairomm-1.0',
        'boost_system-mt', 
        'boost_filesystem-mt',
        'boost_program_options-mt'] 

env = Environment()

if os.name == 'nt':
    env = Environment(tools = ['mingw'])
    home = os.environ['HOME']
    escape = env['ESCAPE']
    env['ENV']['PATH'] += ';' + escape(home) + '/bin'
    env['ENV']['USERPROFILE'] = home
    env.Append(CCFLAGS = '-I' + escape(home) + '/include')
    env.Append(LIBPATH = [home + '/lib'])
    env.Append(HOME = home)
elif os.name == 'posix':
    libs.append('dl')

if PLATFORM == 'darwin':
    archs = map(lambda x: ['-arch', x], ['i386', 'x86_64'])
    env.Append(
        CCFLAGS = archs,
        LINKFLAGS = archs
    )

debug = ARGUMENTS.get('debug', 1)
if int(debug):
    env.Append(CCFLAGS = '-O0 -g')
    env.Append(LINKFLAGS = '-g')

env.ParseConfig('pkg-config --cflags --libs sigc++-2.0 freetype2 fontconfig pixman-1 cairo cairomm-1.0')

sourcefiles = ['Parser.cpp', 
               'Scanner.cpp', 
               'keyboard.cc',
               'hid_usages.cc',
               'generate.cc',
               'layout.cc',
               'lua_helpers.cc',
               'keyboard_visitor.cc',
               'lua_keyboard_visitor.cc',
               'binding.cc',
               'utils.cc',
               'main.cc']

env.Program(target = 'kspec', 
            source = sourcefiles,
            ENV    = {'PATH' : os.environ['PATH']},
            LIBS   = libs)

env.Command(target = ['Parser.cpp', 'Parser.h', 'Scanner.cpp', 'Scanner.h'],
            source = 'kspec.atg',
            action = "coco $SOURCE")
