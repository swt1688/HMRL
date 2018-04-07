#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T21:31:11
#
#-------------------------------------------------
#QT-->指定项目中使用Qt的模块。默认情况下，QT包含core和gui，以确保标准的GUI应用程序无需进一步的配置就可以构建。
#如果想建立一个不包含Qt GUI模块的项目，可以使用“ -=”操作符。
QT      += core gui serialport sql multimedia network axcontainer printsupport xml 3dinput

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HMRL_ERPClient#TARGET指定目标文件的名称。默认情况下包含的项目文件的基本名称。



#TEMPLATE-->模板变量告诉qmake为这个应用程序生成哪种makefile。可供使用的选项如下:
#app创建一个用于构建应用程序的Makefile（默认）。
#lib创建一个用于构建库的Makefile。
#subdirs创建一个用于构建目标子目录的Makefile，子目录使用SUBDIRS变量指定。
#aux创建一个不建任何东西的Makefile。如果没有编译器需要被调用来创建目标，比如你的项目使用解释型语言写的，使用此功能。注：此模板类型只能用于Makefile-based生成器。特别是，它不会工作在vcxproj和Xcode生成器。
#vcapp仅适用于Windows。创建一个Visual Studio应用程序项目。
#vclib仅适用于Windows。创建一个Visual Studio库项目。
TEMPLATE = app
CONFIG +=  c++11    #CONFIG-->指定编译器选项和项目配置，值由qmake内部识别并具有特殊意义。

SOURCES += main.cpp\
    maininterface.cpp \
    DataInfo/datainfo.cpp \
    DataInfo/mapwidget.cpp \
    LogInfo/logininfo.cpp \
    MyCalendar/DefDateTimeEdit.cpp \
    MyCalendar/DefineCalendar.cpp \
    qcustomplot/qcustomplot.cpp \
    QFrame/buttondelegate.cpp \
    QFrame/ccheckcombox.cpp \
    QFrame/customtabwidget.cpp \
    QFrame/navlistview.cpp \
    QFrame/picturemodify.cpp \
    QFrame/picturepreviewpanel.cpp \
    QFrame/tablemodel.cpp \
    QFrame/tableview.cpp \
    QFrame/viewdelegate.cpp \
    RemoteControl/remotecontrol.cpp \
    SystemMaintain/systemmaintain.cpp \
    aboutus.cpp \
    exportexecl.cpp \
    iconhelper.cpp \
    logoin.cpp \
    printview.cpp \
    remindinterface.cpp \
    DataAnalysis/dataanalysis.cpp

#指定项目中所有的头文件。qmake会自动检测是头文件的类中是否需要moc，并增加适当的依赖关系和文件到项目中，来生成和链接moc文件。
HEADERS  += \
    maininterface.h \
    DataInfo/datainfo.h \
    DataInfo/mapwidget.h \
    LogInfo/logininfo.h \
    MyCalendar/DefDateTimeEdit.h \
    MyCalendar/DefineCalendar.h \
    qcustomplot/qcustomplot.h \
    QFrame/buttondelegate.h \
    QFrame/ccheckcombox.h \
    QFrame/customtabwidget.h \
    QFrame/navlistview.h \
    QFrame/picturemodify.h \
    QFrame/picturepreviewpanel.h \
    QFrame/tablemodel.h \
    QFrame/tableview.h \
    QFrame/viewdelegate.h \
    RemoteControl/remotecontrol.h \
    SystemMaintain/systemmaintain.h \
    aboutus.h \
    exportexecl.h \
    helperclass.h \
    iconhelper.h \
    initdatabase.h \
    logoin.h \
    maininterface.h \
    printview.h \
    remindinterface.h \
    DataAnalysis/dataanalysis.h


FORMS    += \
    maininterface.ui \
    DataInfo/datainfo.ui \
    DataInfo/mapwidget.ui \
    LogInfo/logininfo.ui \
    QFrame/picturemodify.ui \
    RemoteControl/remotecontrol.ui \
    SystemMaintain/systemmaintain.ui \
    aboutus.ui \
    logoin.ui \
    maininterface.ui \
    remindinterface.ui \
    DataAnalysis/dataanalysis.ui


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
# LIBS-->指定链接到项目中的库列表。如果路径包含空格，需要使用引号包含路径。如果使用Unix -l (library) 和 -L (library path) 标志，在Windows上qmake正确处理库（也就是说，将库的完整路径传递给链接器），库必须存在，qmake会寻找-l指定的库所在的目录。
LIBS += -L$$OUT_PWD/../bin/ -lNBaseUiKit
include(../NBaseUiKit/NBaseUiKit_inc.pri)

# 输出编译套件信息
message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(the HMRL_ERPClient will create in folder: $$target_path)

RESOURCES += \
    qss.qrc
#RC_ICONS仅适用于Windows，指定的图标应该包含在一个生成的.rc文件里。如果RC_FILE 和RES_FILE变量都没有设置这才可利用。
RC_ICONS = tree.ico

DISTFILES +=
#DEFINES-->qmake添加这个变量的值作为编译器C预处理器宏(-D选项)。例如：
#DEFINES += QT_NO_DEBUG_OUTPUT
#然后就可以在代码中使用:
#ifdef USE_MY_STUFF
#    // TODO
#else
#    // TODO
#endif
#往往可以指定项目的特殊版本（比如：正式版、试用版）、对一些特殊功能模块（比如：加密狗）进行限制等。
