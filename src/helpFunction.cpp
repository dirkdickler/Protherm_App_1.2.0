
#include <Arduino.h>
#include "constants.h"
#include "Pin_assigment.h"
//#include "HelpFunction.h"
#include "define.h"
#include "constants.h"
#include "main.h"
#include <EEPROM.h>

#include "SD.h"
#include "FS.h"
#include "SPIFFS.h"
#include "esp_log.h"
//#include <EthernetUdp3.h>

//EthernetUDP Udp;
//char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,


void NacitajSuborzSD(void)
{
	if (!SD.begin(SD_CS_pin, SPI))
	{
		log_i("Card Mount begin: ERROR");
		return;
	}
	else
	{
		log_i("Card Mount begin: OK");
	}

	uint8_t cardType = SD.cardType();
	if (cardType == CARD_NONE)
	{
		log_i("No SD card attached");
		return;
	}

	if (cardType == CARD_MMC)
	{
		log_i("SD Card Type: MMC");
	}
	else if (cardType == CARD_SD)
	{
		// Serial.println("SDSC");
		log_i("SD Card Type: SDSC");
	}
	else if (cardType == CARD_SDHC)
	{
		// Serial.println("SDHC");
		log_i("SD Card Type: SDHD");
	}
	else
	{
		// Serial.println("UNKNOWN");
		log_i("SD Card Type: neznama");
	}

	uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	log_i("SD Card Size: %lluMB", cardSize);
	// Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
	log_i("Total space: %lluMB", SD.totalBytes() / (1024 * 1024));
	log_i("Used space: %lluMB", SD.usedBytes() / (1024 * 1024));

	u8 loo = 0;
	File profile;
	do
	{
		profile = SD.open("/aaa.txt", FILE_READ); /* code */
		if (!profile)
		{
			loo++;
			log_i("Open file vratilo profile %i", profile);
		}
		else
		{
			loo = 5;
		}
	} while (loo < 5);

	Serial.printf("Velkost subora je :%lu\r\n", profile.size());
	if (!profile)
	{
		log_i("Opening file to read failed");
		profile.close();
	}
	else
	{
		log_i("Obsah subora po nacitani je:\n");

		while (profile.available())
		{
			while (profile.available())
			{
				Serial.write(profile.read());
			}

			log_i("konec citania subora");
			log_i("================");
		}
		profile.close();
	}
}

int8_t NacitajEEPROM_setting(void)
{

	if (!EEPROM.begin(500))
	{
		log_i("Failed to initialise EEPROM");
		return -1;
	}

	log_i("Succes to initialise EEPROM");

	// EEPROM.writeString(EE_NazovSiete, "zadels\0");
	// EEPROM.writeLong(EE_Vin_gain_1,123);
	// EEPROM.commit();
	// EEPROM.readString(EE_NazovSiete, NazovSiete, 16);

	log_i("EEPROM nacitany rozsah: %u", EEPROM.readByte(EE_rozsah_Prud));

	String slovo;
	slovo = String(EEPROM.readString(EE_NazovSiete)); //, NazovSiete,16);//String(Ethernet.localIP());
	log_i("EEPROM nazov siete je vycitane: %s", slovo);
	EEPROM.readBytes(EE_MAC_LAN, LAN_MAC, 6);
	if (LAN_MAC[0] == 0xff)
	{
		LAN_MAC[0] = 0;
		LAN_MAC[1] = 0x04;
		LAN_MAC[2] = 0x20;
		LAN_MAC[3] = 0x30;
		LAN_MAC[4] = 0x40;
		LAN_MAC[5] = 0x60;
	}

	if (NazovSiete[0] != 0xff) // ak mas novy modul tak EEPROM vrati prazdne hodnoty, preto ich neprepisem z EEPROM, ale necham default
	{
		// String apipch = EEPROM.readString(EE_IPadresa); // "192.168.1.11";
		// local_IP = str2IP(apipch);

		// apipch = EEPROM.readString(EE_SUBNET);
		// subnet = str2IP(apipch);

		// apipch = EEPROM.readString(EE_Brana);
		// gateway = str2IP(apipch);

		// memset(NazovSiete, 0, sizeof(NazovSiete));
		// memset(Heslo, 0, sizeof(Heslo));
		// u8 dd = EEPROM.readBytes(EE_NazovSiete, NazovSiete, 16);
		// u8 ww = EEPROM.readBytes(EE_Heslosiete, Heslo, 20);
		// log_i("Nacitany nazov siete a heslo z EEPROM: %s  a %s\r\n", NazovSiete, Heslo);
		return 0;
	}
	else
	{
		log_i("EEPROM je este prazna, nachavma default hodnoty");
		return 1;
	}
}
String ipToString(IPAddress ip)
{
	String s = "";
	for (int i = 0; i < 4; i++)
		s += i ? "." + String(ip[i]) : String(ip[i]);
	return s;
}

int getIpBlock(int index, String str)
{
	char separator = '.';
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = str.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++)
	{
		if (str.charAt(i) == separator || i == maxIndex)
		{
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? str.substring(strIndex[0], strIndex[1]).toInt() : 0;
}

IPAddress str2IP(String str)
{

	IPAddress ret(getIpBlock(0, str), getIpBlock(1, str), getIpBlock(2, str), getIpBlock(3, str));
	return ret;
}
