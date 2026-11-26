QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    accionguardia.cpp \
    agenteguardia.cpp \
    aprendizaje.cpp \
    armaoscilante.cpp \
    cadaver.cpp \
    enemigo.cpp \
    entidad.cpp \
    extintor.cpp \
    gestorjuego.cpp \
    granada.cpp \
    guardia.cpp \
    itemfigura.cpp \
    itemguardia.cpp \
    itempersonaje.cpp \
    itemrambo.cpp \
    main.cpp \
    mainwindow.cpp \
    nivel.cpp \
    nivel1.cpp \
    nivel2.cpp \
    nubehumo.cpp \
    ondaruido.cpp \
    percepcion.cpp \
    piedra.cpp \
    policia.cpp \
    proyectil.cpp \
    rambo.cpp \
    razonamiento.cpp \
    suministro.cpp

HEADERS += \
    accionguardia.h \
    agenteguardia.h \
    agenteinteligente.h \
    aprendizaje.h \
    armaoscilante.h \
    cadaver.h \
    decision.h \
    enemigo.h \
    entidad.h \
    estimulo.h \
    extintor.h \
    gestorjuego.h \
    granada.h \
    guardia.h \
    itemfigura.h \
    itemguardia.h \
    itempersonaje.h \
    itemrambo.h \
    mainwindow.h \
    modeloarrastre.h \
    modelofisico.h \
    modeloonda.h \
    modelooscilatorio.h \
    modeloparabolico.h \
    modelorebote.h \
    nivel.h \
    nivel1.h \
    nivel2.h \
    nubehumo.h \
    ondaruido.h \
    percepcion.h \
    piedra.h \
    policia.h \
    proyectil.h \
    puntosospecha.h \
    rambo.h \
    razonamiento.h \
    suministro.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    recursos.qrc
