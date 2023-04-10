#ifndef UTILITY_H
#define UTILITY_H

void getRandomStrF(char* output, int len) {
  char* eligible_chars = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890";
  memset(output, 0, len);
  for (int i = 0; i < len - 1; i++) {
    uint8_t random_index = random(0, strlen(eligible_chars));
    output[i] = eligible_chars[random_index];
  }
  output[len - 1] = '\0';
}

template<typename T>
void printArrayList(ArrayList<T> &list) {
  for (int i = 0; i < list.size(); i++) {
    Serial.print("[" + String(i) + "]: ");
    Serial.println(list[i]);
  }
  Serial.println();
}

int separate (String str, char **p, int size, char* sep) {
  int  n;
  char s [100];

  strcpy (s, str.c_str ());

  *p++ = strtok (s, sep);
  for (n = 1; NULL != (*p++ = strtok (NULL, sep)); n++) {
    if (size == n) {
      break;
    }
  }

  return n;
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#endif
