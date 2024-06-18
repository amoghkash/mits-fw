#pragma once


void initialize_spiffs(void);
void deinitialize_spiffs(void);
void create_file(const char *filename, const char *data);
void r_file(const char* filename);
void add_data_to_file(const char *filename, const char *data);
