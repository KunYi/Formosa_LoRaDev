
Irrgation control
===
### INTERFACE: 
#### MODBUS/RS485 RTU PROTOCOL
---
 * Slave ID: 0x01
 * Baud: 9600bps
 * Parity: None
 * Data: 8 Bits
 * Stop: 1

####

Set Attribute in JSON Object
---
**MQTT Topic: 'application/2/device/70bed5fffe040001/tx'**

70bed5fffe040001 : Device EUI(Extended Unique Indentifier), need change it to match your node

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
### Set Irrigation control
```
{
 "confirmed":false, 
 "fPort":10, 
 "object": {
	"Obj": "Irrigation", 
	"Control": "Set", 
	"Irrigation": {
	  "channel1": true,
      "channel2": false,
      "channel3": false,
      "channel4": true 
	}
  }
}
```