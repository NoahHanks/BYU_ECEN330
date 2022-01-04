add_executable(lab6.elf main.c bhTester.c simonDisplay.c globals.c buttonHandler.c flashSequence.c fsTester.c verifySequence.c vsTester.c simonControl.c)
target_link_libraries(lab6.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(lab6.elf PROPERTIES LINKER_LANGUAGE CXX)