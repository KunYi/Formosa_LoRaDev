
function getTemp(tmp)
{
  var t = (tmp[0] << 8 | tmp[1]);
  if (tmp[0] == 0xFF) {
     t = t - 65536;
  }
  return t.toFixed(2) * 0.01;
}

function getTemp1(tmp)
{
  var t = (tmp[0] << 8 | tmp[1]);
  if (tmp[0] == 0xFF) {
     t = t - 65536;
  }
  return t.toFixed(2) * 0.1;
}

function getVWC(tmp) {
  return (tmp[0] << 8 | tmp[1]).toFixed(2) * 0.01;
}

function getPH(tmp) {
  return (tmp[0] << 8 | tmp[1]).toFixed(2) * 0.001;
}

function getHuminity(tmp) {
  return (tmp[0] << 8 | tmp[1]).toFixed(2) * 0.1;
}

function getUint16(tmp) {
  return (tmp[0] << 8 | tmp[1])
}

function getUint32(tmp) {
  return ((tmp[0] << 24) + (tmp[1] << 16) +
    (tmp[2] << 8) + tmp[3]);
}

function getReportType(b) {
  return getUint16(b);
}

function Decode(fPort, bytes) 
{
  	var myObj = {};
    if (fPort == 2) {
      myObj.report = "Data";
      myObj.Temperature = getTemp(bytes.slice(0, 2));
      myObj.VolumeWaterContent = getVWC(bytes.slice(2, 4));
      myObj.ElectricalConductivity = getUint16(bytes.slice(4, 6));
      myObj.Salinty = getUint16(bytes.slice(6, 8));
      myObj.Timestampe = + new Date();
    } else if (fPort == 3) {
        t = getReportType(bytes.slice(0, 2));
        if (t == 0xFE01) {
          myObj.report = "Peroid";
          myObj.Peroid = getUint32(bytes.slice(2, 6));
          myObj.Timestampe = + new Date();
        } else if (t == 0xFE02) {
          myObj.report = "SoilType";
          myObj.SoilType = getUint16(bytes.slice(2, 4));
          myObj.Timestampe = + new Date();
        } else if (t == 0xFE03) {
          myObj.report = "ECTEMPCoef";
          myObj.ECTEMPCoef = getUint16(bytes.slice(2, 4));
          myObj.Timestampe = + new Date();
        } else if (t == 0xFE04) {
          myObj.report = "SaltCoef";
          myObj.SaltCoef = getUint16(bytes.slice(2, 4));
          myObj.Timestampe = + new Date();
        } else if (t == 0xFE05) {
          myObj.report = "TDSCoef";
          myObj.TDSCoef = getUint16(bytes.slice(2, 4));
          myObj.Timestampe = + new Date();
        }
    } else if (fPort == 4) { // for PH Sensor
      myObj.report = "Data";
      myObj.PH = getPH(bytes.slice(0, 2));
      myObj.Timestampe = + new Date();
    } else if (fPort == 5) { // for Light/Temperture/Huminity/CO2
      myObj.report = "Data";
      myObj.Temperature = getTemp1(bytes.slice(0, 2));
      myObj.Huminity = getHuminity(bytes.slice(2,4));
      myObj.Lux = getUint32(bytes.slice(4, 8));
      if (bytes.length == 10) {
        myObj.CO2 = getUint16(bytes.slice(8,10));
      }
      myObj.Timestampe = + new Date();
    } else if (fPort == 6) { // for Irrigation
      myObj.report = "Data";
      myObj.Irrigation = Object;

      if ((bytes[1] & 0x01) == 0x01)
      {
        myObj.Irrigation.channel1 = true;
      }
      else {
        myObj.Irrigation.channel1 = false;
      }

      if ((bytes[1] & 0x02) == 0x02)
      {
        myObj.Irrigation.channel2 = true;
      }
      else {
        myObj.Irrigation.channel2 = false;
      }

      if ((bytes[1] & 0x04) == 0x04)
      {
        myObj.Irrigation.channel3 = true;
      }
      else {
        myObj.Irrigation.channel3 = false;
      }

      if ((bytes[1] & 0x08) == 0x08)
      {
        myObj.Irrigation.channel4 = true;
      }
      else {
        myObj.Irrigation.channel4 = false;
      }
    }
	return myObj;
}