<div align="center">
<h1>ndm-server</h1>
</div>

* A C/C++ server application for GNU/Linux that asynchronously
processes TCP and UDP client connections using 
multiplexing mechanism (based on epoll).

System requirements: unix / linux / debian.

TCP and UDP protocols work.

* Built-in commands:
/time - return current server time;
/stats - show connection statistics;
/shutdown - stop the server;

Echo mode for all non-command messages.

* Create the "ndm-server" directory.
```shell
cd ndm-server
```

```shell
git clone https://github.com/levikabd/NDM_server.git
```

Or download from the link
https://github.com/levikabd/NDM_server/archive/refs/heads/main.zip
and unpack the archive into the "ndm-server" directory.

* To compile the program, go to the "ndm-server" directory and run the "make" command.

* **run #1:**
```shell
./ndm-server
```

* To build the package .deb run the following command:
```shell
cp ndm-server ndm-server_*_amd64/usr/sbin/ndm-server
dpkg-deb --build --root-owner-group ndm-server_*_amd64
```

* To install the package, run:
```shell
sudo dpkg -i ndm-server_*_amd64.deb
```

* **To enable and start the service, run #2:**
```shell
sudo systemctl enable ndm-server.service
sudo systemctl start ndm-server.service
```

* **run #3:**
```shell
sudo ndm-server
```

* monitoring:
```shell
sudo netstat -tulnp | grep :8887
sudo netstat -tulnp | grep :8887
```

* run test:
```shell
echo test > /dev/udp/127.0.0.1/8888
echo test > /dev/tcp/127.0.0.1/8887
echo /time > /dev/udp/127.0.0.1/8888
echo /time > /dev/tcp/127.0.0.1/8887
echo /stats > /dev/udp/127.0.0.1/8888
echo /stats > /dev/tcp/127.0.0.1/8887
echo /shutdown > /dev/udp/127.0.0.1/8888
echo /shutdown > /dev/tcp/127.0.0.1/8887
```
