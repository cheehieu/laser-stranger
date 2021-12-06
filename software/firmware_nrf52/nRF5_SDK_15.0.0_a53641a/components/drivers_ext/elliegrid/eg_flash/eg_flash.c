#include "eg_flash.h"

// Flag to check fds initialization
static bool volatile m_fds_initialized;

// Array to map FDS events to strings
static char const * fds_evt_str[] = {
    "FDS_EVT_INIT",
    "FDS_EVT_WRITE",
    "FDS_EVT_UPDATE",
    "FDS_EVT_DEL_RECORD",
    "FDS_EVT_DEL_FILE",
    "FDS_EVT_GC",
};

// Array to map FDS return values to strings
static char const * fds_err_str[] = {
    "FDS_SUCCESS",
    "FDS_ERR_OPERATION_TIMEOUT",
    "FDS_ERR_NOT_INITIALIZED",
    "FDS_ERR_UNALIGNED_ADDR",
    "FDS_ERR_INVALID_ARG",
    "FDS_ERR_NULL_ARG",
    "FDS_ERR_NO_OPEN_RECORDS",
    "FDS_ERR_NO_SPACE_IN_FLASH",
    "FDS_ERR_NO_SPACE_IN_QUEUES",
    "FDS_ERR_RECORD_TOO_LARGE",
    "FDS_ERR_NOT_FOUND",
    "FDS_ERR_NO_PAGES",
    "FDS_ERR_USER_LIMIT_REACHED",
    "FDS_ERR_CRC_CHECK_FAILED",
    "FDS_ERR_BUSY",
    "FDS_ERR_INTERNAL",
};

void eg_fds_evt_handler(fds_evt_t const * p_evt) {
    if (p_evt->result == FDS_SUCCESS) {
	NRF_LOG_DEBUG("Event: %s received (%s)", fds_evt_str[p_evt->id], fds_err_str[p_evt->result]);
    } else {
	NRF_LOG_ERROR("Event: %s received (%s)", fds_evt_str[p_evt->id], fds_err_str[p_evt->result]);
    }
    
    switch (p_evt->id) {
	case FDS_EVT_INIT: {
            if (p_evt->result == FDS_SUCCESS) {
                m_fds_initialized = true;
            }
	} break;
	    
	case FDS_EVT_WRITE: {
            if (p_evt->result == FDS_SUCCESS) {
//                NRF_LOG_DEBUG("Record ID:\t0x%04x", p_evt->write.record_id);
//                NRF_LOG_DEBUG("File ID:\t0x%04x", p_evt->write.file_id);
//                NRF_LOG_DEBUG("Rec Key:\t0x%04x", p_evt->write.record_key);
		fds_write_flag = 1;	// Indicate successful write
		eg_fds_free_words();	// Check for GC
            }
        } break;

	case FDS_EVT_UPDATE: {
	    if (p_evt->result == FDS_SUCCESS) {
		eg_fds_free_words();	// Check for GC
	    }
	} break;
	    
	case FDS_EVT_DEL_RECORD: {
            if (p_evt->result == FDS_SUCCESS) {
//                NRF_LOG_DEBUG("Record ID:\t0x%04x",  p_evt->del.record_id);
//                NRF_LOG_DEBUG("File ID:\t0x%04x",    p_evt->del.file_id);
//                NRF_LOG_DEBUG("Rec Key:\t0x%04x", p_evt->del.record_key);
		eg_fds_free_words();	// Check for GC	
            }
//            m_delete_all.pending = false;
        } break;
	    
	case FDS_EVT_DEL_FILE: {
	    if (p_evt->result == FDS_SUCCESS) {
		eg_fds_free_words();	// Check for GC
	    }
	} break;
	    
	case FDS_EVT_GC:
	    break;
	    
	default:
	    break;
    }
}

    
void eg_fds_init(void) {
    // Register first to receive an event when initialization is complete
    (void) fds_register(eg_fds_evt_handler);
    
    ret_code_t err_code = fds_init();
    APP_ERROR_CHECK(err_code);
    
    // Wait for fds to initialize
    while (!m_fds_initialized) {
	(void) sd_app_evt_wait();
    }
}


void eg_fds_read_all(void) {
    ret_code_t err_code;
    fds_find_token_t tok   = {0};
    fds_record_desc_t desc = {0};

    NRF_LOG_DEBUG("FILE_ID\t\tREC_KEY\tLEN\t\t\tVALUE");

    while (fds_record_iterate(&desc, &tok) != FDS_ERR_NOT_FOUND) {
        fds_flash_record_t frec = {0};

        err_code = fds_record_open(&desc, &frec);
	APP_ERROR_CHECK(err_code);

        uint32_t const len = frec.p_header->length_words * BYTES_PER_WORD;
        NRF_LOG_DEBUG(" 0x%04x\t\t 0x%04x\t\t %4u bytes\t 0x%08x",
                    frec.p_header->file_id,
                    frec.p_header->record_key,
                    len,
		    *(uint32_t *) frec.p_data);

        err_code = fds_record_close(&desc);
        APP_ERROR_CHECK(err_code);
    }    
}


void eg_fds_read_data(uint16_t fid, uint16_t key) {
    ret_code_t err_code;
    fds_find_token_t tok   = {0};
    fds_record_desc_t desc = {0};

    if (fds_record_find(fid, key, &desc, &tok) == FDS_SUCCESS) {
        fds_flash_record_t frec = {0};
	uint32_t *data;

        err_code = fds_record_open(&desc, &frec);
	APP_ERROR_CHECK(err_code);

	// little endian by word (4byte) 0x01020304 --> 0x04030201
	NRF_LOG_DEBUG("DATA:");
	data = (uint32_t *) frec.p_data;
	for (uint8_t ii=0; ii<frec.p_header->length_words; ii++) {
	    NRF_LOG_DEBUG("\t0x%08X",data[ii]);
	}
	
	err_code = fds_record_close(&desc);
        APP_ERROR_CHECK(err_code);
    }
}

    
void eg_fds_write_data(uint16_t fid, uint16_t key, void const * p_data, uint32_t len) {    
    fds_record_t const rec = {
        .file_id           = fid,
        .key               = key,
        .data.p_data       = p_data,
        .data.length_words = BYTES_TO_WORDS(len)    //(len + 3) / sizeof(uint32_t)
    };
    
    fds_write_flag = 0;
    ret_code_t err_code = fds_record_write(NULL, &rec);
    APP_ERROR_CHECK(err_code);
}


void eg_fds_update_data(uint16_t fid, uint16_t key, void const * p_data, uint32_t len) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};
	    
    if (fds_record_find(fid, key, &desc, &ftok) == FDS_SUCCESS) {
        fds_record_t const rec = {
            .file_id           = fid,
            .key               = key,
            .data.p_data       = p_data,
            .data.length_words = BYTES_TO_WORDS(len)	//(len + 3) / sizeof(uint32_t)
        };
        err_code = fds_record_update(&desc, &rec);
	APP_ERROR_CHECK(err_code);
    }
}


void eg_fds_delete_record(uint16_t file_id, uint16_t rec_key) {
    ret_code_t err_code;
    fds_find_token_t tok   = {0};
    fds_record_desc_t desc = {0};
	    
    if (fds_record_find(file_id, rec_key, &desc, &tok) == FDS_SUCCESS) {
        err_code = fds_record_delete(&desc);
	APP_ERROR_CHECK(err_code);
    }
}


ret_code_t eg_fds_record_exists(uint16_t fid, uint16_t key) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};
    
    return fds_record_find(fid, key, &desc, &ftok);
}


void eg_fds_update_user_data_string(uint16_t fid, uint16_t key, uint8_t *g_var, uint8_t *new_val, uint8_t len) {
    // Copy new value to global var
    memcpy(g_var, new_val, len);
    
    // Check if record exists  
    if (eg_fds_record_exists(fid, key) == FDS_SUCCESS) {
	// Format uint8_t data into uint32_t data chunk (5 words)
	uint32_t temp;
	static uint32_t data[CHUNK_LENGTH_BLE_PACKET];
	memset(data, 0, sizeof(data));
	
	// Swap endianness
	for (uint8_t ii=0; ii<len; ii++) {
	    if (ii%4==0) {
		temp = new_val[ii] << 24;
	    } else if (ii%4==1) {
		temp = new_val[ii] << 16;
	    } else if (ii%4==2) {
		temp = new_val[ii] << 8;
	    } else {
		temp = new_val[ii];
	    }
	    data[ii/4] |= temp;
	}
	
	for (uint8_t ii=0; ii<CHUNK_LENGTH_BLE_PACKET; ii++) {
	    NRF_LOG_DEBUG("\tFORMAT UPDATE DATA[%d]: 0x%08X", ii, data[ii]);
	}
	
	// Update the record
	eg_fds_update_data(fid, key, (uint32_t *) data, BYTE_LENGTH_BLE_PACKET);
    }
}


ret_code_t eg_fds_fetch_user_data_byte(uint16_t fid, uint16_t key, uint8_t *var, uint32_t default_val) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};

    err_code = fds_record_find(fid, key, &desc, &ftok);
    if (err_code == FDS_ERR_NOT_FOUND) {
	*var = default_val;
	uint32_t data = *var;
	
	eg_fds_write_data(fid, key, &data, BYTES_PER_WORD);
//	while (fds_write_flag == 0);
	while (!fds_write_flag) {
	    (void) sd_app_evt_wait();
	}
	return NRF_SUCCESS;
    } else if (err_code == FDS_SUCCESS) {
	err_code = eg_fds_check_default_user_data_byte(fid, key, var, default_val);
	return err_code;
    }
}


ret_code_t eg_fds_fetch_user_data_word(uint16_t fid, uint16_t key, uint32_t *var, uint32_t default_val) {
    ret_code_t err_code = eg_fds_record_exists(fid, key);
    if (err_code == FDS_ERR_NOT_FOUND) {
	*var = default_val;
	uint32_t data = *var;
	eg_fds_write_data(fid, key, &data, BYTES_PER_WORD);
//	while (fds_write_flag == 0);
	while (!fds_write_flag) {
	    (void) sd_app_evt_wait();
	}	
    } else if (err_code == FDS_SUCCESS) {
	err_code = eg_fds_check_default_user_data_word(fid, key, var, default_val);
	if (err_code != NRF_SUCCESS) {
	    NRF_LOG_ERROR(" --- APP_ERROR_CHECK for FETCHWORD eg_fds_check_default_user_data_word(FILE %d, KEY %d): 0x%X", fid, key, err_code);
	    return err_code;
	}
    }

    return NRF_SUCCESS;
}


ret_code_t eg_fds_fetch_data_string(uint16_t fid, uint16_t key, uint8_t *var, uint8_t *default_val, uint8_t len) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};
    fds_flash_record_t	flash_record;
    
    err_code = fds_record_find(fid, key, &desc, &ftok);
    if (err_code == FDS_ERR_NOT_FOUND) {
	// Set global var to default val
	memcpy(var, default_val, len);
	
	// Format device name var into flash words, valid char value is 0x20 - 0x7E
	uint32_t temp;
	uint32_t write_data[CHUNK_LENGTH_BLE_PACKET] = { 0 };
	for(uint8_t ii=0; ii<len; ii++) {
	    if (ii%4==0) {
		temp = default_val[ii] << 24;
	    } else if (ii%4==1) {
		temp = default_val[ii] << 16;
	    } else if (ii%4==2) {
		temp = default_val[ii] << 8;
	    } else {
		temp = default_val[ii];
	    }
	    write_data[ii/4] |= temp;
	}
	NRF_LOG_DEBUG("\tDEV_NAME_DATA[0]: 0x%08X", write_data[0]);
	
	// Write record to flash
	eg_fds_write_data(fid, key, write_data, BYTE_LENGTH_BLE_PACKET);
//	while (fds_write_flag == 0);
	while (!fds_write_flag) {
	    (void) sd_app_evt_wait();
	}	
    } else if (err_code == FDS_SUCCESS) {
//	err_code = eg_fds_check_default_user_data_string(fid, key, var, default_val);
	err_code = fds_record_open(&desc, &flash_record);
	APP_ERROR_CHECK(err_code);
	    
#if defined(LOG_VERBOSE)
	NRF_LOG_DEBUG("Found Record ID = %d", desc.record_id);
#endif
	    
	// Format default_val into flash words for comparison
//	memset(var, 0, sizeof(uint8_t)*MAX_BLE_PACKET_LENGTH);	// clear old data
	uint32_t *data = (uint32_t *) flash_record.p_data;
	for (uint8_t ii=0; ii<flash_record.p_header->length_words; ii++) {
#if defined(LOG_VERBOSE)
	    NRF_LOG_DEBUG("\t\t0x%08X", data[ii]);
#endif
	    var[(ii*4)] = (data[ii] >> 24) & 0xFF;
	    var[(ii*4)+1] = (data[ii] >> 16) & 0xFF;
	    var[(ii*4)+2] = (data[ii] >> 8) & 0xFF;
	    var[(ii*4)+3] = data[ii] & 0xFF;
	}

	// Close the record when done
	err_code = fds_record_close(&desc);
	APP_ERROR_CHECK(err_code);
    } else {
	return err_code;
    }
    
    return NRF_SUCCESS;
}


ret_code_t eg_fds_check_default_user_data_byte(uint16_t fid, uint16_t key, uint8_t *var, uint32_t default_val) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};
    fds_flash_record_t  flash_record;
    uint32_t *data;
    
    // Assumes all records have unique file ID and key
    err_code = fds_record_find(fid, key, &desc, &ftok);
    APP_ERROR_CHECK(err_code);
    
    err_code = fds_record_open(&desc, &flash_record);
    APP_ERROR_CHECK(err_code);

    // Access the record through the flash_record structure
//    NRF_LOG_DEBUG("Fetching Record ID = %d", desc.record_id);
    data = (uint32_t *) flash_record.p_data;
    for (uint8_t i=0; i<flash_record.p_header->length_words; i++) {
//	NRF_LOG_DEBUG("\tData = 0x%08X", data[i]);

	// If record doesn't match default val, overwrite global with record val
	if (data[i] != default_val) {
	    *var = data[i];
#if defined(LOG_VERBOSE)
	    NRF_LOG_DEBUG("Fetching Record ID = %d. Non-default global, flash=0x%08X", desc.record_id, data[i]);
#endif
	} else {
	    *var = default_val;
#if defined(LOG_VERBOSE)
	    NRF_LOG_DEBUG("Fetching Record ID = %d. flash=0x%08X == default=0x%08X", desc.record_id, data[i], default_val);
#endif
	}
    }

    // Close the record
    err_code = fds_record_close(&desc);
    APP_ERROR_CHECK(err_code);
    
    return NRF_SUCCESS;
}
    

ret_code_t eg_fds_check_default_user_data_word(uint16_t fid, uint16_t key, uint32_t *var, uint32_t default_val) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};
    fds_flash_record_t  flash_record;
    uint32_t *data;
    
    // Assumes all records have unique file ID and key, and new record isn't created
    err_code = fds_record_find(fid, key, &desc, &ftok);
    APP_ERROR_CHECK(err_code);

    err_code = fds_record_open(&desc, &flash_record);
    APP_ERROR_CHECK(err_code);

    // Access the record through the flash_record structure
//    NRF_LOG_DEBUG("Fetching Record ID = %d", desc.record_id);
    data = (uint32_t *) flash_record.p_data;
    for (uint8_t i=0; i<flash_record.p_header->length_words; i++) {
//	NRF_LOG_DEBUG("\tData = 0x%08X", data[i]);

	// If record doesn't match default val, overwrite global with record val
	if (data[i] != default_val) {
	    *var = data[i];
#if defined(LOG_VERBOSE)
	    NRF_LOG_DEBUG("Fetching Record ID = %d. Overwriting global with flash_val=0x%08X", desc.record_id, data[i]);
#endif
	} else {
	    *var = default_val;
#if defined(LOG_VERBOSE)
	    NRF_LOG_DEBUG("Fetching Record ID = %d. flash_val=0x%08X == default_val=0x%08X", desc.record_id, data[i], default_val);
#endif
	}
    }

    // Close the record when done
    err_code = fds_record_close(&desc);
    APP_ERROR_CHECK(err_code);
    
    return NRF_SUCCESS;
}


ret_code_t eg_fds_update_user_data_byte(uint16_t fid, uint16_t key, uint8_t *var, uint32_t new_val) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};
    
    // Check if record exists
    if (eg_fds_record_exists(fid, key) == FDS_SUCCESS) {
	// Check if data differs from new_val
	
	// Update the global
	*var = new_val;
	static uint32_t data;
	data = new_val;
	
//	NRF_LOG_DEBUG("\t data(0x%08x) = %d", (uint32_t) &data, data);	
	
	// Update the record
	eg_fds_update_data(fid, key, &data, BYTES_PER_WORD);
//	NRF_LOG_DEBUG("Updating %d words to Record ID: %d ", record_chunk.length_words, record_desc.record_id);		
    }

    return NRF_SUCCESS;
}


ret_code_t eg_fds_update_user_data_word(uint16_t fid, uint16_t key, uint32_t *var, uint32_t new_val) {
    ret_code_t err_code;
    fds_record_desc_t desc = {0};
    fds_find_token_t  ftok = {0};
    
    // Check if record exists
    if (eg_fds_record_exists(fid, key) == FDS_SUCCESS) {
	// Check if data differs from new_val
	
	// Update the global
	*var = new_val;
	static uint32_t data;
	data = new_val;
	
//    NRF_LOG_DEBUG("\t data(0x%08x) = %d", (uint32_t) &data, data);	
	
	// Update the record
	eg_fds_update_data(fid, key, &data, BYTES_PER_WORD);
//    NRF_LOG_DEBUG("Updating %d words to Record ID: %d ", record_chunk.length_words, record_desc.record_id);		
    }

    return NRF_SUCCESS;
}

ret_code_t eg_fds_fetch_user_prefs(void) {
    ret_code_t err_code;
    err_code = eg_fds_fetch_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_R, &g_led_r, DEFAULT_LED_R);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_G, &g_led_g, DEFAULT_LED_G);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_B, &g_led_b, DEFAULT_LED_B);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_LED_PATTERN, &g_led_pattern_sel, DEFAULT_LED_PATTERN);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_user_data_word(FILE_ID_USER_PREFS, REC_KEY_CHAR_DURATION_MS, &g_char_duration_ms, DEFAULT_CHAR_DURATION_MS);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_user_data_word(FILE_ID_USER_PREFS, REC_KEY_INTER_CHAR_DURATION_MS, &g_inter_char_duration_ms, DEFAULT_INTER_CHAR_DURATION_MS);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_user_data_word(FILE_ID_USER_PREFS, REC_KEY_SPACE_DURATION_MS, &g_space_duration_ms, DEFAULT_SPACE_DURATION_MS);
    APP_ERROR_CHECK(err_code);
   
    err_code = eg_fds_fetch_user_data_byte(FILE_ID_USER_PREFS, REC_KEY_TX_POWER, (uint8_t *) &g_tx_power_level, DEFAULT_TX_POWER);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_user_data_byte(FILE_ID_PB_STATE, REC_KEY_DEVICE_NAME_LEN, &g_device_name_len, DEFAULT_DEVICE_NAME_LEN);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_data_string(FILE_ID_PB_STATE, REC_KEY_DEVICE_NAME, g_device_name, (uint8_t *) DEFAULT_DEVICE_NAME, g_device_name_len);
    APP_ERROR_CHECK(err_code);
    err_code = eg_fds_fetch_data_string(FILE_ID_MAINTAIN_DATA, REC_KEY_SERIAL_NUM, g_serial_num, (uint8_t *) DEFAULT_SERIAL_NUM, DEFAULT_SERIAL_NUM_LEN);
    APP_ERROR_CHECK(err_code);
//    g_serial_num[DEFAULT_SERIAL_NUM_LEN] = '\0';	// add null terminated string
    
    return NRF_SUCCESS;
}


void eg_fds_gc(void) {
    ret_code_t err_code = fds_gc();
    APP_ERROR_CHECK(err_code);
}


void eg_fds_free_words(void) {   
    fds_stat_t stat = {0};
    ret_code_t err_code = fds_stat(&stat);
    APP_ERROR_CHECK(err_code);

    if (stat.largest_contig < GC_CONTIG_THRESHOLD) {
	NRF_LOG_WARNING("Need to run garbage collection [stat.largest_contig: %d < %d]", stat.largest_contig, GC_CONTIG_THRESHOLD);
	eg_fds_gc();
    }
}


void eg_fds_stat(void) {
    fds_stat_t stat = {0};
    ret_code_t err_code = fds_stat(&stat);
    APP_ERROR_CHECK(err_code);
    
    NRF_LOG_INFO("\n--- FDS STAT\r\n\
	corruption:\t%s\r\n\
	total pages:\t%u\r\n\
	total records:\t%u\r\n\
	valid records:\t%u\r\n\
	dirty records:\t%u\r\n\
	open records:\t%u",
	stat.corruption ? "true" : "false",
	stat.pages_available,
	stat.valid_records + stat.dirty_records,
	stat.valid_records,
	stat.dirty_records,
	stat.open_records);
    NRF_LOG_INFO("\n\
	words reserved:\t%u\r\n\
	words used:\t%u\r\n\
	largest contig:\t%u\r\n\
	freeable words:\t%u (%u bytes)\r\n",
	stat.words_reserved,
	stat.words_used,
	stat.largest_contig,
	stat.freeable_words,
	stat.freeable_words * BYTES_PER_WORD);
}


void eg_fds_delete_files(void) {
    ret_code_t err_code;
    
    // Delete user prefs
    err_code = fds_file_delete(FILE_ID_USER_PREFS);
    APP_ERROR_CHECK(err_code);
    
    err_code = fds_file_delete(FILE_ID_PB_STATE);
    APP_ERROR_CHECK(err_code);
}
