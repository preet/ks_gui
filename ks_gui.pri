INCLUDEPATH += $${PWD}

# ks
PATH_KS_GUI = $${PWD}/ks/gui

# win
HEADERS += \
    $${PATH_KS_GUI}/KsGuiConfig.hpp \
    $${PATH_KS_GUI}/KsGuiPlatform.hpp \
    $${PATH_KS_GUI}/KsGuiApplication.hpp \
    $${PATH_KS_GUI}/KsGuiScreen.hpp \
    $${PATH_KS_GUI}/KsGuiWindow.hpp \
    $${PATH_KS_GUI}/KsGuiInput.hpp

SOURCES += \
    $${PATH_KS_GUI}/KsGuiPlatform.cpp \
    $${PATH_KS_GUI}/KsGuiApplication.cpp \
    $${PATH_KS_GUI}/KsGuiScreen.cpp \
    $${PATH_KS_GUI}/KsGuiWindow.cpp
