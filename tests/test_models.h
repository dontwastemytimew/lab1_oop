#ifndef TEST_MODELS_H
#define TEST_MODELS_H

#include <cassert>
#include <iostream>
#include "note.h"
#include "schema.h"

class TestModels {
private:
    static void test_schema_basic() {
        std::cout << "  - Testing Schema creation... ";
        Schema s("Тест-Схема");
        assert(s.getName() == "Тест-Схема");
        assert(s.getFields().isEmpty());

        Field f1 = {"Автор", "Текст"};
        s.addField(f1);
        assert(s.getFields().size() == 1);
        std::cout << "Passed!\n";
    }

    static void test_note_basic_and_tags() {
        std::cout << "  - Testing Note tags and fields... ";
        Note n("Тест-Нотатка", 1);

        // Тест полів
        n.addField("Вага", "10 кг");
        assert(n.getFields().value("Вага") == "10 кг");

        // Тест тегів
        n.addTag("робота");
        n.addTag("важливо");
        n.addTag("робота");

        assert(n.getTags().size() == 2);
        assert(n.getTags().contains("важливо"));

        // Тест дати
        assert(n.getCreationDate().isValid());

        std::cout << "Passed!\n";
    }

    static void test_note_setters() {
        std::cout << "  - Testing Note setters (setTitle, setTags)... ";

        Note n("Початкова назва", 0);

        // Тестуємо setTitle()
        n.setTitle("Нова назва");
        assert(n.getTitle() == "Нова назва");

        // Тестуємо setTags()
        QSet<QString> newTags = {"тег1", "тег2"};
        n.setTags(newTags);
        assert(n.getTags().size() == 2);
        assert(n.getTags().contains("тег1"));
        assert(!n.getTags().contains("старий_тег"));

        std::cout << "Passed!\n";
    }

public:
    static void runTests() {
        std::cout << "--- Running Models Tests ---\n";
        test_schema_basic();
        test_note_basic_and_tags();
        test_note_setters();
        std::cout << "--- Models Tests Passed ---\n";
    }
};

#endif // TEST_MODELS_H