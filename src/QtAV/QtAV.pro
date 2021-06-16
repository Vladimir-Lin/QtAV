NAME         = QtAV
TARGET       = $${NAME}

QT           = core
QT          += gui
QT          += network
QT          += sql
QT          += script
QT          += positioning
QT          += widgets
QT          += opengl
QT          += printsupport
QT          += multimedia
QT          += multimediawidgets
QT          += QtCUDA
QT          += QtOpenCV
QT          += Essentials
QT          += QtCalendar
QT          += QtGMP
QT          += QtGSL
QT          += QtAlgebra
QT          += QtDiscrete
QT          += QtFFT
QT          += Mathematics
QT          += QtFuzzy
QT          += QtFLP
QT          += QtFoundation
QT          += QtGeometry
QT          += QtGadgets
QT          += QtComponents
QT          += QtManagers
QT          += QtWorld
QT          += QtVCF
QT          += QtPhotography
QT          += DataWidgets
QT          += MathWidgets
QT          += QtModels
QT          += QtTasks
QT          += QtHuman

load(qt_build_config)
load(qt_module)

INCLUDEPATH += $${PWD}/../../include/$${NAME}

HEADERS     += $${PWD}/../../include/$${NAME}/qtav.h

include ($${PWD}/Supports/Supports.pri)
include ($${PWD}/Audios/Audios.pri)
include ($${PWD}/Videos/Videos.pri)
include ($${PWD}/Widgets/Widgets.pri)
include ($${PWD}/World/World.pri)
include ($${PWD}/Universe/Universe.pri)
include ($${PWD}/Scene/Scene.pri)
include ($${PWD}/Obsolete/Obsolete.pri)

OTHER_FILES += $${PWD}/../../include/$${NAME}/headers.pri

include ($${PWD}/../../doc/Qt/Qt.pri)

TRNAME       = $${NAME}
include ($${PWD}/../../Translations.pri)

win32 {
LIBS        += -lavcodec
LIBS        += -lavdevice
LIBS        += -lavfilter
LIBS        += -lavformat
LIBS        += -lavutil
LIBS        += -lpostproc
LIBS        += -lswresample
LIBS        += -lswscale
LIBS        += -lglu32
LIBS        += -lopengl32
}
