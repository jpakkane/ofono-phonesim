CONFIG += ordered release

TEMPLATE = subdirs

SUBDIRS = lib app

DATA_FILES = moblin.xml

phonesim_data.files = $$DATA_FILES
phonesim_data.path = $$INSTALL_PREFIX/share/phonesim
phonesim_data.hint = data

INSTALLS += phonesim_data

DISTFILES += $$DATA_FILES
