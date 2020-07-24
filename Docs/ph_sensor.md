
PH SENSOR
===
### INTERFACE: 
#### MODBUS/RS485 RTU PROTOCOL
---
 * Slave ID: 0x01
 * Baud: 9600bps
 * Parity: None
 * Data: 8 Bits
 * Stop: 1


### Data JSON Object, for example
---
**MQTT Topic: 'application/2/device/007032fffe020001/rx'**
```
####

### Data JSON Object, for example
---
**MQTT Topic: 'application/2/device/007032fffe020000/rx'**
```
####

### Data JSON Object, for example
---
**MQTT Topic: 'application/2/device/70bed5fffe020001/rx'**
```

{   
    "applicationID":"2",
    "applicationName":"Formosa-Farm",
    "deviceName":"RM1-LT01",
    "devEUI":"70bed5fffe030001",
    "txInfo":{
        "frequency":904700000,
        "dr":3
        },
    "adr":true,
    "fCnt":8,
    "fPort":5,
    "data":"CeYhVAAAFTg=",
    "object":{
        "PH":5.56,
        "Timestampe":1592554253896,
        "report":"Data"
    }
}


