
Temperature/Huminity/Lux sensor
===
### INTERFACE: 
#### MODBUS/RS485 RTU PROTOCOL
---
 * Slave ID: 0x10
 * Baud: 9600bps
 * Parity: None
 * Data: 8 Bits
 * Stop: 1

#### HoldingRegisters/Function 3(Read)

* 0x0005 : Temperature
* 0x0006 : Huminity
* 0x0009 : Lux (MSB)
* 0x000A : Lux (MLS)

####

### Data JSON Object, for example
---
**MQTT Topic: 'application/2/device/007032fffe010000/rx'**
```
####

### Data JSON Object, for example
---
**MQTT Topic: 'application/2/device/007032fffe010000/rx'**
```
####

### Data JSON Object, for example
---
**MQTT Topic: 'application/2/device/70bed5fffe030001/rx'**
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
        "Huminity":85.32,
        "Lux":5432,
        "Temperature":25.3,
        "Timestampe":1592039730455,
        "report":"Data"}
}

```
### Get/Set Attribute in JSON Object
---
**MQTT Topic: 'application/2/device/70bed5fffe030001/tx'**

70bed5fffe030001 : Device EUI(Extended Unique Indentifier), need change it to match your node

### Period
---
#### get update cycle
```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Get", 
                "Obj": "Period" 
            }
}
```

#### set update cycle, minimum: 15 second, maximum 60sec*60(min)*24=86400
```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Set",
                "Obj": "Period",                
                "Second": 120 
            }
}
```
