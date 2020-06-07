
function getTemp(tmp)
{
  var t = (tmp[0] << 8 | tmp[1]);
  if (tmp[0] == 0xFF) {
     t = t - 65536;
  }
  return t.toFixed(2) * 0.01;
}

function getVWC(tmp) {
  return (tmp[0] << 8 | tmp[1]).toFixed(2) * 0.01;
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
          myObj.SoilType = getUint16(bytes.slic(2, 4));
          myObj.Timestampe = + new Date();
        } else if (t == 0xFE03) {
          myObj.report = "ECTEMPCoef";
          myObj.ECTEMPCoef = getUint16(bytes.slic(2, 4));
          myObj.Timestampe = + new Date();
        } else if (t == 0xFE04) {
          myObj.report = "SaltCoef";
          myObj.SaltCoef = getUint16(bytes.slic(2, 4));
          myObj.Timestampe = + new Date();
        } else if (t == 0xFE05) {
          myObj.report = "TDSCoef";
          myObj.TDSCoef = getUint16(bytes.slic(2, 4));
          myObj.Timestampe = + new Date();
        }
    }
	return myObj;
}