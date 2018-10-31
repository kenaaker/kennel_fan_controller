QT -= gui
QT += core
QT += network
QT += xml

TARGET = kennel_fan_controller
TEMPLATE = app

CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG += debug

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lavahi-common -lavahi-client

SOURCES += \
        kennel_fan_controller_main.cpp \
        ../kennel_fan_controller.cpp \
        ../pwm_fan.cpp \
        ../gpio_tach.cpp \
        ../lin_gpio.cpp \
        ../lin_pwm.cpp \
        ../lin_gpio_util.cpp \
        ../kennel_fan_sslsock.cpp \
        ../zconfserviceclient.cpp \
        ../zconfservice.cpp \
        ../qt-watch.cpp \
        ../gpio_rotary.cpp

HEADERS += \
        ../kennel_fan_controller.h \
        ../pwm_fan.h \
        ../gpio_tach.h \
        ../lin_gpio.h \
        ../lin_pwm.h \
        ../lin_gpio_util.h \
        ../kennel_fan_sslsock.h \
        ../zconfserviceclient.h \
        ../zconfservice.h \
        ../qt-watch.h \
        ../gpio_rotary.h


# Default rules for deployment.
target.path = /tmp/$${TARGET}/bin
INSTALLS += target
