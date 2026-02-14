#ifndef TEST_REPOSITORY_H
#define TEST_REPOSITORY_H

#include <cassert>
#include <iostream>
#include <memory>
#include "note_repository.h"
#include "json_storage_strategy.h"

class TestRepository {
private:
    // Допоміжний метод для створення репозиторію
    static std::unique_ptr<NoteRepository> createRepo() {
        return std::make_unique<NoteRepository>(std::make_unique<JsonStorageStrategy>());
    }

    static void test_repository_crud() {
        std::cout << "  - Testing CRUD operations... ";
        auto repo = createRepo();

        Schema s1("Схема A");
        repo->addSchema(s1);
        Note n1("Нотатка 1", 0);
        repo->addNote(n1);

        assert(repo->getSchemas().size() > 0);
        assert(repo->getNotes().size() == 1);

        Note n2("Нова назва", 0);
        repo->updateNote(0, n2);
        assert(repo->getNotes()[0].getTitle() == "Нова назва");

        Schema s_upd("Схема Update");
        repo->updateSchema(0, s_upd);
        assert(repo->getSchemas()[0].getName() == "Схема Update");

        repo->removeNote(0);
        repo->removeSchema(0);
        assert(repo->getNotes().isEmpty());

        std::cout << "Passed!\n";
    }

    static void test_mutable_notes() {
        std::cout << "  - Testing mutable access... ";
        auto repo = createRepo();
        Note n1("Temp Note", 0);
        repo->addNote(n1);

        repo->getNotes()[0].setTitle("Changed via reference");
        assert(repo->getNotes()[0].getTitle() == "Changed via reference");
        std::cout << "Passed!\n";
    }

    static void test_repository_init_logic() {
        std::cout << "  - Testing Repository default data logic... ";
        auto repo = createRepo();
        repo->load("non_existent.json");

        assert(repo->getSchemas().size() == 1);
        assert(repo->getSchemas()[0].getName() == "Книга");
        std::cout << "Passed!\n";
    }

    static void test_note_sorting() {
        std::cout << "  - Testing Note sorting logic... ";
        auto repo = createRepo();
        int schemaId = 0;

        Note n_old("AAA Oldest", schemaId);
        n_old.setCreationDate(QDateTime(QDate(2023, 10, 1), QTime(10, 0, 0)));

        Note n_pinned("CCC Pinned", schemaId);
        n_pinned.setPinned(true);

        Note n_new("BBB Newest", schemaId);
        n_new.setCreationDate(QDateTime(QDate(2023, 10, 2), QTime(10, 0, 0)));

        repo->addNote(n_old);
        repo->addNote(n_pinned);
        repo->addNote(n_new);

        repo->sortNotes(SortType::ByNameAZ);
        assert(repo->getNotes()[0].isPinned() == true);
        assert(repo->getNotes()[0].getTitle() == "CCC Pinned");

        repo->sortNotes(SortType::ByDateNewest);
        assert(repo->getNotes()[1].getTitle() == "BBB Newest");
        std::cout << "Passed!\n";
    }

public:
    static void runTests() {
        std::cout << "--- Running NoteRepository Tests ---\n";
        test_repository_crud();
        test_mutable_notes();
        test_repository_init_logic();
        test_note_sorting();
        std::cout << "--- NoteRepository Tests Passed ---\n";
    }
};

#endif