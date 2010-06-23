import os

if ARGUMENTS.get('debug', 0):
    env = Environment(CCFLAGS = '-g')
else:
    env = Environment()

env.Program(target='kspec', 
            source=['parser.yy', 
                    'scanner.lex.ll',
                    'main.cc', 
                    'options.cc',
                    'parser_context.cc',
                    'scanner.cc',
                    'astree.cc',
                    'auxlib.cc',
                    'hashtable.cc'],
            ENV = {'PATH' : os.environ['PATH']},
            LIBS=['fl'],
            YACCFLAGS='')

env.Clean('bison', ['parser.hh',
                    'stack.hh',
                    'position.hh',
                    'location.hh'])

env.Clean('flex', ['scanner.lex.h'])



