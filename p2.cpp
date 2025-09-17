#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "chainhash.h"

using namespace std;

// convertir string a minusculas (por uniformidad)
string toLowerCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// descomponer un string en palabras
vector<string> tokenize(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;
    
    while (ss >> word) {
        // Remover signos de puntuacion (por si acaso)
        word.erase(remove_if(word.begin(), word.end(), 
                   [](char c) { return !isalnum(c); }), word.end());
        
        if (!word.empty()) {
            words.push_back(toLowerCase(word));
        }
    }
    return words;
}

// TODO: Implementar el algoritmo Bag of Words
ChainHash<string, vector<int>> bagOfWords(const vector<string>& documentos) {
    ChainHash<string, vector<int>> result(13);
    
    // cada doc
    for (int docIndex = 0; docIndex < documentos.size(); docIndex++) {
        // se tokeniza el doc
        vector<string> words = tokenize(documentos[docIndex]);
        
        // por cada palabra
        for (const string& word : words) {
            // verifica si la palabra ya existe en la tabla hash
            if (result.contains(word)) {
                // si existe, obtener el vector actual
                vector<int> docList = result.get(word);
                
                // verificar si el documento actual ya está en la lista
                // en el caso una palabra se repita en el mismo doc
                if (find(docList.begin(), docList.end(), docIndex) == docList.end()) {
                    docList.push_back(docIndex);
                }
                
                // se actualiza el vector en la tabla hash
                result.set(word, docList);
            } else {
                // si no existe, crear un nuevo vector con el indice del documento actual
                vector<int> docList;
                docList.push_back(docIndex);
                result.set(word, docList);
            }
        }
    }
    
    return result;
}

// Función para mostrar los resultados
void printBagOfWords(ChainHash<string, vector<int>>& bow) {
    cout << "{\n";
    
    // Recorrer todos los buckets para mostrar las palabras
    for (int i = 0; i < bow.bucket_count(); i++) {
        if (bow.bucket_size(i) > 0) {
            for (auto it = bow.begin(i); it != bow.end(i); ++it) {
                cout << " \"" << (*it).key << "\": [";
                
                const vector<int>& docs = (*it).value;
                for (size_t j = 0; j < docs.size(); j++) {
                    cout << docs[j];
                    if (j < docs.size() - 1) cout << ", ";
                }
                cout << "],\n";
            }
        }
    }
    cout << "}\n";
}

int main() {
    vector<string> documentos = {
        "La casa es grande",
        "El gato está en la casa", 
        "La casa es bonita y grande",
        "El sol brilla sobre la casa"
    };
    
    cout << "Documentos de entrada:\n";
    for (size_t i = 0; i < documentos.size(); i++) {
        cout << "[" << i << "] \"" << documentos[i] << "\"\n";
    }
    cout << "\n";
    
    ChainHash<string, vector<int>> bow = bagOfWords(documentos);
    cout << "Resultado de Bag of Words (palabra -> [documentos]):\n";
    printBagOfWords(bow);
    
    return 0;
}
