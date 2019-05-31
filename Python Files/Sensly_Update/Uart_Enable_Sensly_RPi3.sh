#!/bin/bash

echo " Before we start, Have you backed up your pi?"
read -p " " yn;
case $yn in
	[Yy]* )echo "Have you modified the /boot/cmdline.txt or /boot/config.txt file?"; read -p " " yn;
		case $yn in
			[Yy]* )sudo cp /boot/config.txt /boot/config.old && sudo cp /boot/cmdline.txt /boot/cmdline.old; break;;
			[Nn]* )break;;
		esac
		echo " " >> /boot/config.txt;
		echo "# Uart Enable" >> /boot/config.txt;
		echo "dtoverlay=pi3-disable-bt" >> /boot/config.txt
		echo "enable_uart=1" >> /boot/config.txt;
		sed -i "s/console=tty1//g" /boot/cmdline.txt
		sed -i "s/console=serial0/console=tty1/g" /boot/cmdline.txt
		echo "Please Restart your system"
		break;;
	[Nn]* )echo "Please back up your data before running this script"; exit;;

esac