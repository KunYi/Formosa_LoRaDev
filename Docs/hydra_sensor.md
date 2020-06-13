
HYDRA SENSOR
===
### INTERFACE: 
#### MODBUS/RS485 RTU PROTOCOL
---
 * Slave ID: 0x01
 * Baud: 9600bps
 * Parity: None
 * Data: 8 Bits
 * Stop: 1

#### HoldingRegisters/Function 3(Read)/6(Write)
---
* 0x0000 : Temperature
* 0x0001 : Volume Water Content
* 0x0002 : Electrical Conductivity
* 0x0003 : Salinty
* 0x0020 : Soil type, (0 - 3)
* 0x0022 : Temperature Coefficient of Electrical Conductivity, (0 - 100), default 20 (2%)
* 0x0023 : Coefficent of Salinty (0 - 100), default 55(0.55)
* 0x0024 : Coffficent of TDS(Total Dissolved Solids) (0 - 100), default 50(0.5)

####

### Data JSON Object, for example
---
**MQTT Topic: 'application/2/device/70bed5fffe010001/rx'**
```
{ 
    "applicationID":"2",
    "applicationName":"Formosa-Farm",
    "deviceName":"RM1-SS1",
    "devEUI":"70bed5fffe010001",
    "txInfo": { 
                "frequency": 905100000, 
                "dr":3 
            },
    "adr":true,
    "fCnt":8,
    "fPort":2,
    "data":"CeYee0ekOMY=",
    "object": {
                "ElectricalConductivity": 18340,
                "Salinty":14534,
                "Temperature":25.34,
                "Timestampe":1591428220991,
                "VolumeWaterContent":78.03
            }
}
```
### Get/Set Attribute in JSON Object
---
**MQTT Topic: 'application/2/device/70bed5fffe010001/tx'**

70bed5fffe010001 : Device EUI(Extended Unique Indentifier), need change it to match your node

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
---


### Soil Type
---
#### get soil type
```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Get", 
                "Obj": "SoilType" 
            }
}
```


#### set soil type, 0 ~ 3
* 0: 礦物土
* 1: 沙土
* 2: 黏土
* 3: 有機質土


```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Set",
                "Obj": "SoilType",                
                "Type": 0 
            }
}
```
---


### Temperature Coefficient of Electrical Conductivity
---
#### get temperature coefficent of EC
```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Get", 
                "Obj": "ECTEMPCoef" 
            }
}
```


#### set temperature coefficent of EC, value 0 ~ 100

```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Set",
                "Obj": "ECTEMPCoef",                
                "Value": 0 
            }
}
```
---


### Coefficent of Salinty
---
#### get coefficent of Salinty
```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Get", 
                "Obj": "SalCoef" 
            }
}
```


#### set coefficent of Salinty, value 0 ~ 100

```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Set",
                "Obj": "SalCoef",                
                "Value": 0 
            }
}
```
---


### Coefficent of TDS
---
#### get coefficent of TDS
```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Get", 
                "Obj": "TDSCoef" 
            }
}
```


#### set coefficent of TDS, value 0 ~ 100

```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Set",
                "Obj": "TDSCoef",                
                "Value": 0 
            }
}
```
---


### Directly update data to gateway (only LoRaWAN Class type C node support)
---
#### get sensor data
```
{
    "confirmed":true,
    "fPort":10,
    "object": { 
                "Control": "Get", 
                "Obj": "Data" 
            }
}