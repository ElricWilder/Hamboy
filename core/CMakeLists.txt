add_library(core
    src/cpu.cpp
    src/utils.cpp
    src/opcodes.cpp 
    src/bus.cpp 
    src/cart.cpp 
    src/ppu.cpp 
    src/tile.cpp
    src/modes.cpp
    src/io.cpp  
    src/sprite.cpp
    src/timer.cpp
    src/rtc.cpp 
    src/logger.cpp)

target_include_directories(core PUBLIC include)