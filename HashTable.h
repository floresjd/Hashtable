#ifndef HASHTABLE_H
#define HASHTABLE_H

//#include <iostream>

template <typename KeyType, typename ValueType>
class HashTable
{
public:
	HashTable(unsigned int numBuckets, unsigned int capacity);
	~HashTable();
	bool isFull() const { return (m_usedCap == m_capacity || m_nBuckets == m_usedBuckets); }
	bool set(const KeyType& key, const ValueType& value, bool permanent = false);
	bool get(const KeyType& key, ValueType& value) const;
	bool touch(const KeyType& key);
	bool discard(KeyType& key, ValueType& value);
private:
	//to prevent HashTable from being copied or assigned
	//copy constructor and assignment operator private and no implemtation
	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);
	//end copy constructor an assignment operator

	struct DLL; struct Node;//incomplete declarations

	//private data members
	unsigned int m_nBuckets;
	unsigned int m_capacity;
	unsigned int m_usedBuckets;
	unsigned int m_usedCap;
	unsigned int m_permanent;
	Node** m_buckets;
	//end data members

	//private functions
	unsigned int determineBucketForKey(const KeyType& key) const;
	//end private functions

	//vvvvv  used for HashTable
	struct Node
	{
		Node(const KeyType& key, const ValueType& val)
			:m_key(key), m_value(val), m_next(nullptr), m_DLL(nullptr)
		{}
		KeyType m_key;
		ValueType m_value;
		Node* m_next;
		DLL* m_DLL;
	}; //end strcut Node

	//to keep track of precedence
	struct DLL
	{
		DLL(const KeyType& key, const ValueType& val)
			:m_key(key), m_value(val), m_next(nullptr), m_prev(nullptr)
		{	}
		KeyType m_key;
		ValueType m_value;
		DLL* m_next;
		DLL* m_prev;
	}; //end struct DLL

	//to be used with struct DLL
	//implemtations of a Doubly Linked List
	int m_size;
	DLL* m_head;
	DLL* m_tail;
	DLL* addNewDLLToFront(const KeyType& key, const ValueType& value);
	void moveDLLToFront(DLL* f);
	void deleteTailDLL();
}; //end class HashTable

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
	:m_nBuckets(numBuckets), m_capacity(capacity), m_usedBuckets(0), m_usedCap(0), m_permanent(0),
	m_size(0), m_head(nullptr), m_tail(nullptr)
{
	//if capacity is 0 we do nothing >>> nothing is set
	if (capacity == 0 || numBuckets == 0) return;

	//dynamic allocation of buckets
	//set each bucket to nullptr
	m_buckets = new Node*[numBuckets];
	for (unsigned int k = 0; k < m_nBuckets; ++k)
		m_buckets[k] = nullptr;
}//end constructor

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{

	//if capaity is 0 return >>> nothing was set
	if (m_capacity == 0 || m_nBuckets == 0)	return;

	//delete Linked List from HashTable
	for (unsigned int k = 0; k < m_nBuckets; ++k)
	{
		if (m_buckets[k] != nullptr)
		{
			//std::cout << k << " something in bucket\n";
			Node* p = m_buckets[k];
			while (p != nullptr)
			{
				Node* del = p;
				p = p->m_next;
				delete del;
				--m_usedCap;
			}
		}
	}

	//delete Doubly Linked List 
	while (m_head != nullptr)
	{
		DLL* temp = m_head;
		m_head = m_head->m_next;
		delete temp;
	}
	delete [] m_buckets;
	//std::cout << "destructor good\n";
}//end destructor

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent)
{
	//return false >>> nothing was set
	if(m_capacity == 0 || m_nBuckets == 0) return false;
	
	//determine a bucket number 
	unsigned int bucketNum = determineBucketForKey(key);
	//if the bucket is empty then add Node to bucket
	if (m_buckets[bucketNum] == nullptr)
	{
		Node* n = new Node(key, value);
		//if not permanent then add a new DLL to the Doubly Linked List that keeps track of precedence
		if (!permanent) n->m_DLL = addNewDLLToFront(key, value);
		m_buckets[bucketNum] = n;
		++m_usedCap;
		++m_usedBuckets;
		return true;
	}

	//else (check if key is already mapped)
	Node* curr = m_buckets[bucketNum];
	Node* prev = nullptr;
	while (curr != nullptr)
	{
		//if key is mapped and not permanent
		if (curr->m_key == key && curr->m_DLL != nullptr)
		{
			//update value in hashtable Node and Doubly Linked List
			curr->m_DLL->m_value = curr->m_value = value;
			moveDLLToFront(curr->m_DLL);
			return true;
		}
		//if key is mapped but permanent return false
		if (curr->m_key == key && curr->m_DLL == nullptr)
			return false;
		//else continue
		prev = curr;
		curr = curr->m_next;
	}
	//at this point key is not mapped
	//add a new Node
	//prev points to the last node in the bucket
	prev->m_next = new Node(key, value);
	//if not permanent then add a new DLL to the Doubly Linked List that keeps track of precedence
	if (!permanent) prev->m_next->m_DLL = addNewDLLToFront(key, value);
	++m_usedCap;
	++m_usedBuckets;
	return true;
}// end HashTable::set

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
	//return false >> nothing was set
	if (m_capacity == 0 || m_nBuckets == 0)	return false;
	
	//look for key
	unsigned int bn = determineBucketForKey(key);
	Node* p = m_buckets[bn];
	if (p != nullptr)
	{
		//search thorugh linked list until we find key
		//if did not find return false
		for (; p != nullptr; p = p->m_next)
			if (p->m_key == key)
			{
			value = p->m_value;
			return true;
			}
		return false;
	}
	//if buckect is empty return false
	return false;
} //end HashTable::get

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
	//return false >>> nothing was set
	if (m_capacity == 0 || m_nBuckets == 0) return false;

	unsigned int bn = determineBucketForKey(key);
	Node* curr = m_buckets[bn];

	//if bucket is not empty look for key
	while (curr != nullptr)
	{
		// if match and not permanernt move DLL to front
		if (curr->m_key == key && curr->m_DLL != nullptr)
		{
			moveDLLToFront(curr->m_DLL);
			return true;
		}
		//if is permanent
		if (curr->m_key == key && curr->m_DLL == nullptr)
			return false;
	}
	//if not in bucket
	return false;
} //end HashTable::touch

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
	//nothing was set << return false
	if (m_head == nullptr || m_capacity == 0 || m_nBuckets == 0)	return false;
	
	//set key and value equal to least recently added node
	//in Doubly Linked List
	key = m_tail->m_key;
	value = m_tail->m_value;
	//delete tail from Doubly Linked List
	deleteTailDLL();

	//now we look for assosication in hash table and delete it
	unsigned int bn = determineBucketForKey(key);
	Node* p = m_buckets[bn];
	//check if assoscation is at the front
	if (p->m_value == value)
	{
		Node* del = p;
		p = del->m_next;
		m_buckets[bn] = p;
		delete del;
		// delete successful
		--m_usedCap;
		return true;
	}
	//else not at the front
	for (; p != nullptr; p=p->m_next)
		if (p->m_next != nullptr && p->m_next->m_value == value)	
			break;
	if (p != nullptr)
	{
		//pointing at Node that is one above the Node to delete
		Node* del = p->m_next;
		p->m_next = del->m_next;
		delete del;
		--m_usedCap;
		return true;
	}
	return false;
} //end discard

//private member functions implementations---------------------------------
template <typename KeyType, typename ValueType>
unsigned int HashTable<KeyType, ValueType>::determineBucketForKey(const KeyType& key) const
{
	unsigned int computeHash(const KeyType& k);
	unsigned int res = computeHash(key);
	return (res % m_nBuckets);
} //end HashTable::determineBucketForKey
//end private member functions implementations


//---------------------------------------------------
//implementations for use of DLL
template <typename KeyType, typename ValueType>
typename HashTable<KeyType, ValueType>::DLL* HashTable<KeyType, ValueType>::addNewDLLToFront(const KeyType& key, const ValueType& value)
{
	//adding to the front
	if (m_head == nullptr)
	{
		m_tail = m_head = new DLL(key, value);
		return m_head;
	}
	//else (Doubly Linked List is not empty)
	DLL* n = new DLL(key, value);
	n->m_next = m_head;
	m_head->m_prev = n;
	m_head = n;
	++m_size;
	return n;
} //end HashTable::addNewDLLToFront

template <typename KeyType, typename ValueType>
void HashTable<KeyType, ValueType>::moveDLLToFront(typename HashTable<KeyType, ValueType>::DLL* f)
{
	//if (DLL is in front)
	if (f == m_head || m_head == nullptr)
		return;
	//if (DLL is at tail)
	if (f == m_tail)
	{
		//link second to last to tail
		f->m_prev->m_next = nullptr;
		m_tail = f->m_prev;
		//link f to front
		f->m_next = m_head;
		f->m_prev = nullptr;
		m_head->m_prev = f;
		m_head = f;
		return;
	}
	//else (f is somewhere in between)
	f->m_prev->m_next = f->m_next;
	f->m_next->m_prev = f->m_prev;
	//link f to front
	f->m_next = m_head;
	f->m_prev = nullptr;
	m_head->m_prev = f;
	m_head = f;
	return;
} //end HashTable::MoveDLLToFront

template <typename KeyType, typename ValueType>
void HashTable<KeyType, ValueType>::deleteTailDLL()
{
	if (m_head == nullptr)
		return;
	//only one DLL node
	if (m_head == m_tail)
	{
		delete m_head;
		m_head = m_tail = nullptr;
	}
	//else delete least recently added DLL node
	DLL* del = m_tail;
	del->m_prev->m_next = nullptr;
	m_tail = del->m_prev;
	delete del;
} //end HashTable::deleteTailDLL

//end implementations for DLL
#endif //HASHTABLE_H
