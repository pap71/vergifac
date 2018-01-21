#!/bin/sh

name="vergifac"

ARCH=`uname -m`  
if [ $ARCH = "x86_64" ]
then
prog="vergifac64"
else
prog="vergifac32"
fi

if [ -f "$name".desktop ]; then
	rm $name.desktop
fi

if [ -f ~/.local/share/applications/"$name".desktop ]; then
	rm ~/.local/share/applications/$name.desktop
fi

touch "$name".desktop

    echo "[Desktop Entry]"  >> "$name".desktop
    echo "Encoding=UTF-8"   >> "$name".desktop
    echo "Type=Application" >> "$name".desktop
    echo "Version=1.0"      >> "$name".desktop
    echo "Name=$name"       >> "$name".desktop
    echo "Path=$PWD"       >> "$name".desktop
    echo "Exec=$PWD/$prog"       >> "$name".desktop
#"Categories"= "AudioVideo" "Audio" "Video" "Development" "Education" "Game"
# "Graphics" "Network" "Office" "Settings" "System" 
    echo "Categories=Office" >> "$name".desktop

chmod +x "$name".desktop

cat "$name".desktop

test -f ${XDG_CONFIG_HOME:-~/.config}/user-dirs.dirs && source ${XDG_CONFIG_HOME:-~/.config}/user-dirs.dirs
desktop=${XDG_DESKTOP_DIR:-$HOME/Desktop}

    cp "$name".desktop "$desktop"
mkdir -p "~/.local/share/applications/"
mv "$name".desktop ~/.local/share/applications/

if [ $? -eq 0 ]
then
    echo "Installation Lanceur OK"
else
    echo "Erreur Installation Lanceur "
fi
#echo "desktop..." ${XDG_DESKTOP_DIR:-$HOME/Desktop}
