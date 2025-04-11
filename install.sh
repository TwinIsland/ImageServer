#!/bin/bash

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
    VER=$VERSION_ID
else
    echo "fail to detect os-release"
    exit 1
fi

case $OS in
    ubuntu|debian)
        echo "start installing..."
        sudo apt-get update
        sudo apt-get install -y libwebp-dev
        ;;
    centos|rhel)
        echo "start installing..."
        sudo yum install -y libwebp-devel
        ;;
    fedora)
        echo "start installing..."
        sudo dnf install -y libwebp-devel
        ;;
    *)
        echo "doesn't support your os yet, bye!"
        exit 1
        ;;
esac

echo "dependencies installing ok!"    