# hydroponicautomation
I have made a complete process to guide you how to properly install it in my youtube channel plese follow and ask question 


This is the software written in c++ and it is basicaly a programing written in arduino ide and for nodemcu wifi development board. This application not only control relay switch and motion sensor activity but also log its data to thingspeak iot analytics platform , this program best feature is that it also can be control by any browser connected to this network.
hardware needed
1. node mcu
2. arduino ide (configure with ide) select the nodemcu board in arduino ide from tool menue.
3. windows operating system 
4.usb cable to connect with nodemcu
5. select port from arduino ide (starting with com like , com1, com5)
stpe to run program in node mcu
1. copy two file  to one folder 
2. open that ino file using arduino ide
3. change the wifi password and ssid as well as thingspeak credential
4. remember that it is a OTA program over the air program 
5. at first you have to load by using usb port but leter you can load this without using usb port threw wifi connection make sure nodemcu and the system should be in the same network
6. before runing this software make sure that your have created 1 channel and 4 field in the thingspeak iot analytics platform to see your data.
                                    process to load this program over the wireless
                                    1. make sure your node mcu and computer in the same network
                                    2. restart arduino ide and make sure port selected is some ip adress
                                    3. compile your code and when is your code ready to install at that time you restatr your nodemcu
                                       your led will blink for 30 second in this 30 second window your code will loaded in node mcu.
                                    4. to control your relay you can youse any browser and simple type the URL to activate and deactivate 
                                       relay.
