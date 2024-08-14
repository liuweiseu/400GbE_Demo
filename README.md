# 400G Demo
The code is used for 400G Ethernet tests.

## How to use the code 
1. OS requirement
    Currently, the code is only tested on Ubuntu22.04.
    ```
    ~$ sudo lsb_release -a
    No LSB modules are available.
    Distributor ID:	Ubuntu
    Description:	Ubuntu 22.04.4 LTS
    Release:	22.04
    Codename:	jammy
    ```
2. install cmake
    ```
    sudo apt install cmake
    ```
3. clone the repo
    ```
    git clone https://github.com/liuweiseu/400GbE_Demo.git
    ```
3. go to RoCEv2 directory, and create a build directory
    ```
    cd RoCEv2
    mkdir build
    ```
4. compile the code
    ```
    cd build
    cmake ../
    make
    sudo make install
    ```
5. run `RecvDemo -h` or `SendDemo -h` to get the information about how to do the test.
    ```
    ~$ RecvDemo -h
    Usage:
    RecvDemo     Receiver Demo at 400Gbps

    Options:
        -h, print out the helper information.
        -d, NIC dev number. '0' means mlx5_0.
        --smac, source MAC address.
        --dmac, destination MAC address.
        --sip, source IP address.
        --dip, destination IP address.
        --sport, source port number.
        --dport, destination port number.
        --gpu, allocate memory on GPU. the memory is allocated on the host by default.
        --disable-recv, disable recv.
        --help, -h,  print out the helper information.

    ~$ SendDemo -h
    Usage:
    SendDemo     Sender Demo at 400Gbps

    Options:
        -h, print out the helper information.
        -d, NIC dev number. '0' means mlx5_0.
        --smac, source MAC address.
        --dmac, destination MAC address.
        --sip, source IP address.
        --dip, destination IP address.
        --sport, source port number.
        --dport, destination port number.
        --streams, number of streams.
        -N, the packet number to be sent out is Nx512.
        --help,-h print out the helper information.
    ```
## Scripts
There are several useful scripts in `scripts` directory. 