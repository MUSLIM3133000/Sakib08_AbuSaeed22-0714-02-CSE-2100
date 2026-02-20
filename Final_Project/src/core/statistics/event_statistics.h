/**
 * @file core/statistics/event_statistics.h
 * @brief Event statistics calculation interface
 *
 * Provides aggregated event counts by severity level by directly
 * calling Windows Event Log functions.
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#ifndef EVENT_STATISTICS_H
#define EVENT_STATISTICS_H

#include "core/types/event_record.h"

/**
 * @brief Computes event statistics from Windows Event Log
 * @param log_name  Log identifier (wide-string channel name)
 * @param hours_back  Hours of history to analyse; 0 = all events
 * @return Populated EventStatistics (all fields zero on failure)
 */
EventStatistics EventStatistics_Calculate(const wchar_t *log_name, int hours_back);

#endif /* EVENT_STATISTICS_H */
