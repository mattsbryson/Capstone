gnome-terminal -t "launch xserver"  -e "sh xlaunch.sh"
sleep 4s 
gnome-terminal -t "launch vrx" -e "sh vrxlaunch.sh"
sleep 2s
gnome-terminal -t "launch windows for vrx" -e "sh windowlaunch.sh"
