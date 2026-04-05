# EECS 300 Demo Setup

## GUI Display

### Install dependencies

```sh
sudo apt update
sudo apt install cmake ninja-build qtbase5-dev libqt5serialport5 libqt5serialport5-dev
```

### Build

Navigate to the repo root directory an run:

```sh
cmake -S gui -B gui/build -G Ninja
ninja -C gui/build
```

### Run the executable

```sh
./gui/build/eecs300-demo
```
