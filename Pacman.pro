TEMPLATE = app
#CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    pacman.cpp \
    food.cpp \
    enemy.cpp \
    character.cpp

LIBS += `pkg-config --libs allegro-5.0 allegro_dialog-5.0 allegro_ttf-5.0 allegro_font-5.0 allegro_primitives-5.0 allegro_image-5.0 allegro_audio-5.0 allegro_acodec-5.0`

QMAKE_CXXFLAGS += -std=gnu++14

HEADERS += \
    pacman.h \
    food.h \
    enemy.h \
    character.h
