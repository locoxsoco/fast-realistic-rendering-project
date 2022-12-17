# Author: Marc Comino 2020

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ViewerPBS
TEMPLATE = app

CONFIG += c++14
CONFIG(release, release|debug):QMAKE_CXXFLAGS += -Wall -O2

CONFIG(release, release|debug):DESTDIR = release/
CONFIG(release, release|debug):OBJECTS_DIR = release/
CONFIG(release, release|debug):MOC_DIR = release/
CONFIG(release, release|debug):UI_DIR = release/

CONFIG(debug, release|debug):DESTDIR = debug/
CONFIG(debug, release|debug):OBJECTS_DIR = debug/
CONFIG(debug, release|debug):MOC_DIR = debug/
CONFIG(debug, release|debug):UI_DIR = debug/

INCLUDEPATH += '$$PWD/dependencies/eigen3'
INCLUDEPATH += '$$PWD/dependencies/glm'

# Using x64 architecture, comment if necessary
LIBS += -L'$$PWD/dependencies/glew/lib/Release/x64/' -lglew32 -lglu32 -lopengl32
LIBS += -L'$$PWD/dependencies/glew/bin/Release/x64/' -lglew32
# Using x86 architecture, uncomment if necessary
#LIBS += -L'$$PWD/dependencies/glew/lib/Release/Win32/' -lglew32 -lglu32 -lopengl32
#LIBS += -L'$$PWD/dependencies/glew/bin/Release/Win32/' -lglew32

INCLUDEPATH += '$$PWD/dependencies/glew/include'
INCLUDEPATH += '$$PWD/dependencies/glew/lib/Release/x64'
DEPENDPATH += '$$PWD/dependencies/glew/lib/Release/x64'
#INCLUDEPATH += '$$PWD/dependencies/glew/lib/Release/Win32'
#DEPENDPATH += '$$PWD/dependencies/glew/lib/Release/Win32'

SOURCES += \
    main.cc \
    tiny_obj_loader.cc \
    triangle_mesh.cc \
    mesh_io.cc \
    main_window.cc \
    glwidget.cc \
    camera.cc

HEADERS  += \
    tiny_obj_loader.h \
    triangle_mesh.h \
    mesh_io.h \
    main_window.h \
    glwidget.h \
    camera.h

FORMS    += \
    main_window.ui

OTHER_FILES +=

DISTFILES += \
    ./shaders/diffuse_irradiance.frag \
    ./shaders/diffuse_irradiance.vert \
    ./shaders/phong.frag \
    ./shaders/phong.vert \
    ./shaders/reflection.frag \
    ./shaders/reflection.vert \
    ./shaders/brdf.frag \
    ./shaders/brdf.vert \
    ./shaders/sky.frag \
    ./shaders/sky.vert \
    ./shaders/texture_mapping_color.frag \
    ./shaders/texture_mapping_color.vert \
    ./shaders/texture_mapping_metalness.frag \
    ./shaders/texture_mapping_metalness.vert \
    ./shaders/texture_mapping_roughness.frag \
    ./shaders/texture_mapping_roughness.vert \
    ./shaders/specular_irradiance.frag \
    ./shaders/specular_irradiance.vert \
    shaders/step_four.frag \
    shaders/step_four.vert \
    shaders/step_one.frag \
    shaders/step_one.vert \
    shaders/step_three.frag \
    shaders/step_three.vert \
    shaders/step_two.frag \
    shaders/step_two.vert
