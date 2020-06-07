
#include "LoRaWan_APP.h"
#include "SoftwareSerial.h"
#include "Arduino.h"
#include "ModbusMaster.h"
#include "TimeLib.h"

/* OTAA para*/
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0xff, 0xfe, 0x01, 0x00, 0x00 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0xa0, 0x5b, 0x6d, 0x3d, 0x1c, 0x24, 0x46, 0x14, 0x9e, 0x6e, 0x89, 0x49, 0xc9, 0x4f, 0x23, 0x1a };

/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask, default channels 8-15*/ 
uint16_t userChannelsMask[6]={ 0xFF00,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;

struct SENSOR_REPORT_T {
  uint16_t soilType: 1;
  uint16_t ECTempCoef: 1;
  uint16_t saltCoef: 1;
  uint16_t TDSCoef: 1;
  uint16_t period: 1;
  uint16_t dateTime: 1;
  uint16_t twice: 1;
};

typedef union {
  struct SENSOR_REPORT_T b;
  uint16_t               V;
} SENSOR_REPORT;
static SENSOR_REPORT gReportFlags = { .V = 0 };
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;
typedef enum  {
  CMD_RESET = 0xFDAA,  
  CMD_PERIOD = 0xFE01,
  CMD_SOILTYPE = 0xFE02,
  CMD_ECTEMPCOEF = 0xFE03,
  CMD_SALCOEF = 0xFE04,
  CMD_TDSCOEF = 0xFE05,
  CMD_GETDATA = 0xFEDA,
  CMD_SYNCTIME = 0xFEEA
} HYDRA_CMDS;

typedef enum {
  TYPE_GET = 0,
  TYPE_SET = 1
} HYDRA_CMD_TYPE;

typedef enum {
  REG_SOILTYPE = 0x0020,
  REG_ECTEMPCOEF = 0x0022,
  REG_SALCOEF = 0x0023,
  REG_TDSCOEF = 0x0024
} HYDRA_REGS;

ModbusMaster node;
#if 1
SoftwareSerial swser( GPIO5/* rx */, GPIO6/* tx */);
#else
//HardwareSerial hwser(UART_NUM_1);
#endif

static void prepareSoilTypeFrame(void) {
  uint8_t i = 0;
  uint8_t resultMain;
  resultMain = node.readHoldingRegisters(REG_SOILTYPE, 1);
  if (resultMain == node.ku8MBSuccess) {
    uint16_t v = node.getResponseBuffer(0x00);
    appPort = 3;
    appData[i++] = (CMD_SOILTYPE >> 8) & 0xFF;
    appData[i++] = CMD_SOILTYPE & 0xFF;
    appData[i++] = (uint8_t)(v >> 8) & 0xFF;
    appData[i++] = (uint8_t)(v >> 0) & 0xFF;
    appDataSize = i;
  }
  gReportFlags.b.soilType = 0;
}

static void prepareECTempCoefFrame(void) {
  uint8_t i = 0;
  uint8_t resultMain;
  resultMain = node.readHoldingRegisters(REG_ECTEMPCOEF, 1);
  if (resultMain == node.ku8MBSuccess) {
    uint16_t v = node.getResponseBuffer(0x00);
    appPort = 3;
    appData[i++] = (CMD_ECTEMPCOEF >> 8) & 0xFF;
    appData[i++] = CMD_ECTEMPCOEF & 0xFF;
    appData[i++] = (uint8_t)(v >> 8) & 0xFF;
    appData[i++] = (uint8_t)(v >> 0) & 0xFF;
    appDataSize = i;
  }
  gReportFlags.b.ECTempCoef = 0;
}

static void prepareSaltCoefFrame(void) {
  uint8_t i = 0;
  uint8_t resultMain;
  resultMain = node.readHoldingRegisters(REG_SALCOEF, 1);
  if (resultMain == node.ku8MBSuccess) {
    uint16_t v = node.getResponseBuffer(0x00);
    appPort = 3;
    appData[i++] = (CMD_SALCOEF >> 8) & 0xFF;
    appData[i++] = CMD_SALCOEF & 0xFF;
    appData[i++] = (uint8_t)(v >> 8) & 0xFF;
    appData[i++] = (uint8_t)(v >> 0) & 0xFF;
    appDataSize = i;
  }
  gReportFlags.b.saltCoef = 0;
}

static void prepareTDSCoefFrame(void) {
  uint8_t i = 0;
  uint8_t resultMain;
  resultMain = node.readHoldingRegisters(REG_TDSCOEF, 1);
  if (resultMain == node.ku8MBSuccess) {
    uint16_t v = node.getResponseBuffer(0x00);
    appPort = 3;
    appData[i++] = (CMD_TDSCOEF >> 8) & 0xFF;
    appData[i++] = CMD_TDSCOEF & 0xFF;
    appData[i++] = (uint8_t)(v >> 8) & 0xFF;
    appData[i++] = (uint8_t)(v >> 0) & 0xFF;
    appDataSize = i;
  }
  gReportFlags.b.TDSCoef = 0;
}

static void preparePeroidFrame(void)
{
  uint32_t t = (appTxDutyCycle / 1000);
  uint8_t i = 0;
  appPort = 3;
  appData[i++] = (uint8_t)(CMD_PERIOD >> 8) & 0xFF;
  appData[i++] = (uint8_t)(CMD_PERIOD >> 0) & 0xFF;
  appData[i++] = (uint8_t)(t >> 24) & 0xFF;
  appData[i++] = (uint8_t)(t >> 16) & 0xFF;
  appData[i++] = (uint8_t)(t >>  8) & 0xFF;
  appData[i++] = (uint8_t)(t >>  0) & 0xFF;
  appDataSize = i;
  #if 0
  if (!gReportFlags.b.twice) {
    gReportFlags.b.twice = 1;
  } else {
    gReportFlags.b.period = 0;
    gReportFlags.b.twice = 0;
  }
  #endif
  gReportFlags.b.period = 0;
}

static void prepareDataFrame(void)
{
  uint8_t i;
  uint8_t resultMain;
  uint16_t temperature = 0, vwc = 0, EC = 0, sal = 0;

  resultMain = node.readHoldingRegisters(0x0000, 4);
  if (resultMain == node.ku8MBSuccess) {
    Serial.println("---------------");
    temperature = node.getResponseBuffer(0x00);
    vwc = node.getResponseBuffer(0x01);
    EC = node.getResponseBuffer(0x02);
    sal = node.getResponseBuffer(0x03);
    Serial.print("temp:" + String(temperature));
    Serial.print(", vwc:" + String(vwc));
    Serial.print(", EC:" + String(EC));
    Serial.println(", Sal:" + String(sal));
  } else {
    /* fake data for test */
    temperature = 2534; // 25.34 C
    //temperature = 0xFF05; // -2.51 C
    vwc = 7803;         // 78.03%
    EC = 18340;
    sal = 14534;
    Serial.println("Send simulation data");
  }
  i = 0;
  appPort = 2;
  appData[i++] = (uint8_t)(temperature >> 8);
  appData[i++] = (uint8_t)(temperature & 0xFF);
  appData[i++] = (uint8_t)(vwc >> 8);
  appData[i++] = (uint8_t)(vwc & 0xFF);
  appData[i++] = (uint8_t)(EC >> 8);
  appData[i++] = (uint8_t)(EC & 0xFF);
  appData[i++] = (uint8_t)(sal >> 8);
  appData[i++] = (uint8_t)(sal & 0xFF);
  appDataSize = i;
}

/* Prepares the payload of the frame */
static void prepareTxFrame(void)
{
	/*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
	*appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
	*if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
	*if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
	*for example, if use REGION_CN470, 
	*the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
	*/

  if (gReportFlags.V != 0) {
    if (gReportFlags.b.soilType) {
      prepareSoilTypeFrame();
      return;
    }
    if (gReportFlags.b.ECTempCoef) {
      prepareECTempCoefFrame();
      return;
    }
    if (gReportFlags.b.saltCoef) {
      prepareSaltCoefFrame();
      return;
    }
    if (gReportFlags.b.TDSCoef) {
      prepareTDSCoefFrame();
      return;
    }
    if (gReportFlags.b.period) {
      preparePeroidFrame();
      return;
    }
  }
  prepareDataFrame();
}

static void reportPeriod() {
  gReportFlags.b.period = 1;
}

static void setPeroid(uint8_t size, uint8_t *d) {
  uint32_t t = 15; // 15second
  if (size >= 4) {
    t = d[0] << 24 | d[1] << 16 | d[2] << 8 | d[3];
    if (t < 15) t = 15;
    if (t > 86400) t = 86400;
  }
  appTxDutyCycle = t * 1000;
  gReportFlags.b.period = 1;
}

static void reportSoilType() {
  Serial.println("set report SoilType");
  gReportFlags.b.soilType = 1;
}

static void setSoilType(uint8_t size, uint8_t *d) {
  if (size >= 2) {
    uint16_t v = d[0] << 0 | d[1];
    if (v <= 3) {
      node.writeSingleRegister(REG_SOILTYPE, v);
      gReportFlags.b.soilType = 1;
    }
  }
}

static void reportECTempCoef() {
  gReportFlags.b.ECTempCoef = 1;
}

static void setECTempCoef(uint8_t size, uint8_t *d) {
  if (size >= 2) {
    uint16_t v = d[0] << 0 | d[1];
    if (v <= 100) {
      node.writeSingleRegister(REG_ECTEMPCOEF, v);
      gReportFlags.b.ECTempCoef = 1;
    }
  }
}

static void reportSaltCoef() {
  gReportFlags.b.saltCoef = 1;
}

static void setSaltCoef(uint8_t size, uint8_t *d) {
  if (size >= 2) {
    uint16_t v = d[0] << 0 | d[1];
    if (v <= 100) {
      node.writeSingleRegister(REG_SALCOEF, v);
      gReportFlags.b.saltCoef = 1;
    }
  }
}

static void reportTDSCoef() {
  gReportFlags.b.TDSCoef = 1;
}

static void setTDSCoef(uint8_t size, uint8_t *d) {
  if (size >= 2) {
    uint16_t v = d[0] << 0 | d[1];
    if (v <= 100) {
      node.writeSingleRegister(REG_TDSCOEF, v);
      gReportFlags.b.TDSCoef = 1;
    }
  }
}

static void setNetTime(uint8_t size, uint8_t *d) {
    if (size >= 8) {
      uint64_t time;
      time = d[0] << 56 | d[1] << 48 | d[2] << 40 | d[3] << 32 |
            d[4] << 24 | d[5] << 16 | d[6] << 8 | d[7];
      setTime((time_t)time);
    }
}

static void resetDevice(void) {
    typedef void (*funcptr)();
    Serial.println("Reset Device now!\r\n");
    delay(200);
    (* (funcptr) (void *) 0)(); // software reset
    (* (funcptr) (void *) 0)();
}

static void procRxData(const uint8_t port, uint8_t size, uint8_t *data) 
{
  if (port != 10 || size < 3 || data == NULL) return;  
  uint16_t cmd = data[0] << 8 | data[1];

  switch(cmd) {
  case CMD_PERIOD:
    if (data[3] == TYPE_GET) {
      reportPeriod();
    } else if (data[3] == TYPE_SET) {
      setPeroid(size - 3, &data[4]);
    }
    break;
  case CMD_SOILTYPE:
    if (data[3] == TYPE_GET) {
      reportSoilType();
    } else if (data[3] == TYPE_SET) {
      setSoilType(size - 3, &data[4]);
    }
    break;
  case CMD_ECTEMPCOEF:
    if (data[3] == TYPE_GET) {
      reportECTempCoef();
    } else if (data[3] == TYPE_SET) {
      setECTempCoef(size - 3, &data[4]);
    }
    break;
  case CMD_SALCOEF:
    if (data[3] == TYPE_GET) {
      reportSaltCoef();
    } else if (data[3] == TYPE_SET) {
      setSaltCoef(size - 3, &data[4]);
    }
    break;
  case CMD_TDSCOEF:
    if (data[3] == TYPE_GET) {
      reportTDSCoef();
    } else if (data[3] == TYPE_SET) {
      setTDSCoef(size - 3, &data[4]);
    }
    break;
  case CMD_SYNCTIME:
    setNetTime(size - 2, &data[4]);
    break;
  case CMD_RESET:
    if (data[3] == 0x55 && data[4] == 0xAA)
      resetDevice();
    break;
  }
}

//downlink data handle function example
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  Serial.printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
  Serial.print("+REV DATA:");
  for(uint8_t i=0;i<mcpsIndication->BufferSize;i++)
  {
    Serial.printf("%02X",mcpsIndication->Buffer[i]);
  }
  Serial.println();
  procRxData(mcpsIndication->Port, mcpsIndication->BufferSize, mcpsIndication->Buffer);
}

void setup() {
  
  uint8_t resultMain;
	boardInitMcu();
	Serial.begin(115200);

  swser.begin(9600);
  node.begin(1, swser); /* modbus slave id 1 */  

  //Serial1.begin(9600);
  //node.begin(1, Serial1); /* modbus slave id 1 */

  //node.preTransmission(preTransmission);
  //node.postTransmission(postTransmission);

#if(AT_SUPPORT)
	enableAt();
#endif
	deviceState = DEVICE_STATE_INIT;
	LoRaWAN.ifskipjoin();
}

void loop()
{
	switch( deviceState )
	{
		case DEVICE_STATE_INIT:
		{
#if(AT_SUPPORT)
			getDevParam();
#endif
			printDevParam();
			LoRaWAN.init(loraWanClass,loraWanRegion);
			deviceState = DEVICE_STATE_JOIN;
			break;
		}
		case DEVICE_STATE_JOIN:
		{
			LoRaWAN.join();
			break;
		}
		case DEVICE_STATE_SEND:
		{
			prepareTxFrame();
			LoRaWAN.send();
			deviceState = DEVICE_STATE_CYCLE;
			break;
		}
		case DEVICE_STATE_CYCLE:
		{
			// Schedule next packet transmission
			txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
			LoRaWAN.cycle(txDutyCycleTime);
			deviceState = DEVICE_STATE_SLEEP;
			break;
		}
		case DEVICE_STATE_SLEEP:
		{
			LoRaWAN.sleep();
			break;
		}
		default:
		{
			deviceState = DEVICE_STATE_INIT;
			break;
		}
	}
}
