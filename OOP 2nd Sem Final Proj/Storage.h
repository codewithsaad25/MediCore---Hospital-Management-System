#ifndef STORAGE_H
#define STORAGE_H

template <class T>
class Storage {
private:
    T data[100];
    int count;

public:
    Storage() {
        count = 0;
    }

    bool add(const T& item) {
        if (count >= 100) {
            return false;
        }
        data[count] = item;
        count++;
        return true;
    }

    bool removeById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == id) {
                for (int j = i; j < count - 1; j++) {
                    data[j] = data[j + 1];
                }
                count--;
                return true;
            }
        }
        return false;
    }

    T* findById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == id) {
                return &data[i];
            }
        }
        return 0;
    }

    bool contains(const T& item) {
        for (int i = 0; i < count; i++)
            if (data[i] == item) return true;
        return false;
    }

    T* getAll() {
        return data;
    }

    const T* getAll() const {
        return data;
    }

    int size() const {
        return count;
    }

    void clear() {
        count = 0;
    }
};

#endif#pragma once
