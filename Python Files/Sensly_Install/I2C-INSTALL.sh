#!/bin/bash
echo "Have you enabled I2C on your Pi?"; read -p "Y/N: " yn;
case $yn in
	[Yy]* ) ;;
	[Nn]* ) sudo apt-get install i2c-tools python-smbus python3-smbus python-dev python3-dev -f;
		echo "dtparam=i2c_arm=on" >> /boot/config.txt;
		echo "I2C Enabled";;
esac
