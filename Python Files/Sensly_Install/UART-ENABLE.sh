#!/bin/bash
echo "Have you enabled UART on your Pi?"; read -p "Y/N: " yn;
case $yn in
	[Yy]* ) ;;
	[Nn]* ) echo " " >> /boot/config.txt;
		echo "# Uart Enable" >> /boot/config.txt;
		echo "dtoverlay=pi3-disable-bt" >> /boot/config.txt;
		echo "enable_uart=1" >> /boot/config.txt;
		sed -i "s/console=tty1//g" /boot/cmdline.txt;
		sed -i "s/console=serial0/console=tty1/g" /boot/cmdline.txt;
		echo "Uart Enabled";;
esac

