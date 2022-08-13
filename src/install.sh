#!/bin/bash

sudo apt-get update
sudo apt-get install build-essential automake autoconf make gawk -y
make lib
sudo make libinstall
make all
