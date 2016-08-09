# Network Guard
Network Guard is an application for monitoring user-specified network connection.

## Features
* Close apps when the connection is down.
* Currently support vpn/pppoe.

## Usage
* network-guard.exe -c [Entry Name] -a [App Name] -p [App Parameters] -t [Detection Intervals]
* Example: network-guard.exe -c *"vpn"* -a *"C:\Program Files (x86)\Mozilla Firefox\firefox.exe"* -t *1000*

## Dependencies
* rasapi32.lib/librasapi32.a (VC++/MinGW)

## Compiling and Building
1. Create a project named "network-guard"
2. Copy all the files to the project folder
3. In linker under project settings, add the dependencies name.
4. Add flags if you want to compile in x86 mode.
5. Compile and Run!