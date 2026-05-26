#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace std;
namespace fs = std::filesystem;

struct StringArray {
    string* data;
    size_t capacity;
    size_t size;
    StringArray(size_t initial_capacity = 10) {
        capacity = initial_capacity;
        data = new string[capacity];
    }
    ~StringArray() { delete[] data; }
    void push_back(const string& str) {
        if (size == capacity) {
            capacity *= 2;
            string* new_data = new string[capacity];
            for (size_t i = 0; i < size; i++) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }
        data[size++] = str;
    }
};
struct HashNode {
    string key;
    int count;
    HashNode* next;
    HashNode(string k) : key(k), count(1), next(nullptr) {}
};
struct HashTable {
    static const int SIZE = 1000003;
    HashNode* table[SIZE];
    HashTable() {
        for (int i = 0; i < SIZE; i++) table[i] = nullptr;
    }
    ~HashTable() {
        for (int i = 0; i < SIZE; i++) {
            HashNode* curr = table[i];
            while (curr != nullptr) {
                HashNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }
    // djb2 hash
    unsigned long hashString(const string& str) {
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % SIZE;
    }
    void insert(const string& key) {
        if (key.empty()) return;
        int index = hashString(key);
        HashNode* curr = table[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->count++;
                return;
            }
            curr = curr->next;
        }
        HashNode* newNode = new HashNode(key);
        newNode->next = table[index];
        table[index] = newNode;
    }
};
struct LogEntry {
    string user_id = "";
    string device_id = "";
    string app_id = "";
    string resource_id = "";
    string event_type = "";
    string location = "";
    long long timestamp = 0;
};
struct LogArray {
    LogEntry* data;
    size_t capacity;
    size_t size;
    LogArray(size_t initial_capacity = 4500000) {
        capacity = initial_capacity;
        size = 0;
        data = new LogEntry[capacity];
    }
    ~LogArray() { delete[] data; }
    void push_back(const LogEntry& entry) {
        if (size == capacity) {
            capacity *= 2;
            LogEntry* new_data = new LogEntry[capacity];
            for (size_t i = 0; i < size; i++) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
        }
        data[size++] = entry;
    }
};
long long fast_atoll(const char* str, size_t len) {
    long long val = 0;
    for (size_t i = 0; i < len; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            val = val * 10 + (str[i] - '0');
        }
    }
    return val;
}
void swapLog(LogEntry& a, LogEntry& b) {
    LogEntry temp = a;
    a = b;
    b = temp;
}
int partition(LogEntry* arr, int low, int high) {
    long long pivot = arr[high].timestamp;
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].timestamp < pivot) {
            i++;
            swapLog(arr[i], arr[j]);
        }
    }
    swapLog(arr[i + 1], arr[high]);
    return (i + 1);
}
void quickSort(LogEntry* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
int findLowerBound(LogEntry* arr, int size, long long target) {
    int left = 0, right = size - 1;
    int result = size;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid].timestamp >= target) {
            result = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return result;
}
int findUpperBound(LogEntry* arr, int size, long long target) {
    int left = 0, right = size - 1;
    int result = -1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid].timestamp <= target) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return result;
}
void parseLine(const string& line, LogArray& logs, HashTable& resourceTable, bool isFirstLine) {
    if (isFirstLine || line.empty() || line.length() < 5) return;
    LogEntry entry;
    size_t start = 0;
    size_t end = line.find(',');
    int col = 0;
    while (end != string::npos) {
        string field = line.substr(start, end - start);
        size_t first = field.find_first_not_of(" \t\r\n");
        if (first != string::npos) {
            size_t last = field.find_last_not_of(" \t\r\n");
            field = field.substr(first, (last - first + 1));
        } else {
            field = "";
        }
        switch (col) {
            case 0:
                entry.user_id = field;
                break;
            case 1:
                entry.device_id = field;
                break;
            case 2:
                entry.app_id = field;
                break;
            case 3:
                entry.resource_id = field;
                break;
            case 4:
                entry.event_type = field;
                break;
            case 5:
                entry.location = field;
                break;
        }
        col++;
        start = end + 1;
        end = line.find(',', start);
    }
    string ts_field = line.substr(start);
    size_t first = ts_field.find_first_not_of(" \t\r\n");
    if (first != string::npos) {
        size_t last = ts_field.find_last_not_of(" \t\r\n");
        ts_field = ts_field.substr(first, (last - first + 1));
        entry.timestamp = fast_atoll(ts_field.c_str(), ts_field.length());
    }
    resourceTable.insert(entry.resource_id);
    logs.push_back(entry);
}
void parseFromFileOS(const char* filename, LogArray& logs, HashTable& resourceTable) {
    const size_t CHUNK_SIZE = 1024 * 1024;  // 1MB chunk
    char* buffer = new char[CHUNK_SIZE];
    string leftover = "";
    bool isFirstLine = true;
#ifdef _WIN32
    HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "Error: Cannot read from file!\n";
        delete[] buffer;
        return;
    }
    DWORD bytesRead;
    while (ReadFile(hFile, buffer, CHUNK_SIZE, &bytesRead, NULL) && bytesRead > 0) {
#else
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        cout << "Error: Cannot read from file!\n";
        delete[] buffer;
        return;
    }
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, CHUNK_SIZE)) > 0) {
#endif
        string chunk = leftover + string(buffer, bytesRead);
        size_t pos = 0;
        size_t next_pos = 0;
        while ((next_pos = chunk.find('\n', pos)) != string::npos) {
            string line = chunk.substr(pos, next_pos - pos);
            parseLine(line, logs, resourceTable, isFirstLine);
            if (isFirstLine) isFirstLine = false;
            pos = next_pos + 1;
        }
        leftover = chunk.substr(pos);
    }
    if (!leftover.empty()) {
        parseLine(leftover, logs, resourceTable, isFirstLine);
    }
#ifdef _WIN32
    CloseHandle(hFile);
#else
    close(fd);
#endif
    delete[] buffer;
    if (logs.size > 0) {
        quickSort(logs.data, 0, logs.size - 1);
    }
}
void getUser(const string& uid, long long start_time, long long end_time, const LogArray& logs) {
    int start_idx = findLowerBound(logs.data, logs.size, start_time);
    int end_idx = findUpperBound(logs.data, logs.size, end_time);
    if (start_idx > end_idx || start_idx == logs.size || end_idx == -1) {
        cout << "No data in this timestamp :(\n";
        return;
    }
    for (int i = start_idx; i <= end_idx; i++) {
        if (uid == logs.data[i].user_id) {
            cout << logs.data[i].device_id << " -> " << logs.data[i].app_id << " -> "
                 << logs.data[i].resource_id << " (Time: " << logs.data[i].timestamp << ")\n";
        }
    }
}
void getResource(const string& resource, long long start_time, long long end_time,
                 const LogArray& logs) {
    int start_idx = findLowerBound(logs.data, logs.size, start_time);
    int end_idx = findUpperBound(logs.data, logs.size, end_time);
    if (start_idx > end_idx || start_idx == logs.size || end_idx == -1) {
        cout << "No data in this timestamp :(\n";
        return;
    }
    for (int i = start_idx; i <= end_idx; i++) {
        if (resource == logs.data[i].resource_id) {
            cout << logs.data[i].user_id << " -> " << logs.data[i].device_id << " -> "
                 << logs.data[i].app_id << " (Time: " << logs.data[i].timestamp << ")\n";
        }
    }
}
struct TopItem {
    string key;
    int count = 0;
};
void getTop10(HashTable& resourceTable) {
    TopItem top10[10];
    for (int i = 0; i < HashTable::SIZE; i++) {
        HashNode* curr = resourceTable.table[i];
        while (curr != nullptr) {
            int current_count = curr->count;
            for (int j = 0; j < 10; j++) {
                if (current_count > top10[j].count) {
                    for (int k = 9; k > j; k--) {
                        top10[k] = top10[k - 1];
                    }
                    top10[j].count = current_count;
                    top10[j].key = curr->key;
                    break;
                }
            }
            curr = curr->next;
        }
    }
    for (int i = 0; i < 10; i++) {
        if (top10[i].count > 0) {
            cout << "# " << i + 1 << ": " << top10[i].key << " (Retrived : " << top10[i].count
                 << " time(s))\n";
        }
    }
}
string selectLogFile() {
    StringArray csvFiles(10);
    string currentPath = ".";
    cout << "Finding log files...\n";
    try {
        for (const auto& entry : fs::recursive_directory_iterator(currentPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".csv") {
                csvFiles.push_back(entry.path().string());
            }
        }

    } catch (const fs::filesystem_error& e) {
        cout << "Error: " << e.what() << '\n';
        return "";
    }
    if (csvFiles.size == 0) {
        cout << "No log files found!\n";
        return "";
    }
    cout << "------------------------------- LOG FILES -------------------------------\n";
    cout << "0. Exit\n";
    for (size_t i = 0; i < csvFiles.size; ++i) {
        cout << i + 1 << ". " << csvFiles.data[i] << '\n';
    }
    cout << "-------------------------------------------------------------------------\n";
    int choice = -1;
    while (true) {
        cout << "choose file (0-" << csvFiles.size << "): ";
        if (cin >> choice && choice >= 0 && choice <= (int)csvFiles.size) {
            break;
        } else {
            cout << "Chosen file is not valid or does not exists :(\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }
    if (choice == 0) {
        return "";
    }
    return csvFiles.data[choice - 1];
}
int main() {
    string targetFile = selectLogFile();
    if (targetFile == "") {
        cout << "Exited successfully\n";
        return 0;
    }
    LogArray logs(4500000);
    HashTable resourceTable;
    cout << "Loading file...\n";
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    parseFromFileOS(targetFile.c_str(), logs, resourceTable);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << "File read and sorted in "
         << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000
         << " miliseconds\n";
    cout << "Total records: " << logs.size << "\n";
    while (true) {
        cout << "-----------------------------------------------HALO "
                "ENGINE-------------------------------------------\n";
        cout << "0. Exit\n";
        cout << "1. List user's path (Device -> App -> Resource)\n";
        cout << "2. List resource's path (User -> Device -> App)\n";
        cout << "3. Rank 10 most retrived resources\n";
        cout << "----------------------------------------------------------------------------------"
                "-------------------\n";
        int input = 0;
        cout << "Input: ";
        cin >> input;
        if (input == 0) {
            cout << "Exited successfully!\n";
            break;
        }
        switch (input) {
            case 1: {
                string uid;
                long long start_t, end_t;
                cout << "User's ID: ";
                cin >> uid;
                cout << "Time interval start (Epoch): ";
                cin >> start_t;
                cout << "Time interval end (Epoch): ";
                cin >> end_t;
                getUser(uid, start_t, end_t, logs);
                break;
            }
            case 2: {
                string rid;
                long long start_t, end_t;
                cout << "Resource's ID: ";
                cin >> rid;
                cout << "Time interval start (Epoch): ";
                cin >> start_t;
                cout << "Time interval end (Epoch): ";
                cin >> end_t;
                getResource(rid, start_t, end_t, logs);
                break;
            }
            case 3: {
                getTop10(resourceTable);
                break;
            }
        }
    }
    return 0;
}
