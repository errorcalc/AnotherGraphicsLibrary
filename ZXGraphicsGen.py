#FIXME add Exceptions and etc...
# sorry, your eyes can take damage while reading this script

import re

src = open('zxGraphics.c', 'r')
dst = open('zxGraphicsBitBlit.h', 'w')

def gen():
    dst.write('/* WARING: DO NOT EDIT THIS CODE, THIS CODE IS AUTO GENERATED,\n')
    dst.write('   SEE ''zxGraphicsGen.py''\n\n')
    dst.write('   Copyright © 2018 errorsoft. All rights reserved. */\n\n')
    state = 'search'
    for s in src:
        if state == 'search':
            se = re.search(r'BEGIN \$.*\$', s)
            if se != None:
                name = re.search(r'\$.*\$', s).group(0)[1:-1]
                endName = re.search(r'\w*', name).group(0)
                endName = re.escape('$'+endName+'$')
                print('add macro: ' +name)
                state = 'search{'
        elif state == 'search{':
            if re.search(r'{', s) != None:
                dst.write('#define ' + name + ' \\\n')
                dst.write('do {')
                state = 'copy'
        elif state == 'copy':
            if re.search(r'END ' + endName, s) == None:
                dst.write(' \\\n')

                se = re.search(r'\$.*\$', s)
                if se != None:
                    dst.write(' '*re.search(r'\S', s).start() + se.group(0)[1:-1] + ';')
                else:
                    dst.write(s.rstrip())
            else:
                dst.write(' while(0)\n\n')
                state = 'search'

gen()

print('ok')

src.close()
dst.close()