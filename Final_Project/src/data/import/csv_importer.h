/**
 * @file data/import/csv_importer.h
 * @brief CSV file importer for saved event log records
 *
 * Reads CSV files previously exported by CsvExporter and rebuilds
 * an EventRecord array. Separated from the exporter following SRP.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef CSV_IMPORTER_H
#define CSV_IMPORTER_H

#include "core/types/event_record.h"

/**
 * @brief Loads event records from a CSV file
 * @param filename  Path to the CSV file to read
 * @param events    Output: heap-allocated EventRecord array; caller must free each + array
 * @return Number of records loaded, or 0 on failure
 * @note The first line (header row) is automatically skipped
 */
int CsvImporter_Load(const char *filename, EventRecord **events);

#endif /* CSV_IMPORTER_H */
