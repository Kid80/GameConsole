import math

HEIGHT = 1
WIDTH = 1
BYTE_WIDTH = math.ceil(WIDTH/8.0)
BYTE_SIZE = BYTE_WIDTH * HEIGHT

'''Makes c file with the name font{HEIGHT}.c into an empty font'''
def initBlank():
    file = open(f"/home/ben/pico/Pico-ResTouch-LCD-X_X_Code/c/lib/font/font{HEIGHT}.c", "w")

    file.write(f"// HEIGHT:     {HEIGHT}\n")
    file.write(f"// WIDTH:      {WIDTH}\n")
    file.write(f"// BYTE_WIDTH: {BYTE_WIDTH}\n")
    file.write(f"// BYTE_SIZE:  {BYTE_SIZE}\n")

    file.write("#include \"fonts.h\"\n\n")

    file.write(f"const uint8_t Font{HEIGHT}_Table [] = \n")
    file.write("{\n")

    for i in range(94):
        file.write("\n")
        file.write(f"\t// @{i * BYTE_SIZE} ' ' ({WIDTH} pixels wide)\n")
        for y in range(HEIGHT):
            file.write("\t")
            for x in range(BYTE_WIDTH):
                file.write("0x00, ")
            file.write("\n")
    file.write("};\n\n")

    file.write(f"sFONT Font{HEIGHT} = ")
    file.write("{\n")
    file.write(f"\tFont{HEIGHT}_Table,\n")
    file.write(f"\t{WIDTH}, /* Width */\n")
    file.write(f"\t{HEIGHT}, /* Height */\n")
    file.write("};")

    file.close()

initBlank()