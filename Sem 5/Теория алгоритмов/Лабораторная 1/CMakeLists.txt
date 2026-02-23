cmake_minimum_required(VERSION 3.16)

project(MyQtApp VERSION 1.0 LANGUAGES CXX)

# Указываем, что используем C++17 (или выше)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к Qt (если не в PATH)
set(CMAKE_PREFIX_PATH "C:/Qt/6.9/6.9.3/msvc2022_64")

# Находим Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Создаём исполняемый файл
add_executable(MyQtApp main.cpp)

# Подключаем Qt6
target_link_libraries(MyQtApp PRIVATE Qt6::Core Qt6::Widgets)