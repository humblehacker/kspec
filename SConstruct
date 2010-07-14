import os

if ARGUMENTS.get('debug', 0):
    env = Environment(CCFLAGS = '-g')
else:
    env = Environment()

env.Program(target='kspec', 
            source=['Scanner.cpp',
                    'Parser.cpp',
                    'options.cc', 
                    'main.cc'],
            ENV = {'PATH' : os.environ['PATH']},
            LIBS=[''])


bld = Builder(action = 'coco $SOURCE $TARGET')

