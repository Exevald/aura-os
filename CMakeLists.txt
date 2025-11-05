cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)
project(aura-os C ASM)

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
add_custom_target(kernel_asm_entry ALL DEPENDS ${KERNEL_ENTRY_OBJECT})

add_executable(aura_kernel_elf
        kernel/kernel.c
        ${KERNEL_ENTRY_OBJECT}
)

target_link_options(aura_kernel_elf PRIVATE
        "-T${CMAKE_SOURCE_DIR}/bootloader/linker.ld"
        "-nostdlib"
        "-Wl,--gc-sections"
        "-Wl,--strip-debug"
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
        COMMAND dd if=/dev/zero of=${BUILD_DIR}/os.img bs=1024 count=1440
        COMMAND dd if=${BUILD_DIR}/boot.bin of=${BUILD_DIR}/os.img conv=notrunc
        COMMAND dd if=${BUILD_DIR}/kernel.bin of=${BUILD_DIR}/os.img seek=1 conv=notrunc
        DEPENDS bootloader kernel_raw
        COMMENT "Creating final disk image (os.img)"
)
add_custom_target(disk_image ALL DEPENDS ${BUILD_DIR}/os.img)

find_program(QEMU_EXECUTABLE NAMES qemu-system-i386 qemu-system-x86_64 REQUIRED)

add_custom_target(qemu
        COMMAND ${QEMU_EXECUTABLE} -drive file=${BUILD_DIR}/os.img,format=raw,if=floppy,index=0 -boot order=a
        DEPENDS disk_image
        COMMENT "Running Aura OS in QEMU (1.44 MB floppy)"
)

add_custom_target(qemu_nographic
        COMMAND ${QEMU_EXECUTABLE} -fda ${BUILD_DIR}/os.img -nographic -boot a
        DEPENDS disk_image
        COMMENT "Running Aura OS in QEMU (text-only, no graphics)"
)

add_custom_target(qemu_debug
        COMMAND ${QEMU_EXECUTABLE} -fda ${BUILD_DIR}/os.img -s -S -boot a
        DEPENDS disk_image
        COMMENT "Running Aura OS in QEMU with GDB stub (-s -S)"
)

add_custom_target(qemu_monitor
        COMMAND ${QEMU_EXECUTABLE} -fda ${BUILD_DIR}/os.img -S -monitor stdio -boot a
        DEPENDS disk_image
        COMMENT "Running Aura OS in QEMU with monitor and paused CPU"
)