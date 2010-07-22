import os

env = Environment()
debug = ARGUMENTS.get('debug', 0)
if int(debug):
    env.Append(CCFLAGS = '-g')
    env.Append(LINKFLAGS = '-g')

sourcefiles = ['Parser.cpp', 
               'Scanner.cpp', 
               'keyboard.cc',
               'hid_usages.cc',
               'utils.cc',
               'options.cc', 
               'main.cc']

env.Program(target = 'kspec', 
            source = sourcefiles,
            ENV    = {'PATH' : os.environ['PATH']},
            LIBS   = [''])

env.Command(target = ['Parser.cpp', 'Parser.h', 'Scanner.cpp', 'Scanner.h'],
            source = 'kspec.atg',
            action = "coco $SOURCE")

