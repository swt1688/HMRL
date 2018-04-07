#-------------------------------------------------
#
# Project created by QtCreator 2018-02-09T13:27:00
#
#-------------------------------------------------

QT       += core gui network sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HMRL_ERPServer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG +=  c++11    #CONFIG-->指定编译器选项和项目配置，值由qmake内部识别并具有特殊意义。

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



SOURCES += \
        main.cpp \
        spollserver.cpp \
    DBProcess/DBProcess.cpp \
    networks/mytcpclient.cpp \
    networks/mytcpserver.cpp \
    Protocol/protocolclass.cpp \
    Protocol/s7_200_pcppi.cpp \
    Protocol/verifychecksum.cpp \
    globalinfo.cpp \
    DBProcess/dbconnectionpool.cpp

HEADERS += \
        spollserver.h \
    databaseinit.h \
    DBProcess/DBProcess.h \
    networks/mytcpclient.h \
    networks/mytcpserver.h \
    Protocol/citycode.h \
    Protocol/protocolclass.h \
    Protocol/s7_200_pcppi.h \
    Protocol/verifychecksum.h \
    globalinfo.h \
    helperclass.h \
    DBProcess/dbconnectionpool.h

#FORMS += \
#        spollserver.ui

RESOURCES += \
    qss.qrc

RC_ICONS = ICON\tree.ico
# 定义输出路径
win32{

    CONFIG += debug_and_release    #项目准备以debug和release两种模式构建。
    CONFIG(release, debug|release) {
            target_path = ./release
        } else {
            target_path = ./debug
        }
        DESTDIR = ../bin
        MOC_DIR = $$target_path/moc #MOC_DIR指定来自moc的所有中间文件放置的目录(含Q_OBJECT宏的头文件转换成标准.h文件的存放目录)。
        RCC_DIR = $$target_path/rcc #RCC_DIR指定Qt资源编译器输出文件的目录（.qrc文件转换成qrc_*.h文件的存放目录）。
        UI_DIR = $$target_path/ui   #UI_DIR指定来自uic的所有中间文件放置的目录（.ui文件转化成ui_*.h文件的存放目录）。
        OBJECTS_DIR = $$target_path/obj #OBJECTS_DIR指定所有中间文件.o（.obj）放置的目录。

}

# 输出编译套件信息
message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(the HMRL_ERPServer will create in folder: $$target_path)
