add_executable(lab_project.elf cardJitsuDisplay.c main.c cardJitsuControl.c)
target_link_libraries(lab_project.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(lab_project.elf PROPERTIES LINKER_LANGUAGE CXX)