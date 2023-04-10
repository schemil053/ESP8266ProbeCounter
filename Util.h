#ifndef UTILITY_H
#define UTILITY_H

void getRandomStrF(char* output, int len){
    char* eligible_chars = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890";
    memset(output, 0, len);
    for(int i = 0; i < len - 1; i++){
        uint8_t random_index = random(0, strlen(eligible_chars));
        output[i] = eligible_chars[random_index];
    }
    output[len - 1] = '\0';
}

#endif
