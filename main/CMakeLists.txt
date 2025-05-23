idf_component_register(SRC_DIRS "." "system" "system/commands" "drivers"
                    INCLUDE_DIRS ".")
                    
nvs_create_partition_image(nvs
                    "../nvs_init.csv" FLASH_IN_PROJECT)