/*
 * Copyright (C) 2015 Integrated Detector Electronics AS
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Integrated Detector Electronics AS and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Integrated Detector Electronics AS
 * and its suppliers and may be covered by Norwegian, EU. or U.S. patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Integrated Detector Electronics AS.
 */
/** 
 * \file IDEAS_packetDecode.h
 * \author Integrated Detector Electronics AS
 * \brief Contains decoding functions for the IDEAS Readout and Control Protocol.
 *
 * \defgroup IDEAS_decode IDEAS Packet Decoding
 *
 * \addtogroup IDEAS_decode
 * @{
 */
#ifndef IDEAS_PACKETDECODE_H_
#define IDEAS_PACKETDECODE_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "IDEAS_packetTypes.h"
#include "IDEAS_packetEncode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IDEAS_PACKET_CS_ASICS_PER_PACKET       7       ///< For GRT data: number of ASICs per packet.
#define IDEAS_PACKET_CS_COUNTERS_PER_ASIC      64      ///< For GRT data: number of counters per ASIC.
#define IDEAS_PACKET_CS_OF_FLAGS_PER_ASIC      (IDEAS_PACKET_CS_COUNTERS_PER_ASIC/8) ///< For counting data: number of overflow flags per ASIC
#define IDEAS_PACKET_CS_COUNTERS_PER_PACKET    (IDEAS_PACKET_CS_ASICS_PER_PACKET*IDEAS_PACKET_CS_COUNTERS_PER_ASIC)  ///< For GRT data: number of counters per packet.
#define IDEAS_PACKET_CS_PACKET_LEN             953     ///< For GRT data: packet length of GRT packet.

/**
 * IDEAS packet decode status and error codes.
 */
typedef enum {
    IDEAS_PACKET_DECODE_SUCCESS                             = 0x00, ///< Packet decode was successful.
    IDEAS_PACKET_DECODE_ERROR_LENGTH_NO_HEADER				= 0x10,	///< The packet length is below the header size.
    IDEAS_PACKET_DECODE_ERROR_LENGTH_NO_DATA				= 0x11, ///< The packet data field is below the expected length.
    IDEAS_PACKET_DECODE_ERROR_INCORRECT_LENGTH              = 0x12, ///< The packet or a packet field had incorrect length.
    IDEAS_PACKET_DECODE_ERROR_DATA_LENGTH_FIELD_MISMATCH    = 0x13, ///< The data field of the packet did not match the length of the packet.
    IDEAS_PACKET_DECODE_ERROR_FIELD_VALUE_INCORRECT         = 0x14, ///< A field had an erroneous value.
    IDEAS_PACKET_DECODE_ERROR_PACKET_TYPE_INCORRECT         = 0x15, ///< The packet type is incorrect.
    IDEAS_PACKET_DECODE_ERROR_MEMORY_ALLOCATION             = 0xA0, ///< The system was not able to allocate the requested memory.
    IDEAS_PACKET_DECODE_ERROR_UNKNOWN                       = 0xFF  ///< An unknown decoding error occured.
} IDEAS_packet_decodeStatus;

/** 
 * IDEAS Packet Header structure
 */
typedef struct {
    uint8_t             version;		//< Version of packet protocol. 3 bits.
    uint8_t             system_num;		//< Unique ID number of system. 5 bits.
    IDEAS_packetType    packet_type;	//< Sequence the packet is part of.
    IDEAS_packetSeq     seq_flags;      //< Specifies the sequence the packet is part of. 2 bits.
    uint16_t            packet_count;	//< Incremental counter for packet identification. 14 bits.
    uint32_t            timestamp;		//< System timestamp when packet was created.
    uint16_t            data_len;		//< Length of the "Packet Data" field in number of bytes.
} IDEAS_packet_header; 

/**
 * Write System Register packet data structure
 */
typedef struct {
    uint16_t    reg_addr;		///< System register address.
    uint8_t     reg_len;		///< Length to write.
    uint8_t     *reg_data;		///< Data to write.
} IDEAS_packet_writeSystemRegister;

/**
 * Read System Register packet data structure.
 */
typedef struct {
    uint16_t	reg_addr;		///< System register address.
} IDEAS_packet_readSystemRegister;


/**
 * System Register Read-Back packet data structure.
 */
typedef struct {
    uint16_t	reg_addr;		///< System register address.
    uint8_t     reg_len;		///< Length of read-back register.
    uint8_t     *reg_data;		///< Read-back register data.
} IDEAS_packet_systemRegisterReadBack;

/**
 * ASIC configuration packet data structure.
 */
typedef struct {
    uint8_t     asic_id;		///< ID number of ASIC(s) to configure/that was configured. 
    uint16_t	conf_len;		///< Length of ASIC configuration in bits.
    uint8_t     *conf_data;		///< ASIC configuration data.
} IDEAS_packet_asicConfig;

/**
 * ASIC SPI Register Write/Read/Readback data structure
 */
typedef struct {
    uint8_t     asic_id;		///< ID number of ASIC(s) to configure/that was configured.
    uint8_t     spi_format;     ///< SPI format to use/that was used for the transacation.
    uint16_t	reg_addr;		///< ASIC SPI register address.
    uint16_t    reg_len;		///< Length of ASIC SPI register.
    uint8_t     *reg_data;		///< ASIC SPIregister data.
} IDEAS_packet_asicSpiRegister;

/**
 * @brief Frame data packet counter group (for an ASIC) structure
 */
typedef struct {
    uint16_t    counters[IDEAS_PACKET_CS_COUNTERS_PER_ASIC];   ///< Counters for one ASIC.
    uint8_t     of_flags[IDEAS_PACKET_CS_OF_FLAGS_PER_ASIC];   ///< Overflow flags for one ASIC.
} IDEAS_packet_counterGroup;

/**
 * @brief Counting frame 16-bit data packet structure.
 */
typedef struct {
    uint8_t                     data_group; ///< ID of data group the data belongs to.
    IDEAS_packet_counterGroup   counter_groups[IDEAS_PACKET_CS_ASICS_PER_PACKET];  ///< Grouped counter data (per ASIC).
} IDEAS_packet_countingFrameData;

/**
 * @brief Packet image data. Part of a full frame split into several packets.
 */
typedef struct {
    uint8_t     system_num;         ///< The system num from the packet header.
    uint8_t     packet_type;		///< The packet type from packet header.

    uint16_t    frame_number;       ///< The frame number of the image.
    uint16_t    image_width;		///< The width of the image.
    uint16_t    image_height;		///< The height of the image.
    uint16_t    spectral_channels;	///< The number of spectral channels in the image.
    uint8_t     reserved;		///< Reserved field.
    uint8_t     data_width;		///< The data width of the pixel data in the image. Must be a multiple of 2.
    uint32_t    userDefined;		///< User Defined .
    uint16_t    num_data_packets;	///< The number of data packets in the image packet sequence.
    uint16_t    data_seq;		///< The position of the packet in the image packet sequence.
    uint8_t     *pImageData;		///< Array containing the encoded (partial) image data.
    uint16_t    imageDataLen;		///< Length of the image data array (pImageData).
} IDEAS_packet_imageData;

/**
 * @brief 16-bit image data structure.
 */
typedef struct {
    uint8_t     system_num;         ///< The system num from the packet header.
    uint8_t     packet_type;		///< The packet type from packet header.

    uint16_t    frame_number;       ///< The frame number of the image.
    uint16_t    image_width;		///< The width of the image.
    uint16_t    image_height;		///< The height of the image.
    uint16_t    spectral_channels;	///< The number of spectral channels in the image.
    uint32_t    userDefined;		///< User Defined.
    uint16_t    **pImageData;		///< 2D array containing N (spectral channels) number of frames, each of the specified width x height.
    uint16_t    imageDataLen;		///< Length of each frame in the image data array.
} IDEAS_packet_image_u16;

/**
 * @brief Pulse height 16-bit data packet structure. For Single-Event Pulse-Height Data.
 */
typedef struct {
    uint8_t             source_id;      ///< ID of source the data belongs to.
    IDEAS_triggerType   trigger_type;   ///< Type of trigger that caused the readout.
    uint8_t             channel_id;     ///< ID of (last) readout channel.
    uint16_t            hold_delay;     ///< Hold delay that was used for the readout.
    uint16_t            num_samples;    ///< Number of samples readout out.
    uint16_t            *samples;       ///< 16-bit peak height list data.
} IDEAS_packet_pulseHeightData;

/**
 * @brief Pulse height 16-bit sample structure. For Multi-Event Pulse-Height Data.
 */
typedef struct {
    IDEAS_triggerType   trigger_type;   ///< Sample trigger type.
    uint8_t             source_id;      ///< Source ID of sample.
    uint8_t             channel_id;     ///< Channel ID of sample.
    uint16_t            sample;         ///< 16-bit peak height sample.
} IDEAS_packet_pulseHeightSample;

/**
 * @brief Pulse height 16-bit data event structure. For Multi-Event Pulse-Height Data.
 */
typedef struct {
    uint32_t                        timestamp;  ///< System timestamp of event. Generated in hardware.
    IDEAS_packet_pulseHeightSample  *samples;   ///< Event data structure array.
} IDEAS_packet_pulseHeightEvent;

/**
 * @brief Pulse height 16-bit data packet structure. For Multi-Event Pulse-Height Data.
 */
typedef struct {
    uint8_t                         num_events;     ///< Number of events in packet.
    uint16_t                        num_samples;    ///< Number of samples per event.
    IDEAS_packet_pulseHeightEvent   *events;        ///< Event data structure array.
} IDEAS_packet_multiEventPulseHeightData;

/**
 * @brief Trigger time data event structure.
 */
typedef struct {
    uint32_t    timestamp;          ///< System timestamp of event. Generated in hardware.
    uint8_t     source_id;          ///< Source ID of trigger
    uint8_t     channel_id;         ///< Channel ID of trigger
} IDEAS_packet_triggerTimeEvent;

/**
 * @brief Trigger time data packet structure.
 */
typedef struct {
    uint8_t                         num_events;     ///< Number of events in packet.
    IDEAS_packet_triggerTimeEvent   *events;        ///< Event data structure array.
} IDEAS_packet_triggerTimeData;

/**
 * @brief Pipeline sampling 16-bit data packet structure.
 */
typedef struct {
    uint8_t     source_id;          ///< ID of source the data belongs to.
    uint8_t     trigger_type;       ///< Type of trigger that caused the readout.
    uint16_t    hold_delay;         ///< Hold delay that was used for the readout.

    uint8_t     channel_type;       ///< 0: Cathode, 1: Anode.
    uint8_t     cell_pointer;       ///< (Only populated if Channel Type is 'Cathode'.) Shows the column pipeline pointer at the time of the trigger. Values: 0-159.
    uint8_t     trigger_flag;       ///< (Only populated if Channel Type is 'Anode'.) If ASIC is not in â€œfull readoutâ€� mode: specifies if the channel triggered the event. 0b1 if triggered, 0b0 if not triggered. If ASIC is in â€œfull readoutâ€� mode: always equal to 0b1.
    uint8_t     x_addr;             ///< (Only populated if Channel Type is 'Anode'.) 4-bit unsigned integer, specifying the x-address of the channel.
    uint8_t     y_addr;             ///< (Only populated if Channel Type is 'Anode'.) 4-bit unsigned integer, specifying the x-address of the channel.

    uint16_t    cell_data[160];     ///< Digitized channel data. 160 values, corresponding to the sample and hold pipeline cells of the channel. 14-bit ADC value + 1 bit overflow.
    uint8_t     cell_data_len;      ///< Number of values in 'cell_data' array. For convience only. Fixed to 160.

} IDEAS_packet_pipelineSamplingData;

/** 
 * @brief Decodes an IDEAS packet header. 
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pHeader       Pointer to header struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_header(
        uint8_t             *pBuffer,
        uint16_t            buffer_len,
        IDEAS_packet_header *pHeader
        );

/** 
 * @brief Decodes an IDEAS "Write System Register" packet data field.
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_writeSystemRegister(
        uint8_t                             *pBuffer,
        uint16_t                             buffer_len,
        IDEAS_packet_writeSystemRegister    *pPacketData
        );

/** 
 * @brief Decodes an IDEAS "Read System Register" packet data field.
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_readSystemRegister(
        uint8_t                             *pBuffer,
        uint16_t                             buffer_len,
        IDEAS_packet_readSystemRegister     *pPacketData
        );

/** 
 * @brief Decodes an IDEAS "System Register Read-Back" packet data field.
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_systemRegisterReadBack(
        uint8_t                                 *pBuffer,
        uint16_t                                 buffer_len,
        IDEAS_packet_systemRegisterReadBack     *pPacketData
        );

/** 
 * @brief Decodes an IDEAS "ASIC Configuration Register Write/Read" or "ASIC Configuration Register Read-Back" packet data field.
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_asicConfigRegister(
        uint8_t                     *pBuffer,
        uint16_t                     buffer_len,
        IDEAS_packet_asicConfig     *pPacketData
        );
/**
 * @brief Decodes an IDEAS "ASIC SPI Register Write" packet data field.
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_asicSpiRegisterWrite(
        uint8_t                         *pBuffer,
        uint16_t                        buffer_len,
        IDEAS_packet_asicSpiRegister    *pPacketData
        );

/**
 * @brief Decodes an IDEAS "ASIC SPI Register Read" packet data field.
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_asicSpiRegisterRead(
        uint8_t                         *pBuffer,
        uint16_t                        buffer_len,
        IDEAS_packet_asicSpiRegister    *pPacketData
        );

/**
 * @brief Decodes an "ASIC SPI Register Readback" packet data field.
 * @param pBuffer       Buffer containing encoded packet data to decode.
 * @param buffer_len	Length of data buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_asicSpiRegisterReadback(
        uint8_t                         *pBuffer,
        uint16_t                        buffer_len,
        IDEAS_packet_asicSpiRegister    *pPacketData
        );

/**
 * @brief Decode an IDEAS "Counting Frame Data" packet data field.
 * @param pData         Pointer to buffer containing encoded packet data.
 * @param data_len      Length of packet data in buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_countingFrameData(
        uint8_t                         *pData,
        uint16_t                        data_len,
        IDEAS_packet_countingFrameData  *pPacketData
        );

/**
 * @brief Decode an IDEAS "Image Data" packet data field.
 * @param system_num    System number field from the packet header.
 * @param packet_type   Packet Type field from the packet header.
 * @param pData         Pointer to buffer containing encoded packet data.
 * @param data_len      Length of packet data in buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_imageData(
		uint8_t                 system_num,
		uint8_t                 packet_type,
        uint8_t                 *pData,
        uint16_t                data_len,
        IDEAS_packet_imageData  *pPacketData
        );

/**
 * @brief Decode an IDEAS "Image" from one or multiple data packets.
 * @param pPacketHeader    		Pointer to header struct containing  packet header.
 * @param pData         		Pointer to buffer containing encoded packet data.
 * @param data_len      		Length of packet data in buffer.
 * @param pcurrentImageU16  	Pointer to packet data struct for storing the decoded data.
 * @param pFull_Image_Recieved  Pointer to Full image received status.
 	 	 	 	 	 	 	 	 Set to TRUE, when the complete image has been received.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_imagePacketsData(
		IDEAS_packet_header 	*pPacketHeader,
        uint8_t                 *pData,
        uint16_t                data_len,
        IDEAS_packet_image_u16 	*pcurrentImageU16,
        uint8_t					*pFull_Image_Recieved
        );

/**
 * @brief Decode an image from IDEAS "Image Data" packets.
 * @param pPacketData       Pointer to array of decoded packet structs.
 * @param data_len      	Length of packet data in buffer. 
 * @param pImage            Pointer to image data struct for storing the image.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_image_u16(
        IDEAS_packet_imageData  **pPacketData,
        uint16_t                data_len,
        IDEAS_packet_image_u16  *pImage
        );

/**
 * @brief Decode an IDEAS "Single-Event Pulse-Height Data" packet data field.
 * @param pData         Pointer to buffer containing encoded packet data.
 * @param data_len      Length of packet data in buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_pulseHeightData(
        uint8_t                         *pData,
        uint16_t                        data_len,
        IDEAS_packet_pulseHeightData    *pPacketData
        );

/**
 * @brief Decode an IDEAS "Multi-Event Pulse-Height Data" packet data field.
 * @param pData         Pointer to buffer containing encoded packet data.
 * @param data_len      Length of packet data in buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_multiEventPulseHeightData(
        uint8_t                                 *pData,
        uint16_t                                data_len,
        IDEAS_packet_multiEventPulseHeightData  *pPacketData
        );
        
/**
 * @brief Decode an IDEAS "Trigger Time Data" packet data field.
 * @param pData         Pointer to buffer containing encoded packet data.
 * @param data_len      Length of packet data in buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_triggerTimeData(
        uint8_t                                 *pData,
        uint16_t                                data_len,
        IDEAS_packet_triggerTimeData            *pPacketData
        );

/**
 * @brief Decode an IDEAS "Pipeline Sampling Data" packet data field.
 * @param pData         Pointer to buffer containing encoded packet data.
 * @param data_len      Length of packet data in buffer.
 * @param pPacketData   Pointer to packet data struct for storing the decoded data.
 * @return Packet decode status.
 */
IDEAS_API IDEAS_packet_decodeStatus IDEAS_CALL IDEAS_decode_pipelineSamplingData(
        uint8_t                             *pData,
        uint16_t                            data_len,
        IDEAS_packet_pipelineSamplingData   *pPacketData
        );

/**
 * @brief Decodes a U16 variable from a encoded buffer.
 * @param pBuffer Pointer to buffer with encoded data.
 * @return Decoded U16 variable.
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_decode_u16(
        uint8_t *pBuffer
        );

/**
 * @brief Decodes a U24 variable from a encoded buffer.
 * @param pBuffer Pointer to buffer with encoded data.
 * @return Decoded U24 variable.
 */
IDEAS_API uint32_t IDEAS_CALL IDEAS_decode_u24(
        uint8_t *pBuffer
        );

/**
 * @brief Decodes a U32 variable from a encoded buffer.
 * @param pBuffer Pointer to buffer with encoded data.
 * @return Decoded U32 variable.
 */
IDEAS_API uint32_t IDEAS_CALL IDEAS_decode_u32(
        uint8_t *pBuffer
        );

/**
 * @brief Decodes a U64 variable from a encoded buffer.
 * @param pBuffer Pointer to buffer with encoded data.
 * @return Decoded U64 variable.
 */
IDEAS_API uint64_t IDEAS_CALL IDEAS_decode_u64(
        uint8_t *pBuffer
        );

/**
 * @brief Frees memory allocated by IDEAS_decode_writeSystemRegister.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_writeSystemRegister(
    IDEAS_packet_writeSystemRegister *pPacketData
	);

/**
 * @brief Frees memory allocated by IDEAS_decode_systemRegisterReadBack.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_systemRegisterReadBack(
    IDEAS_packet_systemRegisterReadBack *pPacketData
	);

/**
 * @brief Frees memory allocated by IDEAS_decode_asicConfigRegister.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_asicConfigRegister(
    IDEAS_packet_asicConfig *pPacketData
	);
/**
 * @brief Frees memory allocated by IDEAS_decode_asicSpiRegisterWrite or IDEAS_decode_asicSpiRegisterReadback.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_asicSpiRegister(
    IDEAS_packet_asicSpiRegister *pPacketData
    );

/**
 * @brief Frees memory allocated by IDEAS_decode_pulseHeightData.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_pulseHeightData(
    IDEAS_packet_pulseHeightData *pPacketData
    );

/**
 * @brief Frees memory allocated by IDEAS_decode_multiEventPulseHeightData.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_multiEventPulseHeightData(
    IDEAS_packet_multiEventPulseHeightData *pPacketData
    );
    
/**
 * @brief Frees memory allocated by IDEAS_decode_triggerTimeData.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_triggerTimeData(
    IDEAS_packet_triggerTimeData *pPacketData
    );

/**
 * @brief Frees memory allocated by IDEAS_decode_imageData.
 * @param pPacketData Previously allocated Packet Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_imageData(
    IDEAS_packet_imageData *pPacketData
    );

/**
 * @brief Frees memory allocated by IDEAS_decode_image_u16.
 * @param pImage Previously allocated Packet Image Data structure.
 */
IDEAS_API void IDEAS_CALL IDEAS_free_image_u16(
    IDEAS_packet_image_u16 *pImage
    );

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* IDEAS_PACKETDECODE_H_ */
