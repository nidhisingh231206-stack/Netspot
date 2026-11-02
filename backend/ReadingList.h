
#ifndef READING_LIST_H
#define READING_LIST_H

#include <string>
#include <cstddef>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>

class ReadingList
{
private:
    struct Node
    {
        std::string data;
        Node* next;
        Node(const std::string& value)
        {
            data = value;
            next = nullptr;
        }
    };

    Node* head;
    Node* tail;
    std::size_t count;
    mutable std::mutex mtx;

public:
    ReadingList()
    {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    // Prevent accidental copying of linked-list pointers
    ReadingList(const ReadingList&) = delete;
    ReadingList& operator=(const ReadingList&) = delete;

    // Insert a reading at the end of the list
    void append(const std::string& reading)
    {
        std::lock_guard<std::mutex> lock(mtx);
        Node* newNode = new Node(reading);

        if (head == nullptr)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }

        count++;
    }

    // Return the most recently stored reading
    bool getLatest(std::string& reading) const
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (tail == nullptr)
        {
            return false;
        }

        reading = tail->data;
        return true;
    }

    std::vector<std::string> getRecent(std::size_t limit) const
    {
        std::lock_guard<std::mutex> lock(mtx);

        std::vector<std::string> result;

        std::size_t take = std::min(limit, count);
        std::size_t skip = count - take;

        Node* current = head;

        while (current != nullptr && skip > 0)
        {
            current = current->next;
            skip--;
        }

        while (current != nullptr)
        {
            result.push_back(current->data);
            current = current->next;
        }

        std::reverse(result.begin(), result.end());

        return result;
    }

    std::size_t size() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return count;
    }

    bool isEmpty() const
    {
        std::lock_guard<std::mutex> lock(mtx);
        return head == nullptr;
    }

    // Release all dynamically allocated nodes
    ~ReadingList()
    {
        Node* current = head;

        while (current != nullptr)
        {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }
};

#endif

