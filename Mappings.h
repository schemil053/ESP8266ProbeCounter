
#ifndef MAPPINGS_H
#define MAPPINGS_H

template<typename hash, typename map>
class HashType {
  public:
    HashType() {
      
    }

    HashType(hash code, map value): hashCode(code), mappedValue(value) {}


    hash getKey() {
      return hashCode;
    }
    void setKey(hash code) {
      hashCode = code;
    }
    map getValue() {
      return mappedValue;
    }
    void setValue(map value) {
      mappedValue = value;
    }

    HashType& operator()(hash key, map value) {
      setKey(key);
      setValue(value);
    }
  private:
    hash hashCode;
    map mappedValue;
};

template<typename hash, typename map>
class HashMap {
  public:
    HashMap() {

    }

    HashType<hash, map>& operator[](int x) {
      return hashMap[x];
    }

    byte getIndexOf(hash key ) {
      for (byte i = 0; i < hashMap.size(); i++) {
        if (hashMap[i].getKey() == key) {
          return i;
        }
      }
    }
    map get(hash key) {
      for (int i = 0; i < hashMap.size(); i++) {
        if (hashMap[i].getKey() == key) {
          return hashMap[i].getValue();
        }
      }
      return 0;
    }

    void clear() {
      hashMap.clear();
    }

    void put(hash key, map object) {
      boolean setted = true;
      for (int i = 0; i < hashMap.size(); i++) {
        if (hashMap[i].getKey() == key) {
          hashMap[i].setValue(object);
          setted = false;
        }
      }
      if(setted) {
        HashType<hash, map> cur;
        cur.setKey(key);
        cur.setValue(object);
        hashMap.add(cur);
      }
    }


  private:
    ArrayList<HashType<hash, map>> hashMap;
};

#endif
