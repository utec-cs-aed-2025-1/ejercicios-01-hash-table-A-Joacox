#include <vector>

using namespace std;

const int maxColision = 3;
const float maxFillFactor = 0.8;

template<typename TK, typename TV>
struct ChainHashNode {
    TK key;
	TV value;
	size_t hashcode;
	ChainHashNode* next;

	ChainHashNode(TK k, TV v, size_t h) : key(k), value(v), hashcode(h), next(nullptr) {}
};

template<typename TK, typename TV>
class ChainHashListIterator {
 	typedef ChainHashNode<TK, TV> Node;
	Node* current;
public:
	ChainHashListIterator(Node* current) : current(current) {};

	ChainHashListIterator& operator++() {
		this->current = this->current->next;
		return *this;
	}

	bool operator!=(const ChainHashListIterator& other) {
		return this->current != other.current;
	}

	Node& operator*() {
		return *this->current;
	}

	Node* operator->() {
		return this->current;
	}
};

template<typename TK, typename TV>
class ChainHash
{
private:    
    typedef ChainHashNode<TK, TV> Node;
    typedef ChainHashListIterator<TK, TV> Iterator;

	Node** array;  // array de punteros a Node
    int nsize; // total de elementos <key:value> insertados
	int capacity; // tamanio del array
	int *bucket_sizes; // guarda la cantidad de elementos en cada bucket
	int usedBuckets; // cantidad de buckets ocupados (con al menos un elemento)

public:
    ChainHash(int initialCapacity = 10){
		this->capacity = initialCapacity; 
		this->array = new Node*[capacity]();  
		this->bucket_sizes = new int[capacity]();
		this->nsize = 0;
		this->usedBuckets = 0;
	}

	TV get(TK key){
		size_t hashcode = getHashCode(key);
		size_t index = hashcode % capacity;
		
		Node* current = this->array[index];
		while(current != nullptr){
			if(current->key == key) return current->value;
			current = current->next;
		}
		throw std::out_of_range("Key no encontrado");
	}
	
	int size(){ return this->nsize; }	

	int bucket_count(){ return this->capacity; }

	int bucket_size(int index) { 
		if(index < 0 || index >= this->capacity) throw std::out_of_range("Indice de bucket invalido");
		return this->bucket_sizes[index]; 
	}	
	
	// TODO: implementar los siguientes métodos
	void set(TK key, TV value){
		if (fillFactor() > maxFillFactor) rehashing();

		size_t hashcode = getHashCode(key);
		int index = hashcode % this->capacity;

		// Verificar si la clave ya existe y actualizar su valor
		Node* current = this->array[index];
		while (current != nullptr) {
			if (current->key == key) {
				current->value = value;
				return;
			}
			current = current->next;
		}

		//Caso contrario, push front
		Node* newNode = new Node(key, value, hashcode);
		newNode->next = this->array[index];
		this->array[index] = newNode;

		//Actualizar contenedores
		this->nsize++;
		this->bucket_sizes[index]++;
		if (this->bucket_sizes[index] == 1) 
			this->usedBuckets++;

		if (this->bucket_sizes[index] > maxColision) 
			rehashing();
	};
	
	bool remove(TK key){
    size_t hashcode = getHashCode(key);
    int index = hashcode % this->capacity;

    Node* current = this->array[index];
    if (current == nullptr) return false; // Bucket vacio

    // Caso primer nodo
    if (current->key == key) {
        this->array[index] = current->next;
        delete current;
        this->nsize--;
        this->bucket_sizes[index]--;
        if (this->array[index] == nullptr) this->usedBuckets--;
        return true;
    }

    // Caso otros nodos
    Node* prev = current;
    current = current->next;
    while (current != nullptr) {
        if (current->key == key) {
            prev->next = current->next;
            delete current;
            this->nsize--;
            this->bucket_sizes[index]--;

            // verificar con bucket_sizes si quedo vacio
            if (this->bucket_sizes[index] == 0) this->usedBuckets--;

            return true;
        }
        prev = current;
        current = current->next;
    }

    return false; // clave no encontrada
	}


	bool contains(TK key) {
		size_t hashcode = getHashCode(key);
		size_t index = hashcode % capacity;
		
		Node* current = this->array[index];
		while(current != nullptr){
			if(current->key == key) return true;
			current = current->next;
		}
		return false;
	};
		
	Iterator begin(int index){
		return Iterator(this->array[index]);
	}
	Iterator end(int index){
		return Iterator(nullptr);
	}

private:
	double fillFactor(){
		return (double)this->usedBuckets / (double)this->capacity;
	}	

	size_t getHashCode(TK key){
		std::hash<TK> ptr_hash;
		return ptr_hash(key);
	}

	//TODO: implementar rehashing
	void rehashing() {
		int oldCapacity = this->capacity;
		this->capacity = this->capacity * 2 + 1;

		// Guardar punteros antiguos
		Node** oldArray = this->array;
		int* oldBucketSizes = this->bucket_sizes;

		// crear nuevas estructuras;
		this->array = new Node*[this->capacity]();
		this->bucket_sizes = new int[this->capacity]();
		this->nsize = 0;
		this->usedBuckets = 0;

		// Reinsertar elementos
		for (int i = 0; i < oldCapacity; i++) {
			Node* current = oldArray[i];
			while (current != nullptr) {
				// TODO: arreglar reinsertar elementos sin disparar otro rehash
				
				/* Version antigua
				this->set(current->key, current->value);
				Node* tbd = current;
				current = current->next;
				delete tbd;
				*/

				// Nueva version sin usar set()
				size_t newIndex = current->hashcode % this->capacity;
				Node* newNode = new Node(current->key, current->value, current->hashcode);
				newNode->next = this->array[newIndex];
				this->array[newIndex] = newNode;

				this->nsize++;
				this->bucket_sizes[newIndex]++;
				if (this->bucket_sizes[newIndex] == 1) 
					this->usedBuckets++;

				Node* tbd = current;
				current = current->next;
				delete tbd;	
			}
		}
		
		delete[] oldArray;
		delete[] oldBucketSizes;
	};

public:
	// TODO: implementar destructor
	~ChainHash(){
		for (int i = 0; i < this->capacity; i++) {
			Node* current = this->array[i];
			while (current != nullptr) {
				Node* tbd = current;
				current = current->next;
				delete tbd;
			}
		}
		delete[] this->array;
		delete[] this->bucket_sizes;
	};
};
