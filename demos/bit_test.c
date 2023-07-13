#include <stdio.h>

#pragma pack(1)

typedef struct {
    union {
        struct {
            unsigned char x : 8;
            unsigned char y : 8;
            unsigned char pal : 4;
            unsigned char xmirror : 1;
            unsigned char ymirror : 1;
            unsigned char rotate : 1;
            unsigned char x8 : 1;
            unsigned char visible: 1;
            unsigned char enable5: 1;
            unsigned char pattern: 6;
        } bitFields;
        
        struct {
            unsigned char r1;
            unsigned char r2;
            unsigned char r3;
            unsigned char r4;
        } byteFields;

        struct {
            unsigned short int16_1;
            unsigned short int16_2;
        };
        
        unsigned int int32;
    };
} MyStruct;

int main() {
    MyStruct myData;

    // Assign values to the struct members
    __asm 
    ; x field
    __endasm

    myData.bitFields.x = 50;
    
    __asm 
    ; y field
    __endasm
    
    myData.bitFields.y = 100;
    
    __asm 
    ; pal field
    __endasm
    
    myData.bitFields.pal = 3;
    
    __asm 
    ; xmirror field
    __endasm
    
    myData.bitFields.xmirror = 1;
    
    __asm 
    ; ymirror field
    __endasm
    
    myData.bitFields.ymirror = 0;
    
    __asm 
    ; rotate field
    __endasm
    
    myData.bitFields.rotate = 1;
    
    __asm 
    ; x8 field
    __endasm
    
    myData.bitFields.x8 = 0;

    __asm 
    ; visible field
    __endasm
    
    myData.bitFields.visible = 1;
    
    __asm 
    ; enable5 field
    __endasm
    
    myData.bitFields.enable5 = 0;

    __asm 
    ; pattern field
    __endasm
    
    myData.bitFields.pattern = 6;

    unsigned int fullX = (myData.bitFields.x8 << 8) | myData.bitFields.x;
    printf("x (9-bit value): %u\n", fullX);
    printf("y: %u\n", myData.bitFields.y);
    printf("pal: %u\n", myData.bitFields.pal);
    printf("xmirror: %u\n", myData.bitFields.xmirror);
    printf("ymirror: %u\n", myData.bitFields.ymirror);
    printf("rotate: %u\n", myData.bitFields.rotate);
    printf("x8: %u\n", myData.bitFields.x8);

    printf("visible: %u\n", myData.bitFields.visible);
    printf("enable5: %u\n", myData.bitFields.enable5);
    printf("pattern: %u\n", myData.bitFields.pattern);



    printf("R1: %u\n", myData.byteFields.r1);
    printf("R2: %u\n", myData.byteFields.r2);
    printf("R3: %u\n", myData.byteFields.r3);
    printf("R4: %u\n", myData.byteFields.r4);

    // Accessing the 16-bit integers
    myData.int16_1 = 1234;
    myData.int16_2 = 5678;
    printf("int16_1: %hu\n", myData.int16_1);
    printf("int16_2: %hu\n", myData.int16_2);

    // Accessing the 32-bit integer
    myData.int32 = 987654321;
    printf("int32: %d\n", myData.int32);

    return 0;
}
