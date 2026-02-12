#ifndef TEST_DATAMANAGER_H
#define TEST_DATAMANAGER_H

#include <cassert>
#include <iostream>
#include "datamanager.h"

class TestDataManager {
private:
    static void test_manager_crud() {
        std::cout << "  - Testing CRUD operations... ";
        DataManager dm;

        Schema s1("Схема A");
        dm.addSchema(s1);
        Note n1("Нотатка 1", 0);
        dm.addNote(n1);

        assert(dm.getSchemas().size() > 0);
        assert(dm.getNotes().size() == 1);

        Note n2("Нова назва", 0);
        dm.updateNote(0, n2);
        assert(dm.getNotes()[0].getTitle() == "Нова назва");

        Schema s_upd("Схема Update");
        dm.updateSchema(0, s_upd);
        assert(dm.getSchemas()[0].getName() == "Схема Update");

        dm.removeNote(0);
        dm.removeSchema(0);
        assert(dm.getNotes().isEmpty());

        std::cout << "Passed!\n";
    }

    static void test_mutable_notes() {
        std::cout << "  - Testing mutable access to Notes... ";
        DataManager dm;
        Note n1("Temp Note", 0);
        dm.addNote(n1);

        dm.getNotes()[0].setTitle("Змінено через посилання");

        assert(dm.getNotes()[0].getTitle() == "Змінено через посилання");
        std::cout << "Passed!\n";
    }

    static void test_manager_constructor() {
        std::cout << "  - Testing DataManager constructor default schema... ";
        DataManager dm;
        assert(dm.getSchemas().size() == 1);
        assert(dm.getSchemas()[0].getName() == "Книга");
        assert(dm.getSchemas()[0].getFields().size() == 2);
        std::cout << "Passed!\n";
    }

    static void test_invalid_indices() {
        std::cout << "  - Testing invalid index handling... ";
        DataManager dm;
        Note n("Test", 0);
        dm.addNote(n);

        dm.removeNote(-1);
        dm.removeNote(99);

        dm.updateNote(-1, n);
        dm.updateNote(99, n);

        assert(dm.getNotes().size() == 1);

        std::cout << "Passed!\n";
    }

    static void test_note_sorting() {
        std::cout << "  - Testing Note sorting logic... ";
        DataManager dm;

        int schemaId = 0;

        Note n_old("AAA Oldest", schemaId);
        n_old.setCreationDate(QDateTime(QDate(2023, 10, 1), QTime(10, 0, 0)));

        Note n_pinned("CCC Pinned", schemaId);
        n_pinned.setCreationDate(QDateTime(QDate(2023, 10, 3), QTime(10, 0, 0)));
        n_pinned.setPinned(true); // Закріплена

        Note n_new("BBB Newest", schemaId);
        n_new.setCreationDate(QDateTime(QDate(2023, 10, 2), QTime(10, 0, 0)));

        dm.addNote(n_old);
        dm.addNote(n_pinned);
        dm.addNote(n_new);

        dm.sortNotes(SortType::ByNameAZ);

        assert(dm.getNotes()[0].isPinned() == true);

        assert(dm.getNotes()[0].getTitle() == "CCC Pinned");

        dm.sortNotes(SortType::ByDateNewest);

        assert(dm.getNotes()[0].getTitle() == "CCC Pinned");

        assert(dm.getNotes()[1].getTitle() == "BBB Newest");
        assert(dm.getNotes()[2].getTitle() == "AAA Oldest");

        dm.sortNotes(SortType::ByNameAZ);

        assert(dm.getNotes()[0].getTitle() == "CCC Pinned");

        assert(dm.getNotes()[1].getTitle() == "AAA Oldest");
        assert(dm.getNotes()[2].getTitle() == "BBB Newest");

        std::cout << "Passed!\n";
    }

    static void test_usage_statistics() {
        std::cout << "  - Testing usage statistics tracking... ";
        DataManager dm;

        QMap<QString, int> initialStats = dm.getUsageStats();

        int secondsToday = 120;
        dm.addUsageTime(secondsToday);

        QMap<QString, int> updatedStats = dm.getUsageStats();
        QString today = QDateTime::currentDateTime().toString("yyyy-MM-dd");

        assert(updatedStats.contains(today));

        assert(updatedStats.value(today) >= initialStats.value(today, 0) + secondsToday);

        int secondsMore = 60;
        dm.addUsageTime(secondsMore);

        QMap<QString, int> finalStats = dm.getUsageStats();

        assert(finalStats.value(today) >= initialStats.value(today, 0) + secondsToday + secondsMore);

        std::cout << "Passed!\n";
    }

public:
    static void runTests() {
        std::cout << "--- Running DataManager Tests ---\n";
        test_manager_crud();
        test_mutable_notes();
        test_manager_constructor();
        test_invalid_indices();
        test_note_sorting();
        test_usage_statistics();
        std::cout << "--- DataManager Tests Passed ---\n";
    }
};

#endif //TEST_DATAMANAGER_H
