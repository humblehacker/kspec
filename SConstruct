import os

env = Environment()
debug = ARGUMENTS.get('debug', 1)
if int(debug):
    env.Append(CCFLAGS = '-O0 -g')
    env.Append(LINKFLAGS = '-g')

sourcefiles = ['Parser.cpp', 
               'Scanner.cpp', 
               'keyboard.cc',
               'hid_usages.cc',
               'lua_helpers.cc',
               'keyboard_visitor.cc',
               'lua_keyboard_visitor.cc',
               'binding.cc',
               'utils.cc',
               'main.cc']

env.Program(target = 'kspec', 
            source = sourcefiles,
            ENV    = {'PATH' : os.environ['PATH']},
            LIBS   = ['lua', 
                      'dl',
                      'boost_system-mt', 
                      'boost_filesystem-mt',
                      'boost_program_options-mt'])

env.Command(target = ['Parser.cpp', 'Parser.h', 'Scanner.cpp', 'Scanner.h'],
            source = 'kspec.atg',
            action = "coco $SOURCE")
