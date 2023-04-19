# Modbus MQTT Gateway Using STM32 and NodeMCU.
## In this video we are creating an IoT application using two important IoT protocols-Modbus and MQTT.
### Modbus Communication is implemented between STM32 Bluepill and NodeMCU. The NodeMCU publishes the Modbus data to a Mqtt server and also subscribes to a control message from the dashboard.The control message is forwarded to STM32 for actuation of relay.

 ![alt text](https://github.com/sarathtv/Modbus-Mqtt-Gateway/blob/master/FrontImage.jpg)
- [Library][link]
- [Youtube video][vid]
- [Connection Diagram][dia]

* Connection Details NodeMCU

|     NodeMCU ESP8266 |     RS485 -TTL adapter |
|:-------------------:|:----------------------:|
|          D2         |           DI           |
|          D3         |           RO           |
|          D0         |           RE           |
|          D0         |           DE           |
|         Vcc         |          3.3V          |
|         GND         |           GND          |


* Connection Details STM32 Bluepill

|    STM32 BLUEPILL6 |    RS485 -TTL adapter | DHT11 | Relay |
|:------------------:|:---------------------:|:-----:|-------|
|         PA2        |           DI          |   x   |   x   |
|         PA3        |           RO          |   x   |   x   |
|         PA8        |           RE          |   x   |   x   |
|         PA8        |           DE          |   x   |   x   |
|        3.3V        |          Vcc          |  Vcc  |  Vcc  |
|         GND        |          GND          |  GND  |  GND  |
|        PB12        |           x           |  Out  |   x   |
|         PB1        |           x           |   x   |  IN1  |

[link]:https://github.com/emelianov/modbus-esp8266
[code]:https://github.com/sarathtv/Modbus_PC_SM_RTUoverTCP/blob/master/Client_PC_Modbus_SM_RTUoverTCP.py
[vid]:https://youtu.be/5n9JKzepvGc
[dia]:https://github.com/sarathtv/Modbus-Mqtt-Gateway/blob/master/schematic.jpg