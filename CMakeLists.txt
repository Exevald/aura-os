cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)
project(aura-os C CXX ASM)

SET(TARGET_ARCHITECTURE i686-elf)

SET(BUILD_DIR ${CMAKE_BINARY_DIR}/bin)
FILE(MAKE_DIRECTORY ${BUILD_DIR})

SET(NASM_EXECUTABLE "/usr/local/bin/nasm")
add_custom_command(
        OUTPUT ${BUILD_DIR}/boot.bin
        COMMAND ${NASM_EXECUTABLE} -f bin ${CMAKE_SOURCE_DIR}/bootloader/src/boot.asm -o ${BUILD_DIR}/boot.bin
        DEPENDS ${CMAKE_SOURCE_DIR}/bootloader/src/boot.asm
        COMMENT "Assembling bootloader (boot.bin)"
)
add_custom_target(bootloader ALL DEPENDS ${BUILD_DIR}/boot.bin)

SET(KERNEL_ENTRY_OBJECT ${BUILD_DIR}/kernel_entry.o)
add_custom_command(
        OUTPUT ${KERNEL_ENTRY_OBJECT}
        COMMAND ${NASM_EXECUTABLE} -f elf ${CMAKE_SOURCE_DIR}/bootloader/src/kernel.asm -o ${KERNEL_ENTRY_OBJECT}
        DEPENDS ${CMAKE_SOURCE_DIR}/bootloader/src/kernel.asm
        COMMENT "Assembling kernel entry (${KERNEL_ENTRY_OBJECT})"
)

SET(PAGING_ASM_OBJ ${BUILD_DIR}/paging_helpers.o)
add_custom_command(
        OUTPUT ${PAGING_ASM_OBJ}
        COMMAND nasm -f elf ${CMAKE_SOURCE_DIR}/lib/mm/paging_helpers.asm -o ${PAGING_ASM_OBJ}
        DEPENDS ${CMAKE_SOURCE_DIR}/lib/mm/paging_helpers.asm
)

SET(INTERRUPTS_ASM_OBJ ${BUILD_DIR}/interrupts.o)
add_custom_command(
        OUTPUT ${INTERRUPTS_ASM_OBJ}
        COMMAND nasm -f elf ${CMAKE_SOURCE_DIR}/lib/cpu/interrupts.asm -o ${INTERRUPTS_ASM_OBJ}
        DEPENDS ${CMAKE_SOURCE_DIR}/lib/cpu/interrupts.asm
)

add_compile_options(-m32 -ffreestanding -fno-stack-protector -fno-exceptions -fno-rtti -O2)

add_executable(hello_cpp_elf code/main.cpp)

target_compile_options(hello_cpp_elf PRIVATE
        -m32
        -fno-pie
        -fno-pic
        -ffreestanding
        -fno-stack-protector
)

target_link_options(hello_cpp_elf PRIVATE
        "-m32"
        "-T${CMAKE_SOURCE_DIR}/code/linker.ld"
        "-nostdlib"
        "-static"
        "-no-pie"
        "-Wl,-z,max-page-size=0x1000"
)

set_target_properties(hello_cpp_elf PROPERTIES
        RUNTIME_OUTPUT_NAME "hello.elf"
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}
)

add_executable(aura_kernel_elf
        ${KERNEL_ENTRY_OBJECT}
        ${PAGING_ASM_OBJ}
        ${INTERRUPTS_ASM_OBJ}
        kernel/kernel.cpp
        lib/utils/cxx_abi.cpp
        lib/console/console.cpp
        lib/console/shell.cpp
        lib/mm/pmm.cpp
        lib/mm/paging.cpp
        lib/mm/wsclock.cpp
        lib/cpu/idt.cpp
        lib/cpu/interrupt_manager.cpp
        lib/fs/fs.cpp
        lib/commands/touch.cpp
        lib/commands/cat.cpp
        lib/commands/grep.cpp
        lib/commands/ls.cpp
        lib/commands/echo.cpp
        lib/commands/run.cpp
        lib/history/history.cpp
        lib/cpu/syscall_handler.cpp
        drivers/pic.cpp
        drivers/keyboard.cpp
        apps/almanac.cpp
        drivers/ata_driver.cpp
        lib/utils/utils.cpp
        lib/commands/clear.cpp
        lib/mm/mm.cpp
        lib/mm/heap.cpp
)

target_link_options(aura_kernel_elf PRIVATE
        "-m32"
        "-T${CMAKE_SOURCE_DIR}/bootloader/linker.ld"
        "-nostdlib"
        "-Wl,--gc-sections"
)

set_target_properties(aura_kernel_elf PROPERTIES
        RUNTIME_OUTPUT_NAME "kernel.elf"
        RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}
)

add_custom_command(
        OUTPUT ${BUILD_DIR}/kernel.bin
        COMMAND ${CMAKE_OBJCOPY} -O binary ${BUILD_DIR}/kernel.elf ${BUILD_DIR}/kernel.bin
        DEPENDS aura_kernel_elf
        COMMENT "Converting kernel.elf to raw binary kernel.bin"
)
add_custom_target(kernel_raw ALL DEPENDS ${BUILD_DIR}/kernel.bin)

add_custom_command(
        OUTPUT ${BUILD_DIR}/os.img
        COMMAND dd if=/dev/zero of=${BUILD_DIR}/os.img bs=512 count=20480
        COMMAND dd if=${BUILD_DIR}/boot.bin of=${BUILD_DIR}/os.img bs=512 count=1 conv=notrunc
        COMMAND dd if=${BUILD_DIR}/kernel.bin of=${BUILD_DIR}/os.img bs=512 seek=1 conv=notrunc
        COMMAND dd if=${BUILD_DIR}/hello.elf of=${BUILD_DIR}/os.img bs=512 seek=300 conv=notrunc

        DEPENDS bootloader kernel_raw hello_cpp_elf
        COMMENT "Rebuilding disk image..."
)
add_custom_target(disk_image ALL DEPENDS ${BUILD_DIR}/os.img)

find_program(QEMU_EXECUTABLE NAMES qemu-system-i386 qemu-system-x86_64 REQUIRED)

set(QEMU_DRIVE_CMD
        ${QEMU_EXECUTABLE} -drive file=${BUILD_DIR}/os.img,format=raw,index=0,media=disk
)

add_custom_target(run_aura
        COMMAND ${QEMU_DRIVE_CMD}
        DEPENDS disk_image
        COMMENT "Running Aura OS in QEMU"
)

add_custom_target(run_aura_debug
        COMMAND ${QEMU_DRIVE_CMD} -S -monitor stdio
        DEPENDS disk_image
        COMMENT "Running Aura OS in QEMU with registers monitoring"
)