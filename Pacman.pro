TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    pacman.cpp \
    enemy.cpp \
    game.cpp \
    entity.cpp \
    fruit.cpp

LIBS += `pkg-config --libs allegro-5.0 allegro_dialog-5.0 allegro_ttf-5.0 allegro_font-5.0 allegro_primitives-5.0 allegro_image-5.0 allegro_audio-5.0 allegro_acodec-5.0`

HEADERS += \
    game.hpp \
    enemy.hpp \
    pacman.hpp \
    entity.hpp \
    fruit.hpp
