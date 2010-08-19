import os

libs = ['lua', 
        'usb-1.0',
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
#   env.Append(LINKFLAGS = '-static')

debug = ARGUMENTS.get('debug', 1)
if int(debug):
    env.Append(CCFLAGS = '-O0 -g')
    env.Append(LINKFLAGS = '-g')

sourcefiles = ['Parser.cpp', 
               'Scanner.cpp', 
               'keyboard.cc',
               'hid_usages.cc',
               'generate.cc',
               'program.cc',
               'lua_helpers.cc',
               'keyboard_visitor.cc',
               'lua_keyboard_visitor.cc',
               'binding.cc',
               'utils.cc',
               'usb.cc',
               'usbexception.cc',
               'main.cc']

env.Program(target = 'kspec', 
            source = sourcefiles,
            ENV    = {'PATH' : os.environ['PATH']},
            LIBS   = libs)

env.Command(target = ['Parser.cpp', 'Parser.h', 'Scanner.cpp', 'Scanner.h'],
            source = 'kspec.atg',
            action = "coco $SOURCE")
