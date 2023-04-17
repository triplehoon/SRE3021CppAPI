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
 * \file IDEAS_packetTypes.h
 * \author Integrated Detector Electronics AS
 * \brief Contains definitions used by the IDEAS packet library.
 *
 * \mainpage IDEAS Packet Library
 * \section intro Introduction
 * The IDEAS Packet Library is a C API for IDEAS (Integrated Detector Electronics AS) products.
 *
 * The library contains encoding and decoding functions for packets used to readout and control IDEAS detector systems and ASIC development kits.
 *
 * For more information supported devices, contact IDEAS or visit http://www.ideas.no
 *
 * \section legal Legal Notice
 * Copyright (C) 2015 Integrated Detector Electronics AS\n
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains \n
 * the property of Integrated Detector Electronics AS and its suppliers, \n
 * if any.  The intellectual and technical concepts contained \n
 * herein are proprietary to Integrated Detector Electronics AS \n
 * and its suppliers and may be covered by Norwegian, EU. or U.S. patents, \n
 * patents in process, and are protected by trade secret or copyright law. \n
 * Dissemination of this information or reproduction of this material \n
 * is strictly forbidden unless prior written permission is obtained \n
 * from Integrated Detector Electronics AS. \n
 *
 * \defgroup IDEAS_packetTypes IDEAS Packet Type Definitions
 *
 * \addtogroup \IDEAS_packetTypes
 * @{
 */
#ifndef IDEAS_PACKETTYPES_H_
#define IDEAS_PACKETTYPES_H_

#define IDEAS_PACKET_HEADER_LENGTH	10                          ///< Packet header length.
#define IDEAS_PACKET_MIN_LENGTH		IDEAS_PACKET_HEADER_LENGTH	///< Packet minimum length.

#define IDEAS_PACKET_DATA_WRITE_SYSTEM_REGISTER_MIN_LENGTH      3   ///< Minimum length of a Write System Register packet data field.
#define IDEAS_PACKET_DATA_SYSTEM_REGISTER_READBACK_MIN_LENGTH   3   ///< Minimum length of a System Register Readback packet data field.
#define IDEAS_PACKET_DATA_ASIC_CONF_MIN_LENGTH                  3   ///< Minimum length of an ASIC Configuration packet data field.
#define IDEAS_PACKET_DATA_ASIC_SPI_REGISTER_WRITE_MIN_LENGTH    6   ///< Minimum length of an ASIC SPI Register Write packet data field.
#define IDEAS_PACKET_DATA_ASIC_SPI_REGISTER_READBACK_MIN_LENGTH 6   ///< Minimum length of an ASIC SPI Register Readback packet data field.
#define IDEAS_PACKET_DATA_IMAGE_DATA_MIN_LENGTH                 19  ///< Minimum length of an Image Data packet data field.
#define IDEAS_PACKET_DATA_PULSE_HEIGHT_DATA_MIN_LENGTH          9   ///< Minimum length of a Single-Event Pulse-Height Data packet data field.
#define IDEAS_PACKET_DATA_MULTI_PULSE_HEIGHT_DATA_MIN_LENGTH    13  ///< Minimum length of a Multi-Event Pulse-Height Data packet data field.
#define IDEAS_PACKET_DATA_TRIGGER_TIME_DATA_MIN_LENGTH          6   ///< Minimum length of a Multi-Event Pulse-Height Data packet data field.
#define IDEAS_PACKET_DATA_PIPELINE_SAMPLING_DATA_LENGTH         326 ///< Length of a Pipeline Sampling Data packet data field.

#define IDEAS_PACKET_DATA_READ_SYSTEM_REGISTER_LENGTH           2   ///< Fixed length of a Read System Register packet data field.
#define IDEAS_PACKET_DATA_ASIC_SPI_REGISTER_READ_LENGTH         6   ///< Fixed length of an ASIC SPI Register Read packet data field.

#define IDEAS_PACKET_VERSION        0x00                        ///< Packet version.
#define IDEAS_PACKET_COUNT_MAX      0x3FFF                      ///< Packet header counter field maximum value.

#ifdef _WIN32
    #if defined(IDEAS_STATIC_LINK)
        #define IDEAS_API
        #define IDEAS_CALL
    #elif defined(IDEAS_BUILD_DLL)
        #define IDEAS_API __declspec(dllexport)
        #define IDEAS_CALL __cdecl
    #else
        #define IDEAS_API __declspec(dllimport)
        #define IDEAS_CALL __cdecl
    #endif
#else
    #define IDEAS_API
    #define IDEAS_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IDEAS packet type field enumeration.
 */
typedef enum {
    IDEAS_PACKET_TYPE_WRITE_SYSTEM_REGISTER     = 0x10, ///< Packet is of type "Write System Register".
    IDEAS_PACKET_TYPE_READ_SYSTEM_REGISTER      = 0x11, ///< Packet is of type "Read Sytem Register".
    IDEAS_PACKET_TYPE_SYSTEM_REGISTER_READBACK  = 0x12, ///< Packet is of type "System Register Readback".

    IDEAS_PACKET_TYPE_ASIC_CONF_WRITE_READ      = 0xC0, ///< Packet is of type "ASIC Configuration Write/Read".
    IDEAS_PACKET_TYPE_ASIC_CONF_READBACK        = 0xC1, ///< Packet is of type "ASIC Configuration Readback".

    IDEAS_PACKET_TYPE_ASIC_SPI_REGISTER_WRITE   = 0xC2, ///< Packet is of type "ASIC SPI Register Write".
    IDEAS_PACKET_TYPE_ASIC_SPI_REGISTER_READ    = 0xC3, ///< Packet is of type "ASIC SPI Register Read".
    IDEAS_PACKET_TYPE_ASIC_SPI_REGISTER_READBACK = 0xC4,///< Packet is of type "ASIC SPI Register Readback".

    IDEAS_PACKET_TYPE_COUNTING_FRAME_DATA       = 0xD0, ///< Packet is of type "Counting Frame Data".
    IDEAS_PACKET_TYPE_IMAGE_DATA                = 0xD1, ///< Packet is of type "Image Data".
    IDEAS_PACKET_TYPE_MULTI_EVENT_PULSE_HEIGHT_DATA = 0xD4, ///< Packet is of type "Multi-Event Pulse-Height Data".
    IDEAS_PACKET_TYPE_PULSE_HEIGHT_DATA         = 0xD5, ///< Packet is of type "Single-Event Pulse-Height Data".
    IDEAS_PACKET_TYPE_TRIGGER_TIME_DATA			= 0xD6, ///< Packet is of type "Trigger Time Data".
    IDEAS_PACKET_TYPE_PIPELINE_SAMPLING_DATA    = 0xD9, ///< Packet is of type "Pipeline Sampling Data".

    IDEAS_PACKET_TYPE_UNDEFINED                 = 0xFF  ///< Packet type is undefined.
} IDEAS_packetType;

/** 
 * @brief Specifies the sequence the packet is part of. 
 */
typedef enum {
	IDEAS_PACKET_SEQ_STANDALONE 		= 0x00,	///< Packet is the only packet in the sequence.
	IDEAS_PACKET_SEQ_FIRSTPACKET		= 0x01, ///< Packet is the first packet in the sequence.
    IDEAS_PACKET_SEQ_CONTINUATIONPACKET = 0x02, ///< Packet continues a started sequence.
    IDEAS_PACKET_SEQ_LASTPACKET         = 0x03	///< Packet is tha last packet in the sequence.
} IDEAS_packetSeq;

/**
 * @brief Event trigger types.
 */
typedef enum {
    IDEAS_PACKET_TRIGGER_TYPE_SYSTEM_AUTO_FORCED_READOUT    = 0x00, ///< Readout was forced by the system (using the system clock).
    IDEAS_PACKET_TRIGGER_TYPE_SOFTWARE_TRIGGER              = 0x01, ///< Readout was triggered by a software trigger from the user.
    IDEAS_PACKET_TRIGGER_TYPE_ASIC_TRIGGER                  = 0x02, ///< Readout was triggered by the ASIC from a pulse on the input (i.e. a physical event).
    IDEAS_PACKET_TRIGGER_TYPE_CALIBRATION_TRIGGER           = 0x03, ///< Readout was forced by the calibration pulse generator.
    IDEAS_PACKET_TRIGGER_TYPE_EXTERNAL_TRIGGER              = 0x04, ///< Readout was triggered by an external trigger signal.
    IDEAS_PACKET_TRIGGER_TYPE_SINGLE_CHANNEL_READOUT_MODE   = 0x05, ///< Readout contains only data from the triggered channel.
    IDEAS_PACKET_TRIGGER_TYPE_XA_MULTI_HIT_TRIGGER          = 0x06, ///< Readout contains multi-hit trigger data from an XA ASIC.
    IDEAS_PACKET_TRIGGER_TYPE_UNDEFINED                     = 0xFF  ///< Trigger type is undefined.
} IDEAS_triggerType;

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* IDEAS_PACKETTYPES_H_ */
