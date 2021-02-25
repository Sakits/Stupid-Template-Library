#!/usr/bin/python3

import os

name = ('one', 'two', 'three', 'four', 'five')

pwd = ''


def run_Case(id):
    os.system('pwd')
    os.system('g++ -O2 -std=c++11 code.cpp -I../.. -o code')
    os.system('./code > my.out')
    return os.system('diff -w my.out answer.txt') == 0


def run_Case_memcheck(id):
    os.system('g++ -O2 -std=c++11 code.cpp -I../.. -o code')
    os.system('./code > my.out')
    return os.system('diff -w my.out answer.txt') == 0


def Clean():
    os.system('rm my.out')
    os.system('rm code')


def run():
    os.chdir(os.getcwd() + '/data')

    for i in range(5):

        os.chdir(os.getcwd() + '/' + name[i])

        if run_Case(i):
            print('Case ' + name[i] + ' Accepted!')
            Clean()
        else:
            print('WA on Case ' + name[i])
            Clean()
            break

        os.chdir(os.getcwd() + '/..')

        os.chdir(os.getcwd() + '/' + name[i] + '.memcheck')

        if run_Case_memcheck(i):
            print('Case_memcheck ' + name[i] + ' Accepted!')
            Clean()
        else:
            print('WA on Case_memcheck ' + name[i])
            Clean()
            break

        os.chdir(os.getcwd() + '/..')

    else:
        print('Passed all tests!')


run()
