from os.path import join, dirname

def relative(path):
    return join(dirname(__file__), path)

def Settings(**kwargs):
    flags = [
        "-x", "c++",
        "-isystem", "/usr/lib/gcc/avr/10.1.0/include",
        "-isystem", "/usr/lib/gcc/avr/10.1.0/include-fixed",
        "-isystem", "/usr/lib/avr/include",
        "-I", relative("avr"),
        "-I", relative("."),
        "-DMCU=atmega328p",
        "-DF_CPU=16000000UL",
        "-DBAUD=9600UL",
        "-D__AVR_ATmega328P__",
        "-D__AVR_DEVICE_NAME__=atmega328p"
    ]
    return {
        "flags": flags
    }
