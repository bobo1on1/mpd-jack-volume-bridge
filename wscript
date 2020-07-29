#! /usr/bin/env python
# encoding: utf-8

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='mpd-jack-volume-bridge'

# these variables are mandatory ('/' are converted automatically)
top = '.'
out = 'build'

def options(opt):
  opt.load('compiler_cxx')

def configure(conf):
  conf.load('compiler_cxx')

  conf.check(header_name='jack/jack.h')

  conf.check(lib='m', uselib_store='m', mandatory=False)
  conf.check(lib='jack', uselib_store='jack')
  conf.check(lib='pthread', uselib_store='pthread', mandatory=False)

def build(bld):
  bld.program(source='src/main.cpp\
                      src/tcpsocket.cpp\
                      src/misc.cpp\
                      src/mpdclient.cpp\
                      src/jackclient.cpp\
                      src/volumebridge.cpp',
              use=['jack','m','pthread'],
              includes='./src',
              cxxflags='-Wall -g',
              target='mpd-jack-volume-bridge')

