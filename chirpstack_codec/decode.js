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

function Decode(fPort, bytes) 
{
  	var myObj = {};
    if (fPort == 2) {
    	myObj.Temperature = getTemp(bytes.slice(0, 2));
    	myObj.VolumeWaterContent = getVWC(bytes.slice(2, 4))
    	myObj.ElectricalConductivity = getUint16(bytes.slice(4, 6))
    	myObj.Salinty = getUint16(bytes.slice(6, 8))
        myObj.Timestampe = + new Date();
    }
	return myObj;
}