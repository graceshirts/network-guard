# Network Guard
Network Guard is an application for monitoring user-specified network connection.

## Features
* Close apps when the connection is down.
* Currently support vpn/pppoe.

## Usage
* network-guard.exe -c [Entry Name] -a [App Name] -p [App Parameters] -t [Detection Intervals]
* Example: network-guard.exe -c *"vpn"* -a *"C:\Program Files (x86)\Mozilla Firefox\firefox.exe"* -t *1000*
