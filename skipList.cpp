#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <ctime>
#include <climits>
using namespace std;

struct Event {
    string title;
    string timeStart;
    string timeEnd;
    string color;
};

struct Node {
    int date; 
    vector<Event> events;
    vector<Node*> forward;

    Node(int d, int level) : date(d), forward(level + 1, nullptr) {}
};

class Skiplist {
private:
    Node* head;
    int maxLevel;
    float p;

    int randomLevel() {
        int level = 0;
        while ((rand() / (float)RAND_MAX) < p && level < maxLevel) {
            ++level;
        }
        return level;
    }

public:
    Skiplist() : maxLevel(16), p(0.5) {
        head = new Node(INT_MIN, maxLevel);
        srand(time(0));
    }

    static int convertDateToInt(const string& date) {
        istringstream iss(date);
        int day, year;
        string month;
        iss >> day >> month >> year;

        static map<string, int> monthMap = {
            {"January", 1}, {"February", 2}, {"March", 3}, {"April", 4},
            {"May", 5}, {"June", 6}, {"July", 7}, {"August", 8},
            {"September", 9}, {"October", 10}, {"November", 11}, {"December", 12}
        };
        int monthNumber = monthMap[month];

        return year * 10000 + monthNumber * 100 + day;
    }

    void add(int date, const Event& event) {
        vector<Node*> update(maxLevel + 1, nullptr);
        Node* current = head;
        for (int i = maxLevel; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->date < date) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (current && current->date == date) {
            current->events.push_back(event);
            return;
        }

        int level = randomLevel();
        Node* newNode = new Node(date, level);
        for (int i = 0; i <= level; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
        newNode->events.push_back(event);
    }

    vector<Event> search(int date) {
        Node* current = head;
        for (int i = maxLevel; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->date < date) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];
        if (current && current->date == date) {
            return current->events;
        }
        return {};
    }

    bool erase(int date) {
        vector<Node*> update(maxLevel + 1, nullptr);
        Node* current = head;
        for (int i = maxLevel; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->date < date) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (!current || current->date != date) {
            return false;
        }

        for (int i = 0; i <= maxLevel; ++i) {
            if (update[i]->forward[i] != current) {
                break;
            }
            update[i]->forward[i] = current->forward[i];
        }

        delete current;
        return true;
    }

    ~Skiplist() {
        Node* current = head;
        while (current) {
            Node* next = current->forward[0];
            delete current;
            current = next;
        }
    }
};

int displayMenu() {
    int choice;
    cout << "\nSkiplist Menu:\n";
    cout << "1. Add an event\n";
    cout << "2. Delete events on a date\n";
    cout << "3. Search events on a date\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;
    return choice;
}

Event readEvent() {
    Event event;
    cout << "Enter event title: ";
    cin.ignore();
    getline(cin, event.title);
    cout << "Enter event start time (HH:MM): ";
    getline(cin, event.timeStart);
    cout << "Enter event end time (HH:MM): ";
    getline(cin, event.timeEnd);
    cout << "Enter event color: ";
    getline(cin, event.color);
    return event;
}

int readDate() {
    string date;
    cout << "Enter date (e.g., 23 June 2024): ";
    cin.ignore();
    getline(cin, date);
    return Skiplist::convertDateToInt(date);
}

int main() {
    Skiplist skiplist;
    int choice;

    do {
        choice = displayMenu();

        switch (choice) {
            case 1: { 
                int date = readDate();
                Event event = readEvent();
                skiplist.add(date, event);
                cout << "Event added successfully.\n";
                break;
            }
            case 2: { 
                int date = readDate();
                if (skiplist.erase(date)) {
                    cout << "Events on the date deleted successfully.\n";
                } else {
                    cout << "No events found on the given date.\n";
                }
                break;
            }
            case 3: { 
                int date = readDate();
                auto events = skiplist.search(date);
                if (!events.empty()) {
                    cout << "Events on the given date:\n";
                    for (const auto& e : events) {
                        cout << "Title: " << e.title
                                  << ", Start: " << e.timeStart
                                  << ", End: " << e.timeEnd
                                  << ", Color: " << e.color << endl;
                    }
                } else {
                    cout << "No events found on the given date.\n";
                }
                break;
            }
            case 4: 
                cout << "Exiting program...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 4);

    return 0;
}