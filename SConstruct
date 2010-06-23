
if ARGUMENTS.get('debug', 0):
    env = Environment(CCFLAGS = '-g')
else:
    env = Environment()

env.Program(target='kspec', 
            source=['main.cc', 
                    'Args.cc',
                    'kspec_parser.y', 
                    'kspec_scanner.l'],
            LIBS=['fl'])



