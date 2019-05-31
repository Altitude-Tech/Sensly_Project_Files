#!/bin/bash

echo "Have you modified the /boot/cmdline.txt or /boot/config.txt file?"; read -p "Y/N: " yn;
case $yn in
	[Yy]* ) sudo cp /boot/config.txt /boot/config.old && sudo cp /boot/cmdline.txt /boot/cmdline.old;;
	[Nn]* ) ;;
esac 
