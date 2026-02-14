#include "statistics_service.h"

QMap<QString, int> StatisticsService::getNotesCountByCategory(NoteRepository* repo) {
    QMap<QString, int> counts;
    const auto& schemas = repo->getSchemas();
    const auto& notes = repo->getNotes();

    for(const auto& schema : schemas) counts[schema.getName()] = 0;

    for(const auto& note : notes) {
        if(note.getSchemaId() < schemas.size()) {
            counts[schemas[note.getSchemaId()].getName()]++;
        }
    }
    return counts;
}

QMap<QString, int> StatisticsService::getUserActivity(NoteRepository* repo) {
    return repo->getUsageStats();
}