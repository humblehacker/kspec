import os

if ARGUMENTS.get('debug', 0):
    env = Environment(CCFLAGS = '-g')
else:
    env = Environment()

env.Program(target = 'kspec', 
            source = ['Parser.cpp', 'Scanner.cpp', 'options.cc', 'main.cc'],
            ENV    = {'PATH' : os.environ['PATH']},
            LIBS   = [''])

env.Command(target = ['Parser.cpp', 'Parser.h', 'Scanner.cpp', 'Scanner.h'],
            source = 'kspec.atg',
            action = "coco $SOURCE")

