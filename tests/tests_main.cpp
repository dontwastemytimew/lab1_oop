#include "test_models.h"
#include "test_datamanager.h"
#include "test_io.h"
#include <iostream>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    std::cout << "\n========================================\n";
    std::cout << "     Running ALL Lab Unit Tests\n";
    std::cout << "========================================\n\n";

    TestModels::runTests();
    TestDataManager::runTests();
    TestIO::runTests();

    std::cout << "\n========================================\n";
    std::cout << "  ALL TESTS PASSED SUCCESSFULLY!\n";
    std::cout << "========================================\n";

    return 0;
}