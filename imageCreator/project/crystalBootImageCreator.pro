#-------------------------------------------------
#
# Project created by QtCreator 2015-05-07T17:15:19
#
#-------------------------------------------------



TEMPLATE = subdirs

SUBDIRS = src
SUBDIRS += app
SUBDIRS += tests

#SUBDIRS += comModules/mocklayer/appPlugin

#src.depends = comModules/mocklayer/appPlugin
app.depends = src
tests.depends = src

