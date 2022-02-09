# Example Build without Anaconda

Done at UMN by Tom Eichlersmith.

I _do not_ include building ROOT because this UMN server will not run monitoring.

### O.S.

```
$ cat /etc/os-release
NAME="CentOS Stream"
VERSION="8"
ID="centos"
ID_LIKE="rhel fedora"
VERSION_ID="8"
PLATFORM_ID="platform:el8"
PRETTY_NAME="CentOS Stream 8"
ANSI_COLOR="0;31"
CPE_NAME="cpe:/o:centos:centos:8"
HOME_URL="https://centos.org/"
BUG_REPORT_URL="https://bugzilla.redhat.com/"
REDHAT_SUPPORT_PRODUCT="Red Hat Enterprise Linux 8"
REDHAT_SUPPORT_PRODUCT_VERSION="CentOS Stream"
```

### CMake
cmake version 3.20.2

Installed with system package manager.

### GCC
gcc (GCC) 8.5.0 20210514 (Red Hat 8.5.0-3)

Installed with system package manager.

### Rogue
v5.11.1

Jeremy installed this to `/opt/rogue`, not sure how?

### EUDAQ
v2.4.7

Following build instructions [linked here](https://eudaq.github.io/).

1. Download and checkout latest release. 
```
git clone https://github.com/eudaq/eudaq.git
cd eudaq
git checkout v2.4.7
```
2. Configure build to our system
```
cmake -B build -S .
```
3. Build and Install (to source directory)
```
cd build
make install
```

### yaml-cpp
v0.7.0

```
mkdir yaml-cpp
wget -q -O - \
  https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.tar.gz |\
  tar xz --strip-components=1 --directory yaml-cpp
cd yaml-cpp
cmake -B build -S . -DYAML_BUILD_SHARED_LIBS=ON
cd build
make install
```

## ldmx-daq/software
First done on `tom/polarfirelib-dev` branch.

Using environment script from [tomeichlersmith/ldmx-cob](https://github.com/tomeichlersmith/ldmx-cob/blob/main/env.sh)
```
cd ldmx-daq/software
cmake -B build -S . -Deudaq_DIR=/home/eichl008/eudaq
cd build
make install
cd /home/eichl008/eudaq/lib
ln -s /full/path/to/ldmx-daq/software/install/lib/libeudaq_module_dark.so .
```

