#!/bin/bash
echo "Do you want to reboot now?"; read -p "Y/N: " yn;
case $yn in
	[Yy]* ) sudo reboot;;
	[Nn]* ) ;;
esac
