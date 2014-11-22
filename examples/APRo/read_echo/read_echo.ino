
#include <SPI.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>



/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 Modified by Alain Hernandez and Alfredo Muniz
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

//
// Hardware configuration
//

// Set CSN/SS to pin D4 = 6, Set CE to Pin D3 = 5

RF24 radio(5, 6); // CE, CSN
//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xe7e7e7e7e7, 0xc2c2c2c2c2 };
//const uint8_t pipes[2][6] = {"1Node", "2Node"};
int led = 15;

//
// Payload
//

const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 2;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size + 1]; // +1 to allow room for a terminating NULL char

void setup(void)
{
  // Print preamble
  //

  Serial.begin(9600);
  delay(1);
  printf_begin();
  printf("\n\rRF24/examples/pingpair_dyn/\n\r");
  pinMode(led, OUTPUT);
  //
  // Setup and configure rf radio
  //
  digitalWrite(led, HIGH);

  radio.begin();

  // enable dynamic payloads
  //radio.enableDynamicPayloads();
  radio.setPayloadSize(8);
  //SPI.setClockDivider(SPI_CLOCK_DIV2);
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setAutoAck(1);
  radio.setChannel(10);
  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  //
  // Start listening
  //

  radio.startListening();
  radio.stopListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
  
  radio.startListening();


  digitalWrite(led, LOW);
}

void loop(void)
{
  uint8_t read_pipe = 1;
  // if there is data ready
  if ( radio.available(pipes[1]) )
  {
    printf("Got something\n");
    // Dump the payloads until we've gotten everything
    uint8_t len;
    while (radio.available(pipes[1]))
    {
      // Fetch the payload, and see if this was the last one.
      radio.read( receive_payload, 8);

      // Put a zero at the end for easy printing
      receive_payload[9] = 0;

      // Spew it
        printf("Got payload size=%i value=%s\n\r", 8, receive_payload);
        digitalWrite(led, HIGH);
      
    }
    digitalWrite(led, LOW);

    // First, stop listening so we can talk
    //radio.stopListening();

    // Send the final one back.
    //radio.write( receive_payload, len );
    //printf("Sent response.\n\r");

    // Now, resume listening so we catch the next packets.
    //radio.startListening();
  }

}
// vim:cin:ai:sts=2 sw=2 ft=cpp
