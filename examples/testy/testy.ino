/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example using Dynamic Payloads 
 *
 * This is an example of how to use payloads of a varying (dynamic) size. 
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 3 & 4

RF24 radio(4,3);
//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xE7E7E7E7E7, 0xC2C2C2C2C2 };
int led = 15;


//
// Payload
//

const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 2;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

void setup(void)
{
  // Print preamble
  //

  Serial.begin(115200);
  printf_begin();
  printf("\n\rRF24/examples/pingpair_dyn/\n\r");
  //
  // Setup and configure rf radio
  //
  digitalWrite(led, HIGH);
  radio.begin();

  // enable dynamic payloads
  radio.enableDynamicPayloads();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setDataRate(RF24_250KBPS);
  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
  digitalWrite(led, LOW);
}

void loop(void)
{

    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      uint8_t len;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
	len = radio.getDynamicPayloadSize();
	done = radio.read( receive_payload, len );

	// Put a zero at the end for easy printing
	receive_payload[len] = 0;

	// Spew it
	printf("Got payload size=%i value=%s\n\r",len,receive_payload);
        digitalWrite(led, HIGH);
      }
      digitwalWrite(led, LOW);

      // First, stop listening so we can talk
      radio.stopListening();
      
      // Send the final one back.
      radio.write( receive_payload, len );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }

}
// vim:cin:ai:sts=2 sw=2 ft=cpp
