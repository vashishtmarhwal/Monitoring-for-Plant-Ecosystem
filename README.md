# Monitoring-for-Plant-Ecosystem
Used sensors such as Temperature, Humidity, Soil Moisture, and Ambient Light sensor, to monitor the plant environment and upload data on Thingspeak.
Our Project focuses on automating the task of looking after your plant with the help of IoT. 

Using a set of 3 sensors (Temperature and Humidity Sensor, Soil Moisture Sensor and Light Sensor), we are going to tell about the various environmental conditions surrounding the plant. 

We will determine the soil moisture and by applying Darcy’s Law for flow of water we’ll check when the plant requires watering. 

The Water pump will automatically turn on based on certain set of conditions when fulfilled by cloud server. 

We have incorporated Thingspeak Cloud Service to send data to and analyse using MATLAB. 

Other environmental factors such as Light sensitivity, Temperature and Humidity will also be set and respective notifications about the plant’s condition will be sent to the owner on his mobile device through cloud.

### Data gathered on ThingSpeak
![ThingSpeak Output](https://i.imgur.com/Ux25mAX.png)

### Output on Smartwatch using IFTTT

![Watch Output](https://i.imgur.com/iC17k84.png)
![Watch Output](https://i.imgur.com/qM1tpbQ.png)

*/*Tested under Artificial Conditions*

### Frameworks Used
1. Arduino UNO
2. ThingSpeak
3. IFTTT Webhooks
