import os
import subprocess

WALLPAPER_FILE = "/proc/wallpaper"
path = "/home/skyark/Downloads/girl.jpg"
path_2="/home/skyark/Downloads/change.jpg"
def set_wallpaper(wallpaper_path,wallpaper_path_2,change):
    try:
        if change==False:
            command = f"gsettings set org.gnome.desktop.background picture-uri-dark file://{wallpaper_path}"
            subprocess.run(command, shell=True)
            change=True
        else:
            command = f"gsettings set org.gnome.desktop.background picture-uri-dark file://{wallpaper_path_2}"
            subprocess.run(command, shell=True)
            change=False
        
        print(f"Wallpaper changed to: {wallpaper_path}")
        with open(WALLPAPER_FILE, "w") as file:
            file.write("\0")
            return change
    except Exception as e:
        print(f"Failed to set wallpaper: {e}")

def change_wallpaper():
    change=True
    while True:
        try:
            if os.path.exists(WALLPAPER_FILE):
                with open(WALLPAPER_FILE, "r") as file:
                    cmd = file.read().strip()
                    if cmd == "change":
                        change=set_wallpaper(path,path_2,change)
        except Exception as e:
            print(f"Error monitoring file: {e}")

if __name__ == "__main__":
    change_wallpaper()
