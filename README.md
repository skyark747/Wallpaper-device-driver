# Wallpaper-device-driver
This is a device driver to change desktop background upon 3 consecutive left clicks in linux.

-run python file in a separately in a terminal using command:
python3 driver.py 
or use your selected IDE to run it.

-navigate to folder containing driver.c and run the file in terminal using commmands:
 make

-to insert the kernel module: 
 sudo insmode driver.ko

-to remove the kernel module: 
 sudo rmmode driver.ko

then simply click on the screen 3 consecutive times , wallpaper will change after every 3 clicks.
