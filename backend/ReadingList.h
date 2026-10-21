
#ifndef READING_LIST_H
#define READING_LIST_H

#include <string>
#include <cstddef>

class ReadingList
{
private:
    struct Node
    {
        std::string data;
        Node* next;

        explicit Node(const std::string& value)
            : data(value), next(nullptr)
        {
        }
    };

    Node* head;
    Node* tail;
    std::size_t count;

public:
    ReadingList()
        : head(nullptr), tail(nullptr), count(0)
    {
    }

    // Prevent accidental copying of linked-list pointers
    ReadingList(const ReadingList&) = delete;
    ReadingList& operator=(const ReadingList&) = delete;

    // Insert a reading at the end of the list
    void append(const std::string& reading)
    {
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

        ++count;
    }

    // Return the most recently stored reading
    bool getLatest(std::string& reading) const
    {
        if (tail == nullptr)
        {
            return false;
        }

        reading = tail->data;
        return true;
    }

    std::size_t size() const
    {
        return count;
    }

    bool isEmpty() const
    {
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

