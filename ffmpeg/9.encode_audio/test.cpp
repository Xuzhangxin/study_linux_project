// #include <stdio.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>

// int main(int argc, char **argv)
// {
//     FILE *file = fopen("48000_2_f32le.pcm", "rb");
//     if (!file)
//     {
//         return -1;
//     }



//     uint8_t *buf = (uint8_t *)malloc(8192);

//     // memset(buf, 0, 8192);


//     size_t len  = fread(buf, 1, 8192, file);
//     if (len <= 0)
//     {
//         perror("err");
//         return -1;
//     }


//     printf("len = %d", len);
// }