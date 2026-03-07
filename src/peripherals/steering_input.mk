ifndef STEERING_INPUT_MK
define STEERING_INPUT_MK
1
endef

# REVIEW(Barach): Nothing wrong with using a makefile include for this, although you don't really need it as you have the .c
#   file already added to the CSRC list in the project's root makefile.

# Add the module's source file to the compilation
CSRC += /src/peripherals/steering_input.c

endif # STEERING_INPUT_MK