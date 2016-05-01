// SerialTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "serialib.h"
#include <stdio.h>
#include <iostream>


#if defined (_WIN32) || defined( _WIN64)
#define         DEVICE_PORT             "COM4"                               // COM1 for windows
#endif

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyS0"                         // ttyS0 for linux
#endif

#define RX_BUFFSIZE2 22

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	serialib LS;
	int charsRead1 = 0;
	int charsRead2 = 0;
	char buffer[1] = { 0 };
	char bufferdata[RX_BUFFSIZE2] = { 0 };
	unsigned short bufferdataOut[15] = { 0 };
	UINT8 hi1[6] = { 204, 204, 3, 232, 1, 0 };
	UINT8 hi2[6] = { 170, 170, 15, 255, 15, 255 };
	int j = 0;
	int x = 0;
	int Ret;


	// Open serial port

	Ret = LS.Open(DEVICE_PORT, 921600);                                        // Open serial link at 115200 bauds
	if (Ret != 1) {                                                           // If an error occured...
		printf("Error while opening port. Permission problem ?\n");        // ... display a message ...
		return Ret;                                                         // ... quit the application
	}
	printf("Serial port opened successfully !\n");

	//LS.FlushReceiver();


	while (!charsRead1)
	{

		LS.Write(hi1,6);

		cout << hi1 << endl;

		charsRead1 = LS.Read(buffer, 1);
		cout << charsRead1 << endl;

	}

	printf("TDS connected \n");

	for (j = 0; j < 256; j++)
	{
		int charWrite2 = LS.Write(hi2,6);
		//	cout << bytesWritten << " bytes were written to the serial port" << endl;

		//	cout << "Reading from the serial port: ";
//		do
//		{
		charsRead2 = LS.Read(bufferdata, RX_BUFFSIZE2);
//		} while (!charsRead2);

//		cout << bufferdata[7] << endl;


		//unsigned short x = 0;
		for (int x = 0; x < 9; x++)
		{

			bufferdataOut[x] = ((unsigned short)(bufferdata[x * 2 + 4] * 256) + (unsigned short)bufferdata[x * 2 + 5]);

			cout << bufferdataOut[x] << ",";

		}
		//cout << (unsigned short)bufferdata[1];
		cout << endl;

	}


	LS.Close();

	return 0;
}

