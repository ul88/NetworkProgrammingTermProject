cmake_minimum_required(VERSION 3.16)

project(vending_machine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appvending_machine
    main.cpp
)

qt_add_qml_module(appvending_machine
    URI vending_machine
    QML_FILES
        QML/Main.qml
        QML/BeverageArea.qml
        QML/PromotionBlock.qml
        QML/InputMoneyText.qml
        QML/InputPaperMoney.qml
        QML/InputCoin.qml
        QML/BeverageOutputBlock.qml
        QML/HaveMoneyArea.qml
        QML/OutputMoneyArea.qml
        RESOURCES GarbageImage.qrc
        QML_FILES QML/MoneyDialog.qml
        SOURCES vendingmachine.h vendingmachine.cpp
        SOURCES beverage.h beverage.cpp
        SOURCES linkedlist.h
        SOURCES consumer.h consumer.cpp
        SOURCES money.h money.cpp
        SOURCES manager.h manager.cpp
        SOURCES ErrorHandler.h
        SOURCES Csv.h
        SOURCES FileUtil.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appvending_machine PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appvending_machine
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appvending_machine
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appvending_machine
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
