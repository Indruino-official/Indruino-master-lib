
/*
	Source file for Indruino protocol

	Initial idea & demo: CuongLT
	Modified: HieuNT
	28/02/2017
*/

#define __DEBUG__ 4
#define __MODULE__	"protocol"
#include "user_debug.h"

#include "Indruino_Protocol.h"
#include "FastCRC.h"

#define rx_state_init() rx.state = INDR_RX_IDLE

Indruino_Bus::Indruino_Bus(HardwareSerial& serial, Indru_Bus_Callback callback)
	:busSerial(serial), _callback(callback)
{
	memset(&rx, 0, sizeof(rx));
	memset(&tx, 0, sizeof(tx));
	rx.proto = (Indr_Proto_t *)&rx.payload[0];
	tx.proto = (Indr_Proto_t *)&tx.payload[0];
	rx.state = INDR_RX_IDLE;
	// warning: DO NOT debug() here ;)
}

void Indruino_Bus::begin()
{
	this->busSerial.begin(INDRUINO_PROTO_SERIAL_BAUD);
	this->busSerial.flush();
	// debug("Indruino_Bus::begin\n");
}

void Indruino_Bus::send(uint8_t *data, uint8_t sz)
{
	for (int i = 0; i < sz; ++i)
	{
		busSerial.write(*data++);
	}
}

void Indruino_Bus::receive()
{
	while(busSerial.available()){
		uint8_t data = busSerial.read();
		// debug("data: %02X\n", data);
		switch (rx.state){
			case INDR_RX_IDLE:
				if (data == (*(uint8_t *)(&sync_word))){
					rx.state = INDR_RX_SYNC_0;
					// debug("sync0\n");
				}
				break;
			case INDR_RX_SYNC_0:
				// bug fix: rem to cast address to uint32_t before inc.
				if (data == (*(uint8_t *)((uint32_t)&sync_word+1))){
					rx.state = INDR_RX_SYNC_1;
					rx.crc16.modbus((const uint8_t *)&sync_word, 2);	
					// debug("sync1\n");
				}
				else {
					rx_state_init();
				}
				break;
			case INDR_RX_SYNC_1:
				rx.len = data;
				rx.state = INDR_RX_LEN_0;
				// debug("len0\n");
				break;
			case INDR_RX_LEN_0:
				rx.len |= (uint16_t)data << 8;
				// payload len must less than max payload and greater than crc len
				if (rx.len > MAX_PAYLOAD_SIZE || rx.len < 2){
					error("len = %u, ignore\n", rx.len);
					rx_state_init();
					break;
				}
				rx.cnt = 0;
				rx.state = INDR_RX_PAYLOAD;
				rx.crc16.modbus_upd((const uint8_t *)&rx.len, 2);
				// debug("len1->payload\n");
				break;
			case INDR_RX_PAYLOAD:
				rx.payload[rx.cnt] = data;
				// just calc crc for none-crc payload
				if (rx.cnt < rx.len - 2){
					rx.packetCrc = rx.crc16.modbus_upd((const uint8_t *)&rx.payload[rx.cnt], 1);
				}
				rx.cnt++;
				if (rx.cnt == rx.len){
					// debug("payload complete\n");
					if (rx.packetCrc == *(uint16_t *)&rx.payload[rx.cnt-2]){
						// debug("valid crc, callback...\n");
						if (_callback != NULL) _callback((void *)this, (Indr_Proto_t *)&rx.payload, rx.len-2);
					}
					else {
						error("invalid crc, ignore\n");
					}
					rx_state_init();
				}
				break;
			default:
				// unexpected error
				rx_state_init();
				break;
		}
	}

	// TODO: rx timeout in all state != idle
}

void Indruino_Bus::receive_state_init()
{
	rx_state_init();
}

void Indruino_Bus::set_parent(void *p)
{
	parent = p;
}

void Indruino_Bus::tx_packet_crc_calc()
{
	// sync
	tx.crc16.modbus((const uint8_t *)&sync_word, 2);
	// payload len
	tx.crc16.modbus_upd((const uint8_t *)&tx.len, 2);
	// payload
	tx.packetCrc = tx.crc16.modbus_upd((const uint8_t *)tx.proto, tx.len - 2); // ex crc
	// fix: update to packet itself --> de^~ debug.
	*(uint16_t *)&tx.payload[tx.len-2] = tx.packetCrc;
}

void Indruino_Bus::tx_packet_send()
{
	// sync
	send((uint8_t *)&sync_word, 2);
	// payload len
	send((uint8_t *)&tx.len, 2);
	// payload
	send((uint8_t *)tx.proto, tx.len - 2); // ex crc
	// crc
	send((uint8_t *)&tx.packetCrc, 2); // finish!
}

void Indruino_Bus::send_check_unit(uint16_t seq, uint8_t numOfUnit, uint8_t currentUnitId, uint32_t currentBaseAddr)
{
	// payload len
	tx.len = PAYLOAD_LEN_CALC(sizeof(tx.proto->arg.req.check));
	// payload
	tx.proto->seq = seq;
	tx.proto->cmd = INDR_CHECK;
	tx.proto->arg.req.check.totalUnit = numOfUnit;
	tx.proto->arg.req.check.currentUnitId = currentUnitId;
	tx.proto->arg.req.check.currentBaseAddr = currentBaseAddr;
	tx_packet_crc_calc();
	tx_packet_send();
}

// debug
void Indruino_Bus::dummy_tx_packet()
{
	debug("dummy tx: seq %04X, cmd %04X, arg ", tx.proto->seq, tx.proto->cmd);
	for (int i = 0; i < ARG_LEN_CALC_FROM_PAYLOAD_LEN(tx.len); ++i)
	{
		debugx("%02X ", tx.proto->arg.data[i]);
	}
	debugx(", crc %02X%02X\n", tx.payload[tx.len-2], tx.payload[tx.len-1]);
}

void Indruino_Bus::dummy_rx_packet()
{
	debug("dummy rx: seq %04X, cmd %04X, arg ", rx.proto->seq, rx.proto->cmd);
	for (int i = 0; i < ARG_LEN_CALC_FROM_PAYLOAD_LEN(rx.len); ++i)
	{
		debugx("%02X ", rx.proto->arg.data[i]);
	}
	debugx(", crc %02X%02X\n", rx.payload[rx.len-2], rx.payload[rx.len-1]);
}

const __FlashStringHelper *indru_proto_debug_cmd_str(uint16_t cmd)
{
	switch (cmd){
		case INDR_CHECK:
			return F("INDR_CHECK");
			// break;
		case INDR_STA:
			return F("INDR_STA");
			// break;
		case INDR_RESET:
			return F("INDR_RESET");
			// break;
		case INDR_WRITE:
			return F("INDR_WRITE");
			// break;
		case INDR_READ:
			return F("INDR_READ");
			// break;
	}
	return F("NOT FOUND!");
}
