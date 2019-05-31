#!/bin/bash

echo "Before we start, Have you backed up your pi?"
read -p "Y/N: " yn;
case $yn in
	[Yy]* ) echo "Are you connected to the internet?"; read -p "Y/N: " yn;
		case $yn in
		[Yy]* ) echo "Have you run this script before?"; read -p "Y/N: " yn;
			case $yn in 
			[Yy]* ) ./Backup-cmd-config.sh;
			./UPDATE-PI.sh;
			./UART-ENABLE.sh;
			./I2C-INSTALL.sh;
			./REBOOT.sh;;
			[Nn]* )	chmod +x Backup-cmd-config.sh UPDATE-PI.sh UART-ENABLE.sh I2C-INSTALL.sh REBOOT.sh; 
			./Backup-cmd-config.sh;
			./UPDATE-PI.sh;
			./UART-ENABLE.sh;
			./I2C-INSTALL.sh;
			./REBOOT.sh;;
			esac ;;
		[Nn]* ) echo "Please connect to the internet before running this script";;
		esac;;
		
	[Nn]* )echo "Please back up your data before running this script"; exit;;
esac
