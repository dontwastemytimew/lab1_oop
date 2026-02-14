#ifndef TEST_IO_H
#define TEST_IO_H

#include <cassert>
#include <iostream>
#include <QFile>
#include "note_repository.h"
#include "json_storage_strategy.h"
#include "export_service.h"

class TestIO {
private:
    static void test_save_and_load_data() {
        std::cout << "  - Testing Strategy Save/Load... ";
        const QString testPath = "test_data_temp.json";

        auto repo_save = std::make_unique<NoteRepository>(std::make_unique<JsonStorageStrategy>());
        repo_save->addNote(Note("Test Note", 0));
        repo_save->save(testPath);

        auto repo_load = std::make_unique<NoteRepository>(std::make_unique<JsonStorageStrategy>());
        repo_load->load(testPath);

        assert(repo_load->getNotes().size() == 1);
        assert(repo_load->getNotes()[0].getTitle() == "Test Note");

        QFile::remove(testPath);
        std::cout << "Passed!\n";
    }

    static void test_export_service() {
        std::cout << "  - Testing ExportService (JSON & PDF)... ";
        const QString jsonPath = "test_export.json";
        const QString pdfPath = "test_export.pdf";

        Note n("Service Test", 0);

        // Тест JSON
        assert(ExportService::exportToJson(n, jsonPath));
        assert(QFile::exists(jsonPath));

        // Тест PDF
        assert(ExportService::exportToPdf(n, pdfPath));
        assert(QFile::exists(pdfPath));

        QFile::remove(jsonPath);
        QFile::remove(pdfPath);
        std::cout << "Passed!\n";
    }

public:
    static void runTests() {
        std::cout << "--- Running I/O & Service Tests ---\n";
        test_save_and_load_data();
        test_export_service();
        std::cout << "--- I/O & Service Tests Passed ---\n";
    }
};

#endif