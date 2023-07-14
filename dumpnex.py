"""
NEX File Parser

This script parses a NEX file and extracts the relevant information from its header and memory banks. The NEX file is a binary format used for storing data related to the ZX Spectrum Next computer.

The script defines the structure of the NEX file format using the Construct library. It reads the NEX file, parses its contents, and provides access to the extracted data.

Usage:
    1. Ensure the Construct library is installed (pip install construct).
    2. Update the "filename" variable with the path to your NEX file.
    3. Run the script to parse the NEX file.
    4. Extracted information is available in the "parsed_file" object.

The script currently extracts the file header information, including the number of banks to load, loading screen flags, border color, stack pointer, program counter, and the list of memory banks included in the file. Additionally, it separates the memory banks into individual arrays of 16,384 bytes each.

Due to the way construct works, it is also possible to modify the contents of the NEXT file and export the modified file.

Note: This script assumes a specific structure for the NEX file format. Make sure your NEX files conform to this structure.

Author: MikeDX (https://github.com/MikeDX) assisted by ChatGPT (https://chat.openai.com/)


The NEX file format is used for storing various graphics and memory bank data for the ZX Spectrum Next computer. This header provides essential information about the file structure and contents.

Header Structure:

- "Next" string: A 4-byte identifier indicating the file type.

- Version: A 4-byte string specifying the NEX file version (e.g., "V1.0", "V1.1", "V1.2", "V1.3").

- RAM Required: Indicates the amount of RAM required by the program (0 = 768KB, 1 = 1792KB).

- Number of 16K Banks to Load: Specifies the number of 16KB banks to load (0-112). The actual banks to load are defined in the "Banks Included" array.

- Loading-Screen Blocks: A set of bit-flags specifying the presence of loading screens for different graphics modes, such as Layer2, ULA, Hi-Res, Lo-Res, and more.

- Border Color: The border color index (0-7) for the graphics screens.

- Stack Pointer: The initial stack pointer value.

- Program Counter: The initial program counter value (0 = don't run, just load).

- Number of Extra Files: Currently unused by any known software.

- Banks Included: An array of 112 bytes indicating which 16KB memory banks are included in the file. A value of 1 at a specific index indicates that the corresponding bank is included.

- Layer2 Loading Bar: Specifies whether the Layer2 loading bar is enabled (0 = OFF, 1 = ON).

- Loading Bar Color: The color index (0-255) for the loading bar (for specific graphics modes).

- Loading Delay per Bank: The amount of delay (in frames) between loading each bank.

- Start Delay: The initial delay (in frames) before starting the loading process.

- Preserve Current Next-Registers Values: Specifies whether to preserve the current Next-Registers values (0 = reset machine state, 1 = preserve).

- Required Core Version: Specifies the required core version for the NEX file to run.

- Timex HiRes 512x192 Mode Color: The color index for Timex HiRes 512x192 mode or the palette offset for Layer 2 320x256x8bpp or 640x256x4bpp modes.

- Entry Bank: The bank to be mapped into slot 3 (0xC000-0xFFFF address space) after loading. Used for setting the program counter and file handle address.

- File Handle Address: The file handle address. Determines how the NEX loader handles the file after loading is complete.

- Extensions (V1.3 and beyond): Additional fields included in V1.3 and later versions of the NEX format.

- Memory Banks: Raw memory bank data stored in 16KB chunks. The number of banks included depends on the "Number of 16K Banks to Load" field.

Note: The NEX file format is extensible, and additional fields may be present in specific versions or for custom usage.


"""


from construct import *

# Define the structure of the file header for v1.3
file_header_v1_3 = Struct(
    "disable_expansion_bus" / Byte,
    "has_checksum" / Byte,
    "first_bank_offset" / Int32ul,
    "cli_buffer_address" / Int16ul,
    "cli_buffer_size" / Int16ul,
    "loading_screen_flags2" / IfThenElse(lambda ctx: ctx._.version == b"V1.3", Byte, Pass),
    "has_copper_code" / Byte,
    "tilemode_screen_config" / Bytes(4),
    "loading_bar_y_position" / Byte,
    "reserved" / Bytes(349),
    "crc" / Int32ul,
)

# Define the structure of the file header
file_header = Struct(
    "magic" / Const(b"Next"),
    "version" / Enum(Bytes(4),
        V1_0=b"V1.0",
        V1_1=b"V1.1",
        V1_2=b"V1.2",
        V1_3=b"V1.3"
    ),
    "ram_required" / Enum(Byte,
        ram_768k=0,
        ram_1792k=1,
    ),                          
    "num_banks_to_load" / Byte,
    "loading_screen_flags" / Byte,
    "border_color" / Byte,
    "stack_pointer" / Int16ul,
    "program_counter" / Int16ul,
    "num_extra_files" / Int16ul,
    "banks_included" / Bytes(112),
    "layer2_loading_bar" / Byte,
    "loading_bar_color" / Byte,
    "loading_delay_per_bank" / Byte,
    "start_delay" / Byte,
    "preserve_registers" / Byte,
    "required_core_version" / Bytes(3),
    "timex_hires_color_palette_offset" / Byte,
    "entry_bank" / Byte,
    "file_handle_address" / Int16ul,
    If(lambda ctx: ctx.version == b"V1.3",
        file_header_v1_3
    )
)

# Define the structure of the entire file
file_format = Struct(
    "header" / file_header,
    "palette" / IfThenElse(lambda ctx: ctx.header.loading_screen_flags & 128 != 0, Array(512, Byte), Pass),
    "layer2_screen" / IfThenElse(lambda ctx: ctx.header.loading_screen_flags & 1 != 0, Array(49152, Byte), Pass),
    "ula_screen" / IfThenElse(lambda ctx: ctx.header.loading_screen_flags & 2 != 0, Array(6912, Byte), Pass),
    "lores_screen" / IfThenElse(lambda ctx: ctx.header.loading_screen_flags & 4 != 0, Array(12288, Byte), Pass),
    "timex_hires_screen" / IfThenElse(lambda ctx: ctx.header.loading_screen_flags & 8 != 0, Array(12288, Byte), Pass),
    "timex_hicol_screen" / IfThenElse(lambda ctx: ctx.header.loading_screen_flags & 16 != 0, Array(12288, Byte), Pass),
    "layer2_320x256_screen" / IfThenElse(lambda ctx: ctx.header.version == b"V1.3" and ctx.header.loading_screen_flags2 & 1 != 0, Array(81920, Byte), Pass),
    "copper_code" / IfThenElse(lambda ctx: ctx.header.version == b"V1.3" and ctx.header.has_copper_code == 1, Array(2048, Byte), Pass),
    "memory_banks_count" / Computed(lambda ctx: sum(ctx.header.banks_included)),
    "memory_banks" / Array(
    len_(lambda ctx: [i for i, bank in enumerate(ctx.header.banks_included) if bank]),  # Calculate the number of non-zero banks included
    Struct(
        "bank_data" / Bytes(16384),
    )
    ),
    # "memory_banks" / Array(lambda ctx: ctx.memory_banks_count, Array(16384, Byte)),
    # "memory_banks" / Bytes(lambda ctx: ctx.memory_banks_count),
    "custom_data" / GreedyBytes,
)

# Read and parse the file
def parse_file(filename):
    print(f"Parsing nex: {filename}")
    with open(filename, "rb") as f:
        data = f.read()
        try:
            parsed_data = file_format.parse(data)
            return parsed_data
        except StreamError as e:
            print(f"Error parsing file: {e}")
            return None

# Example usage
parsed_file = parse_file("bin/zxn_sprite_1.nex")
if parsed_file:
    print(parsed_file)

parsed_file = parse_file("build/zxn_sprite_1.nex")
if parsed_file:
    print(parsed_file)

#parsed_file = parse_file("bin/trammath.nex")
#if parsed_file:
#    print(parsed_file)
