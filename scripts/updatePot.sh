#!/bin/bash

function do_gettext()
{
    xgettext --package-name=budgie-control-center --package-version=1.1.0 $* --default-domain=budgie-control-center --join-existing --from-code=UTF-8 --no-wrap
}

function do_intltool()
{
    intltool-extract --type=$1 $2
}

rm budgie-control-center.po -f
touch budgie-control-center.po

#for file in `find src -not -path '*/gvc/*' -name "*.c" -or -name "*.vala"`; do
for file in `find . -name "*.c"`; do
    if [[ `grep -F "_(\"" $file` ]]; then
        do_gettext $file --keyword=_ --keyword=N_ --keyword=C_:1c,2 --keyword=NC_:1c,2 --keyword=gettext --keyword=ngettext:1,2 --keyword=g_dngettext:2,3 --add-comments
    fi
done

for file in `find . -name "*.ui"`; do
    if [[ `grep -F "translatable=\"yes\"" $file` ]]; then
        do_intltool gettext/glade $file
        do_gettext ${file}.h --add-comments --keyword=N_:1 --keyword=C_:2
        rm $file.h
    fi
done

for file in `find . -name "*.in"`; do
    if [[ `grep -E "^_*" $file` ]]; then
        do_intltool gettext/keys $file
        do_gettext ${file}.h --add-comments --keyword=N_:1
        rm $file.h
    fi
done

for file in `find . -name "*.desktop*.in"`; do
    do_gettext ${file} --add-comments
done

mv budgie-control-center.po po/budgie-control-center.pot
