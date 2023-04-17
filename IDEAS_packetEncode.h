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
 * \file IDEAS_packetEncode.h
 * \author Integrated Detector Electronics AS
 * \brief Contains encoding functions for the IDEAS Readout and Control Protocol.
 *
 * \defgroup IDEAS_encode IDEAS Packet Encoding
 *
 * \addtogroup IDEAS_encode
 * @{
 */
#ifndef IDEAS_PACKETENCODE_H_
#define IDEAS_PACKETENCODE_H_

#include <stdint.h>
#include <string.h>

#include "IDEAS_packetTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief (Library internal function). Encodes an IDEAS packet header from parameters.
 * @param version       Version of packet protocol. 3 bits.
 * @param system_num	Unique ID number of system. 5 bits.
 * @param packet_type	Sequence the packet is part of. 
 * @param seq_flags     Specifies the sequence the packet is part of. 2 bits.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param data_len      Length of the "Packet Data" field in number of bytes.
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_header(
        uint8_t         version,
        uint8_t         system_num,
        uint8_t         packet_type,
        IDEAS_packetSeq seq_flags,
        uint16_t        packet_count,
        uint32_t        timestamp,
        uint16_t        data_len,
        uint8_t         *pEncBuffer
        );

/** 
 * @brief Encodes an IDEAS "Write System Register" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr      System register address.
 * @param reg_len       Length to write in number of bytes.
 * @param reg_data      Data to write. The most significant byte should be at the lowest index.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_writeSystemRegister(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,
        uint8_t     reg_len,
        uint8_t     *reg_data,

        uint8_t     *pEncBuffer
        );

/**
 * @brief Shorthand function for IDEAS_encode_writeSystemRegister(). Encodes an IDEAS "Write System Register" packet from a u8 variable.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr      System register address.
 * @param reg_var       Data to write. Up to 8 bits. Will always encode in 1 byte.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_writeSystemRegister_u8(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,
        uint8_t     reg_var,

        uint8_t     *pEncBuffer
        );

/**
 * @brief Shorthand function for IDEAS_encode_writeSystemRegister(). Encodes an IDEAS "Write System Register" packet from a u16 variable.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr      System register address.
 * @param reg_var       Data to write. Up to 16 bits. Will always encode in 2 bytes.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_writeSystemRegister_u16(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,
        uint16_t    reg_var,

        uint8_t     *pEncBuffer
        );

/**
 * @brief Shorthand function for IDEAS_encode_writeSystemRegister(). Encodes an IDEAS "Write System Register" packet from a u24 variable.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr      System register address.
 * @param reg_var       Data to write. Up to 24 bits. Will always encode in 3 bytes.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_writeSystemRegister_u24(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,
        uint32_t    reg_var,

        uint8_t     *pEncBuffer
        );


/**
 * @brief Shorthand function for IDEAS_encode_writeSystemRegister(). Encodes an IDEAS "Write System Register" packet from a u32 variable.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr      System register address.
 * @param reg_var       Data to write. Up to 32 bits. Will always encode in 4 bytes.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_writeSystemRegister_u32(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,
        uint32_t    reg_var,

        uint8_t     *pEncBuffer
        );

/** 
 * @brief Shorthand function for IDEAS_encode_writeSystemRegister(). Encodes an IDEAS "Write System Register" packet from a u64 variable.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr      System register address.
 * @param reg_var       Data to write. Up to 64 bits. Will always encode in 8 bytes.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_writeSystemRegister_u64(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,
        uint64_t    reg_var,

        uint8_t     *pEncBuffer
        );

/** 
 * @brief Encodes an IDEAS "Read System Register" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr	System register address.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_readSystemRegister(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,

        uint8_t     *pEncBuffer
        );

/** 
 * @brief Encodes an IDEAS "System Register Read-Back" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param reg_addr	System register address.
 * @param reg_len	Length that was read, in number of bits.
 * @param reg_data	Data that was read.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_systemRegisterReadBack(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint16_t    reg_addr,
        uint8_t     reg_len,
        uint8_t     *reg_data,

        uint8_t     *pEncBuffer
        );

/** 
 * @brief Encodes an IDEAS "ASIC Configuration Register Write/Read" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param asic_id       ID number of ASIC(s) to configure/that was configured.
 * @param conf_len      Length of ASIC configuration in bits.
 * @param conf_data     ASIC configuration data.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_asicConfigRegisterWriteRead(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint8_t     asic_id,
        uint16_t    conf_len,
        uint8_t     *conf_data,

        uint8_t     *pEncBuffer
        );

/**
 * @brief Encodes an "ASIC Configuration Register Read-Back" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param asic_id       ID number of ASIC(s) to configure/that was configured.
 * @param conf_len      Length of ASIC configuration in bits.
 * @param conf_data     ASIC configuration data.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_asicConfigRegisterReadBack(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint8_t     asic_id,
        uint16_t    conf_len,
        uint8_t     *conf_data,

        uint8_t     *pEncBuffer
        );

/**
 * @brief Encodes an "ASIC SPI Register Write" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param asic_id       ID number of ASIC(s) to configure.
 * @param spi_format    SPI format to use for the transaction.
 * @param reg_addr      ASIC SPI register address.
 * @param reg_len       Length to write, in number of bits.
 * @param reg_data      Data to write.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_asicSpiRegisterWrite(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint8_t     asic_id,
        uint8_t     spi_format,
        uint16_t    reg_addr,
        uint16_t    reg_len,
        uint8_t     *reg_data,

        uint8_t     *pEncBuffer
        );

/**
 * @brief Encodes an "ASIC SPI Register Read" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param asic_id       ID number of ASIC(s) to read.
 * @param spi_format    SPI format to use for the transaction.
 * @param reg_addr      ASIC SPI register address.
 * @param reg_len       Length to read, in number of bits.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_asicSpiRegisterRead(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint8_t     asic_id,
        uint8_t     spi_format,
        uint16_t    reg_addr,
        uint16_t    reg_len,

        uint8_t     *pEncBuffer
        );

/**
 * @brief Encodes an "ASIC SPI Register Readback" packet from parameters.
 *
 * @param system_num	Unique ID number of system. 5 bits.
 * @param timestamp     System timestamp when packet was created.
 * @param packet_count	Incremental counter for packet identification. 14 bits.
 *
 * @param asic_id       ID number of ASIC(s) that were read/written.
 * @param spi_format    SPI format that was used for the transaction.
 * @param reg_addr      ASIC SPI register address.
 * @param reg_len       Length of data that was read/written, in number of bits.
 * @param reg_data      Data that was read/written.
 *
 * @param pEncBuffer    Pointer to U8 buffer where the encoded packet will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_asicSpiRegisterReadback(
        uint8_t     system_num,
        uint16_t    packet_count,
        uint32_t    timestamp,

        uint8_t     asic_id,
        uint8_t     spi_format,
        uint16_t    reg_addr,
        uint16_t    reg_len,
        uint8_t     *reg_data,

        uint8_t     *pEncBuffer
        );


/**
 * @brief Encodes an unsigned 16 bit variable to a encoding buffer.
 * @param value         U16 value to encode.
 * @param pEncBuffer    Point to U8 buffer where the variable will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_u16(
        uint16_t    value,
        uint8_t     *pEncBuffer
        );

/**
 * @brief Encodes an unsigned 24 bit variable to a encoding buffer.
 * @param value         U24 value to encode.
 * @param pEncBuffer    Point to U8 buffer where the variable will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_u24(
        uint32_t    value,
        uint8_t     *pEncBuffer
        );

/**
 * @brief Encodes an unsigned 32 bit variable to a encoding buffer.
 * @param value         U32 value to encode.
 * @param pEncBuffer    Point to U8 buffer where the variable will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_u32(
        uint32_t    value,
        uint8_t     *pEncBuffer
        );

/**
 * @brief Encodes an unsigned 64 bit variable to a encoding buffer.
 * @param value         U64 value to encode.
 * @param pEncBuffer    Point to U8 buffer where the variable will be stored.
 */
IDEAS_API void IDEAS_CALL IDEAS_encode_u64(
        uint64_t    value,
        uint8_t     *pEncBuffer
        );

/**
 * @brief Gets the needed byte buffer length for a specified bit length.
 * @param bit_len   Bit length of data.
 * @return The needed byte buffer length for the bit data.
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcBufferByteLen(
        uint16_t bit_len
        );

/**
 * @brief Calculates the needed buffer length of a "Write System Register" packet.
 * @param reg_len Length of register (in bytes).
 * @return The needed buffer length of the "Write System Register packet".
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcLength_writeSystemRegister(
        uint8_t reg_len
        );

/**
 * @brief Calculates the needed buffer length of a "Read System Register" packet.
 * @return The needed buffer length of the "Read System Register packet".
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcLength_readSystemRegister();

/**
 * @brief Calculates the needed buffer length of a "System Register Readback" packet.
 * @param reg_len Length of register (in bytes).
 * @return The needed buffer length of the "System Register Readback packet".
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcLength_systemRegisterReadback(
        uint8_t reg_len
        );

/**
 * @brief Calculates the needed buffer length of an ASIC configuration packe.
 * @param conf_len Length of ASIC configuration in bits.
 * @return The needed buffer length of the ASIC configuration packet.
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcLength_asicConfig(
        uint16_t conf_len
        );

/**
 * @brief Calculates the needed buffer length of a "ASIC SPI Register Write" packet.
 * @param reg_len Length of register (in bits).
 * @return The needed buffer length of the "ASIC SPI Register Write".
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcLength_asicSpiRegisterWrite(
        uint16_t reg_len
        );

/**
 * @brief Calculates the needed buffer length of a "ASIC SPI Register Read" packet.
 * @return The needed buffer length of the "ASIC SPI Register Read".
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcLength_asicSpiRegisterRead();

/**
 * @brief Calculates the needed buffer length of a "ASIC SPI Register Readback" packet.
 * @param reg_len Length of register (in bits).
 * @return The needed buffer length of the "ASIC SPI Register Readback".
 */
IDEAS_API uint16_t IDEAS_CALL IDEAS_calcLength_asicSpiRegisterReadback(
        uint16_t reg_len
        );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* IDEAS_PACKETENCODE_H_ */
