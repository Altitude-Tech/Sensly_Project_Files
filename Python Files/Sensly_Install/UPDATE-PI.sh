#!/bin/bash
echo "Do you want to update and upgrade your pi? (recommeded)"; read -p "Y/N: " yn; 
case $yn in
	[Yy]* ) sudo apt-get update;
		sudo apt-get upgrade;;
	[Nn]* ) ;;
esac
