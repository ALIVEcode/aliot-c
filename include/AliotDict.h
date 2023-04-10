#ifndef ALIOT_DICT_H
#define ALIOT_DICT_H

#include <ArduinoJson.h>

namespace Dict {

/*
The purpose of the following code is to create a similar data structure to hashmaps (or dictionaries, as seen in the python version of aliot). 
This allows the user to determine key/value pairs in a Json object by directly initializing the pairs in the 
parameters of the createDict function. 
To format json objects correctly, we first deserialize the pairs in createDict(),
serialize it into a string, pass the string in another method, deserialize the string again to change it,
and so on. This allows more flexibility in the value types and number of key/value pairs.
*/

/**
 * NOTE : If it turns out that the above method is too slow or takes too much memory,
 * we can go back to using overloaded methods for updateDoc and sendEvent to handle json related stuff.
 * NOTE 2 : I don't recommend using the std::array versions of createDict. Opt for the regular ones to avoid
 * stack overflow errors.
*/

// TODO: get rid of strings
typedef String AliotDict_t;

/*
T : Type of the second element of the pair (value in key/)
*/
template<typename T>
struct Pair {
    const char* key;
    T value;

    Pair(const char* key, T value) {
        this->key = key; 
        this->value = value;
    }
};

/*
Pair struct that can handle arrays
T : Type of the second element (value) of the key/value pair
N : Number of elements in the json array
*/
template<typename T, size_t N>
struct Pair<T[N]> {
    const char* key;
    T value[N];

    Pair(const char* key, T value[N]) {
        this->key = key; 
        for (int i = 0; i < N; i++) 
            this->value[i] = value[i];
    }
    // Allows to create an array of pairs with brace initialization
    Pair(const char* key, std::array<T, N> value) {
        this->key = key; 
        for (int i = 0; i < N; i++) 
            this->value[i] = value[i];
    }
};

/*
Create a key/value pair and return a string representation of the JSON object.
Generic type T refers to the value type in the key/value pair.
*/
template<typename T> AliotDict_t createDict(Pair<T> pair) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO: get rid of strings

    doc[pair.key] = pair.value;
    serializeJson(doc, data);
    return data;
};

/*
Creates a list of key/array pairs and returns a string representation of the Json Array.
Generic type T refers to the value type in the array. size_t N refers to  
All values in list of arrays must be of same type T. 
All arrays in list must have the same number of elements N.
*/
template<typename T, size_t N> AliotDict_t createDict(Pair<T[N]> arrayPair) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO: get rid of strings

    JsonArray arrayHolder = doc.createNestedArray(arrayPair.key);
    for (int i = 0; i < N; i++)
        arrayHolder.add(arrayPair.value[i]);

    serializeJson(doc, data);
    return data;
}

// ========= WARNING : STD::ARRAY VERSIONS OF createDict ARE NOT RECOMMENDED ==============
// Don't use too much, causes a stack overflow sometimes because of the dynamic initialization.
// I think it's fine for rare use internally, but don't go overboard with calls in loop() yet.
// Just opt for multiple calls to the regular versions of createDict for multiple doc updates
// =========================================================================================

/*
Create a list of key/value pairs and return a string representation of the JSON object.
Generic type T refers to the value type in the key/value pair. All values in list must be of same type.
size_t N refers to the number of key/value pairs in the list.
*/
template<typename T, size_t N> AliotDict_t createDict(std::array<Pair<T>, N> pairList) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO: get rid of strings

    // Map each key/value pair in the json document
    for (int i = 0; i < N; i++) 
        doc[pairList[i].key] = pairList[i].value;

    serializeJson(doc, data);
    return data;
};

/*
Creates a list of key/array pairs and returns a string representation of the Json Array.
Generic type T refers to the value type in the array. size_t N refers to  
All values in list of arrays must be of same type T. 
All arrays in list must have the same number of elements N.
*/
template<typename T, size_t N, size_t P> AliotDict_t createDict(std::array<Pair<T[N]>, P> pairArrayList) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO : get rid of strings

    // Map each key/array pair in the json document
    for (int i = 0; i < pairArrayList.size(); i++) {
        JsonArray arrayHolder = doc.createNestedArray(pairArrayList[i].key);

        // Add each element in the array
        for (int j = 0; j < N; j++) { 
            arrayHolder.add(pairArrayList[i].value[j]);
        }
    }
    serializeJson(doc, data);
    return data;
};

}

#endif /* ALIOT_DICT_H */