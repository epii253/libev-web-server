QT -= gui

CONFIG += c++1z console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++1z

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        get_command_class.cpp \
        main.cpp \
        post_command_class.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -lev

HEADERS += \
    fn.h \
    get_command_class.h \
    message_class.h \
    post_command_class.h \
    template_phrases_namespace.h
