TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS = \
    ext/UGlobalHotkey/uglobalhotkey.pro \
    src/app.pro

app.depends = uglobalhotkey
