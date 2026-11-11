QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    armaoscilante.cpp \
    enemigo.cpp \
    entidad.cpp \
    main.cpp \
    mainwindow.cpp \
    nivel.cpp \
    nivel2.cpp \
    policia.cpp \
    proyectil.cpp \
    rambo.cpp \
    suministro.cpp

HEADERS += \
    armaoscilante.h \
    enemigo.h \
    entidad.h \
    mainwindow.h \
    modelofisico.h \
    modelooscilatorio.h \
    modeloparabolico.h \
    modelorebote.h \
    nivel.h \
    nivel2.h \
    policia.h \
    proyectil.h \
    rambo.h \
    suministro.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
