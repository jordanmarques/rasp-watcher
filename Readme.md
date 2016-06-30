#Rasp-watcher

Rasp-watcher is a little Opencv 3.0 program who use Raspcam to detect movements.

##Installation steps:
First of all, you need OpenCV 3.0 on your Raspberry. If you don't have it, follow these steps:


###Dependencies:

```sh
sudo apt-get update

sudo apt-get upgrade

sudo apt-get install guvcview

sudo apt-get install synaptic

sudo apt-get install python-dev

sudo apt-get install python-numpy python-scipy python-matplotlib python-pandas python-nose

sudo apt-get install build-essential cmake pkg-config

sudo apt-get install default-jdk ant

sudo apt-get install libgtkglext1-dev

sudo apt-get install bison

sudo apt-get install qt4-dev-tools libqt4-dev libqt4-core libqt4-gui

sudo apt-get install v4l-utils

sudo apt-get install qtcreator
```
###Download and install OpenCV 3.0

 - Download:

```sh
sudo wget http://liquidtelecom.dl.sourceforge.net/project/opencvlibrary/opencv-unix/3.1.0/opencv-3.1.0.zip
```

 - Unzip and build it (be carrefull)

```sh
sudo unzip opencv-3.1.0.zip

cd opencv-3.1.0

sudo mkdir build

cd build

sudo cmake -D CMAKE_BUILD_TYPE=RELEASE -D INSTALL_C_EXAMPLES=ON –D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=ON -D WITH_QT=ON -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_OPENGL=ON -D WITH_V4L=ON –D BUILD_NEW_PYTHON_SUPPORT=ON -D WITH_TBB=ON ..

sudo make

sudo make install
```
- Setup your lib files

Open this file:

```sh
sudo nano /etc/ld.so.conf.d/opencv.conf
```

And write the following in:

```txt
/usr/local/lib
```
Execute it:

```sh
sudo ldconfig
```

Open this file:

```sh
sudo nano /etc/bash.bashrc
```

And write the following in:

```txt
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig

export PKG_CONFIG_PATH
```
To check if your installation is correct, go to:

```sh
cd opencv-3.1.0/samples/cpp
```
Compile this sample (or another):

```sh
g++ -o facedetect facedetect.cpp `pkg-config opencv --cflags --libs`
```
And execute it:

```sh
./facedetect
```

##Others

If your camera is not detected use this command line:```modprobe bcm2835-v4l2 ```.

To compile the project use this : 

```sh
sudo g++ -o motion motion.cpp `pkg-config opencv --cflags --libs` 
```