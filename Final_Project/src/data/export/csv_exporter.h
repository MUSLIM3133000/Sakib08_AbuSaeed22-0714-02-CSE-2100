/**
 * @file data/export/csv_exporter.h
 * @brief CSV export functionality for event log records
 *
 * Direct CSV export implementation without strategy pattern abstraction.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include "core/types/event_record.h"

/**
 * @brief Writes events to a CSV file with header row
 * @param filename  Destination file path
 * @param events    Array of EventRecord
 * @param count     Number of records in array
 * @return 1 on success, 0 if the file could not be opened
 */
int CsvExporter_Export(const char *filename, EventRecord *events, int count);

#endif /* CSV_EXPORTER_H */
