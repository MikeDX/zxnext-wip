from PIL import Image

# Convert BMP to SPR
def convert_bmp_to_spr(image_path, output_path):
    image = Image.open(image_path).convert("RGB")
    width, height = image.size

    # Create a custom palette for the SPR image
    palette = []
    for pixel in image.getdata():
        if pixel not in palette:
            palette.append(pixel)
        if len(palette) == 256:
            break

    # Write the SPR file
    with open(output_path, "wb") as spr_file:
        # Write the SPR header
        spr_file.write(bytes([0] * 4))  # Placeholder for palette offset
        spr_file.write(bytes([width, height]))  # Width and height
        spr_file.write(bytes([0] * 4))  # Placeholder for attribute offset

        # Write the palette
        for rgb in palette:
            spr_file.write(bytes(rgb))

        # Write the pixel data
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel((x, y))
                spr_file.write(bytes([palette.index(pixel)]))

        # Calculate and write the attribute offset
        attribute_offset = 8 + len(palette) * 3 + width * height
        attribute_offset %= 256  # Keep the value within the valid range
        spr_file.seek(4)
        spr_file.write(bytes([attribute_offset]))




# Convert BMP to NXP
def convert_bmp_to_nxp(image_path, output_path):
    image = Image.open(image_path).convert("RGB")
    width, height = image.size

    # Create a new NXP file
    with open(output_path, "wb") as nxp_file:
        # Write the NXP header
        nxp_file.write(b"NxPI")
        nxp_file.write(bytes([0]))  # Version

        # Write the image dimensions
        nxp_file.write(bytes([width, height]))

        # Write the pixel data
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel((x, y))
                nxp_file.write(bytes([pixel[0], pixel[1], pixel[2]]))


# Example usage
bmp_image_path = "jack.bmp"
spr_output_path = "output.spr"
nxp_output_path = "output.nxp"

convert_bmp_to_spr(bmp_image_path, spr_output_path)
convert_bmp_to_nxp(bmp_image_path, nxp_output_path)
