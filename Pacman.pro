TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    pacman.cpp \
    enemy.cpp \
    game.cpp \
    entity.cpp \
    fruit.cpp \
    allegro5initializer.cpp \
    allegro5timer.cpp \
    allegro5sample.cpp \
    allegro5font.cpp \
    allegro5bitmap.cpp

LIBS += `pkg-config --libs allegro-5.0 allegro_dialog-5.0 allegro_ttf-5.0 allegro_font-5.0 allegro_primitives-5.0 allegro_image-5.0 allegro_audio-5.0 allegro_acodec-5.0`

HEADERS += \
    game.hpp \
    enemy.hpp \
    pacman.hpp \
    entity.hpp \
    fruit.hpp \
    resourcemanager.hpp \
    allegro5initializer.hpp \
    allegro5timer.hpp \
    allegro5sample.hpp \
    allegro5font.hpp \
    allegro5bitmap.hpp \
    resourceindentifiers.hpp \
    my_untils.hpp

DISTFILES += \
    resourcemanager.impl
