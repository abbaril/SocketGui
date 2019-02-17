#-------------------------------------------------
#
# Project created by QtCreator 2016-08-29T20:51:40
#
# Bryan Cairns
# voidrealms.com
# ...Because why not...
#
# Seige command:
# siege 127.0.0.1:2000 -c 1500
#
# TCP Server that allows different thread models
# 1 - One Thread - all connections on one sever thread
# 2 - ThreadPools - connection share a command group of threads
# 3 - Threaded - one thread per connection
# for Linux hard Limits
# modify /etc/security/limits.conf
# add:
# *               soft    nofile            65536
# *               hard    nofile            65536
# then restart
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SocketGUI6
TEMPLATE = app

include("src/servers/tcp/tcpserver.pri")
include("src/servers/http/httpserver.pri")

SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

