/*
	Header file for Indruino Serial Protocol
	
	Initial idea & demo: CuongLT
	Modified: HieuNT
	28/02/2017
*/

#ifndef __INDRUINO_PROTOCOL_H_
#define __INDRUINO_PROTOCOL_H_

#include <Arduino.h>
#include "FastCRC.h"

enum {
	INDR_CHECK = 0,
	INDR_STA,
	INDR_RESET,
	INDR_WRITE,
	INDR_READ
} Indr_Proto_Cmd_t;

/**
	Packet format
	0xFFFF(SYNC:2bytes) 
	+ LEN(2bytes) 
	+ PAYLOAD(LEN bytes): including 2 bytes CRC at the end of packet
*/

#define MAX_ARG_SIZE		128
#define MAX_PAYLOAD_SIZE 	(MAX_ARG_SIZE + 2 + 2 + 2) // 2 bytes CRC + 2 bytes SEQ + 2 bytes CMD

typedef struct {
	uint8_t unitId;
	uint16_t unitModel;
	uint32_t unitBaseAddr;
} Indr_Proto_Arg_Res_Check_Unit_Item_t;

/**
	Protocol on payload:
	SEQ (2 bytes) + CMD (2 bytes) + ARGS (remaining)
*/

typedef struct {
	uint16_t seq;
	uint16_t cmd;
	union Indr_Proto_Arg_t {
		// Arg for request (from master to unit)
		union Indr_Proto_Arg_Req_t {
			// CHECK
			struct Indr_Proto_Arg_Req_Check_t {
				// base 1
				uint8_t totalUnit;
				// Base 0, currentUnitId max = (totalUnit - 1)
				uint8_t currentUnitId;
				uint32_t currentBaseAddr;
			} check;
			// STA
			struct Indr_Proto_Arg_Req_Sta_t {
				uint8_t sta;
			} sta;
			// RESET has no arg
			// WRITE
			struct Indr_Proto_Arg_Req_Write_t {
				uint32_t address;
				uint16_t len;
				uint8_t ref;
			} write;
			// READ
			struct Indr_Proto_Arg_Req_Read_t {
				uint32_t address;
				uint16_t len;
			} read;
		} req;
		// Arg for response (from unit to master)
		union Indr_Proto_Arg_Res_t {
			// CHECK
			struct Indr_Proto_Arg_Res_Check_t {
				uint8_t totalUnit;
				// remaining is for Indr_Proto_Arg_Res_Check_Unit_Item_t list
			} check;
			// STA
			struct Indr_Proto_Arg_Res_Sta_t {
				uint8_t sta;
				// 1: if all unit response OK, else ERROR (if any unit response ERROR)
				uint8_t result; 
			} sta;
			// RESET
			struct Indr_Proto_Arg_Res_Reset_t {
				// 1: if all unit response OK, else ERROR (if any unit response ERROR)
				uint8_t result; 
			} reset;
			// WRITE
			struct Indr_Proto_Arg_Res_Write_t {
				uint32_t address;
				uint16_t len;
				uint8_t ref;
			} write;
			// READ
			struct Indr_Proto_Arg_Res_Read_t {
				uint32_t address;
				uint16_t len;
				uint8_t ref;
			} read;
		} res;
		// Raw arg data
		uint8_t data[MAX_ARG_SIZE];
	} arg;
} Indr_Proto_t;

// Calculate payload len with given argument len
#define PAYLOAD_LEN_CALC(argLen) (argLen + 2 + 2 + 2)	// seq, cmd and crc
#define ARG_LEN_CALC_FROM_PAYLOAD_LEN(payloadLen) (payloadLen - 2 - 2 - 2) // seq, cmd, crc
#define ARG_LEN_CALC_FROM_PROTO_LEN(protoLen) (protoLen - 2 - 2) // seq, cmd

typedef enum {
	INDR_RX_IDLE = 0,
	INDR_RX_SYNC_0,
	INDR_RX_SYNC_1,
	INDR_RX_LEN_0,
	INDR_RX_PAYLOAD
} Indr_Proto_Rx_Stat_t;

typedef struct {
	Indr_Proto_Rx_Stat_t state;
	uint16_t len;
	uint16_t cnt;
	uint8_t payload[MAX_PAYLOAD_SIZE];
	FastCRC16 crc16;
	uint16_t packetCrc;
	Indr_Proto_t *proto;
} Indr_Proto_Rx_Parser_t;

typedef struct {
	uint16_t len;
	uint8_t payload[MAX_PAYLOAD_SIZE];
	FastCRC16 crc16;
	uint16_t packetCrc;
	Indr_Proto_t *proto;
} Indr_Proto_Tx_Builder_t;

#define INDRUINO_PROTO_SERIAL_BAUD	250000
#define INDRUINO_PROTO_SYNC_WORD	0xFFFF

// ref: point to Indruino_Bus instance
typedef void (*Indru_Bus_Callback)(void *ref, Indr_Proto_t *proto, uint16_t len);

class Indruino_Bus {

	public:
		// constructor with registered callback function
		Indruino_Bus(HardwareSerial& serial, Indru_Bus_Callback callback);
		// begin serial
		void begin();
		// send packet data
		void send(uint8_t *data, uint8_t sz);
		// should be periodically called in main loop
		void receive();
		// Re-init rx state
		void receive_state_init();
		// set parent
		void set_parent(void *p);

		// packet builder
		void tx_packet_crc_calc();
		void tx_packet_send();

		// debug
		void dummy_tx_packet();
		void dummy_rx_packet();

		// cmd `INDR_CHECK`
		void send_check_unit(uint16_t seq, uint8_t numOfUnit, uint8_t currentUnitId, uint32_t currentBaseAddr);

		// Sync word
		const uint16_t sync_word = INDRUINO_PROTO_SYNC_WORD;
		// Rx stack
		Indr_Proto_Rx_Parser_t rx;
		// Tx builder
		Indr_Proto_Tx_Builder_t tx;
		// parent: point to parent class (upper)
		void *parent;
		// success rx callback
		Indru_Bus_Callback _callback;
		// Serial port that protocol communication thru
		HardwareSerial& busSerial;

		// TODO: private, protected classify!
};

// debug
const __FlashStringHelper *indru_proto_debug_cmd_str(uint16_t cmd);

#endif // __INDRUINO_PROTOCOL_H_
