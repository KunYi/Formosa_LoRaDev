function GetHydraSensorData()
{
    return [ 0xFE, 0xDA, 0xDD, 0xAA];
}

function ResetHydraSensor() {
    return [ 0xFD, 0xAA, 0x55, 0xAA];
}

function SyncHydraSensorTime() {
    var bytes = [];
    bytes[0] = 0xFE;
    bytes[1] = 0xEA;
    var d = Date.now();
    /* time 64 */
    bytes[2] = d >> 56 & 0xFF;
    bytes[3] = d >> 48 & 0xFF;
    bytes[4] = d >> 40 & 0xFF;
    bytes[5] = d >> 32 & 0xFF;
    bytes[6] = d >> 24 & 0xFF;
    bytes[7] = d >> 16 & 0xFF;
    bytes[8] = d >> 8  & 0xFF;
    bytes[9] = d & 0xFF;
    return bytes;
}

function GetHydraSensorPeriod()
{
    return [ 0xFE, 0x01, 0x00 ];
}

function SetHydraSensorPeriod(obj) 
{
    var bytes = [];
    if (typeof obj.Second == "number") {
        d = obj.Second;
        if (d < 15) d = 15;
        bytes[0] = 0xFE;
        bytes[1] = 0x01;
        bytes[2] = 0x01; // set
        bytes[3] = d >> 24 & 0xFF;
        bytes[4] = d >> 16 & 0xFF;
        bytes[5] = d >> 8 & 0xFF;
        bytes[6] = d & 0xFF;
    }
    return bytes;
}

function GetHydraSensorSoilType()
{
    return [ 0xFE, 0x02, 0x00 ];
}

function SetHydraSensorSoilType(obj) 
{
    var bytes = [];
    if (typeof obj.Type == "number") {
        d = obj.Type;
        bytes[0] = 0xFE;
        bytes[1] = 0x02;
        bytes[2] = 0x01; // set
        bytes[5] = d >> 8 & 0xFF;
        bytes[6] = d & 0xFF;
    }
    return bytes;
}

function GetHydraSensorECTempCoef()
{
    return [ 0xFE, 0x03, 0x00 ];
}

function SetHydraSensorECTempCoef(obj) 
{
    var bytes = [];
    if (typeof obj.Value == "number") {
        d = obj.Value;
        if (d <= 100) {
            bytes[0] = 0xFE;
            bytes[1] = 0x03;
            bytes[2] = 0x01; // set
            bytes[3] = 0x00;
            bytes[4] = d & 0xFF;
        }
    }
    return bytes;
}

function GetHydraSensorSalCoef()
{
    return [ 0xFE, 0x04, 0x00 ];
}

function SetHydraSensorSalCoef(obj) 
{
    var bytes = [];
    if (typeof obj.Value == "number") {
        d = obj.Value;
        if (d <= 100) {
            bytes[0] = 0xFE;
            bytes[1] = 0x04;
            bytes[2] = 0x01; // set
            bytes[3] = 0x00;
            bytes[4] = d & 0xFF;
        }
    }
    return bytes;
}

function GetHydraSensorTDSCoef()
{
    return [ 0xFE, 0x05, 0x00 ];
}

function SetHydraSensorTDSCoef(obj) 
{
    var bytes = [];
    if (typeof obj.Value == "number") {
        d = obj.Value;
        if (d <= 100) {
            bytes[0] = 0xFE;
            bytes[1] = 0x05;
            bytes[2] = 0x01; // set
            bytes[3] = 0x00;
            bytes[4] = d & 0xFF;
        }
    }
    return bytes;
}

// Encode encodes the given object into an array of bytes.
//  - fPort contains the LoRaWAN fPort number
//  - obj is an object, e.g. {"temperature": 22.5}
// The function must return an array of bytes, e.g. [225, 230, 255, 0]
function Encode(fPort, obj) {
    var bytes = [];
    // const sensorHydrafPort = 10;
    if (fPort == 10) {
        if (!obj.Obj) {
            if (obj.Control == "Reset") {
                return ResetHydraSensor();
            }
            if (obj.Control == "SyncTime") {
                return SyncHydraSensorTime();
            }
        } else {
            if (obj.Obj == "Data") {
                if (obj.Control == "Get") {
                    return GetHydraSensorData();
                }
            }
            else if (obj.Obj == "Period") {
                if (obj.Control == "Get") {
                    return GetHydraSensorPeriod();
                }
                if (obj.Control == "Set") {
                    return SetHydraSensorPeriod(obj);
                }
            }
            else if (obj.Obj == "SoilType") {
                if (obj.Control == "Get") {
                    return GetHydraSensorSoilType();
                }
                if (obj.Control == "Set") {
                    return SetHydraSensorSoilType(obj);
                }
            }
            else if (obj.Obj == "ECTEMPCoef") {
                if (obj.Control == "Get") {
                    return GetHydraSensorECTempCoef();
                }
                if (obj.Control == "Set") {
                    return SetHydraSensorECTempCoef(obj);
                }
            }
            else if (obj.Obj == "SalCoef") {
                if (obj.Control == "Get") {
                    return GetHydraSensorSalCoef();
                }
                if (obj.Control == "Set") {
                    return SetHydraSensorSalCoef(obj);
                }
            }
            else if (obj.Obj == "TDSCoef") {
                if (obj.Control == "Get") {
                    return GetHydraSensorTDSCoef();
                }
                if (obj.Control == "Set") {
                    return SetHydraSensorTDSCoef(obj);
                }
            }      
        } // end of if (obj.Obj)
    } // end of if (fPort == 10)
    return bytes;
}