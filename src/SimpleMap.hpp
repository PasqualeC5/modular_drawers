#ifndef SIMPLEMAP_HPP
#define SIMPLEMAP_HPP

template <typename K, typename V>
class SimpleMap
{
private:
    struct KeyValuePair
    {
        K key;
        V value;
    };

    KeyValuePair *data;
    size_t capacity;
    size_t size;

public:
    // Constructor
    SimpleMap() : data(nullptr), capacity(0), size(0) {}

    // Destructor
    ~SimpleMap()
    {
        delete[] data;
    }

    // [] Operator Overload
    V &operator[](const K &key)
    {
        // Check if the key already exists
        for (size_t i = 0; i < size; ++i)
        {
            if (data[i].key == key)
            {
                return data[i].value;
            }
        }

        // If the key doesn't exist, add a new key-value pair
        if (size == capacity)
        {
            // If the array is full, double its capacity
            capacity = (capacity == 0) ? 1 : capacity * 2;

            KeyValuePair *newData = new KeyValuePair[capacity];
            // Copy existing data to the new array
            for (size_t i = 0; i < size; ++i)
            {
                newData[i] = data[i];
            }

            // Delete old data array and update the pointer
            delete[] data;
            data = newData;
        }

        // Add the new key-value pair
        data[size].key = key;
        return data[size++].value;
    }

    // Find method
    bool find(const K &key) const
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (data[i].key == key)
            {
                return true;
            }
        }
        return false;
    }
};
#endif // SIMPLEMAP_HPP