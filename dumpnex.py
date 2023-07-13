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
    with open(filename, "rb") as f:
        data = f.read()
        try:
            parsed_data = file_format.parse(data)
            return parsed_data
        except StreamError as e:
            print(f"Error parsing file: {e}")
            return None

# Example usage
#parsed_file = parse_file("bin/zxn_sprite_1.nex")
#if parsed_file:
#    print(parsed_file)

parsed_file = parse_file("build/zxn_sprite_1.nex")
if parsed_file:
    print(parsed_file)

#parsed_file = parse_file("bin/trammath.nex")
#if parsed_file:
#    print(parsed_file)
