#include <iostream>
#include <cstring>
#include <cctype>


enum j_type {
    type_integer,
    type_string,
    type_boolean,
    type_float,
    type_none
};

enum ste_entry_type {
    ste_var,
    ste_const,
    ste_routine,
    ste_undefined
};

static char *type_names[] = {"integer", "string", "boolean", "float", "none"};

struct SymbolTableEntry {
    char *name;
    SymbolTableEntry *next;
    ste_entry_type entry_type;

    union {
        struct {
            j_type type;
        } var;
        struct {
            int value;
        } constant;
        struct {
            j_type result_type;
        } routine;
    } f;
};

class SymbolTable {
private:
    SymbolTableEntry **slots;
    int fold_case;
    int number_entries;
    int number_probes;
    int number_hits;
    int max_search_dist;
    SymbolTable *next;

    size_t hash(const char *str);

public:
    SymbolTable();
    SymbolTable(int fold_case_flag);
    ~SymbolTable();
    void clearSymbolTable();
    SymbolTableEntry *getSymbol(const char *str);
    SymbolTableEntry *putSymbol(const char *str);
    void printSymbolStats();
};

SymbolTable::SymbolTable() : SymbolTable(0) {}

SymbolTable::SymbolTable(int fold_case_flag) {
    fold_case = fold_case_flag;
    number_entries = 0;
    number_probes = 0;
    number_hits = 0;
    max_search_dist = 0;
    next = nullptr;
    slots = new SymbolTableEntry *[256](); // Initialize hash table with nullptrs
}

SymbolTable::~SymbolTable() {
    clearSymbolTable();
    delete[] slots;
}

size_t SymbolTable::hash(const char *str) {
    size_t hashValue = 0;
    size_t prime = 31; // A common prime number used in hash functions

    while (*str) {
        char c = *str++;
        if (fold_case) {
            c = tolower(c);
        }
        hashValue = hashValue * prime + c;
    }

    return hashValue % 256; // Adjust to fit within table size
}

void SymbolTable::clearSymbolTable() {
    for (int i = 0; i < 256; ++i) {
        SymbolTableEntry *entry = slots[i];
        while (entry) {
            SymbolTableEntry *nextEntry = entry->next;
            delete[] entry->name;
            delete entry;
            entry = nextEntry;
        }
        slots[i] = nullptr;
    }
    number_entries = 0;
}

SymbolTableEntry *SymbolTable::getSymbol(const char *str) {
    size_t index = hash(str);
    SymbolTableEntry *entry = slots[index];
    number_probes++;

    while (entry) {
        if (strcmp(entry->name, str) == 0) {
            number_hits++;
            return entry;
        }
        entry = entry->next;
    }

    return nullptr;
}

SymbolTableEntry *SymbolTable::putSymbol(const char *str) {
    size_t index = hash(str);
    SymbolTableEntry *entry = slots[index];

    while (entry) {
        if (strcmp(entry->name, str) == 0) {
            return entry;
        }
        entry = entry->next;
    }

    // Create a new entry
    SymbolTableEntry *newEntry = new SymbolTableEntry;
    newEntry->name = new char[strlen(str) + 1];
    strcpy(newEntry->name, str);
    newEntry->next = slots[index];
    slots[index] = newEntry;
    number_entries++;

    return newEntry;
}

void SymbolTable::printSymbolStats() {
    int emptySlots = 0;
    int longestSearchChain = 0;

    for (int i = 0; i < 256; ++i) {
        SymbolTableEntry *entry = slots[i];
        int chainLength = 0;

        if (!entry) {
            emptySlots++;
        } else {
            while (entry) {
                chainLength++;
                entry = entry->next;
            }
            if (chainLength > longestSearchChain) {
                longestSearchChain = chainLength;
            }
        }
    }

    std::cout << "Number of entries: " << number_entries << std::endl;
    std::cout << "Number of empty slots: " << emptySlots << std::endl;
    std::cout << "Average number of probes: " << static_cast<double>(number_probes) / number_entries << std::endl;
    std::cout << "Number of hits: " << number_hits << std::endl;
    std::cout << "Longest search chain: " << longestSearchChain << std::endl;
}

int main() {
    SymbolTable table(true); // Create a symbol table with case folding
    SymbolTableEntry *entry1 = table.putSymbol("apple");
    SymbolTableEntry *entry2 = table.putSymbol("banana");
    SymbolTableEntry *entry3 = table.getSymbol("apple");
    SymbolTableEntry *entry4 = table.getSymbol("orange");

    if (entry1) {
        entry1->entry_type = ste_var;
        entry1->f.var.type = type_string;
    }

    if (entry2) {
        entry2->entry_type = ste_const;
        entry2->f.constant.value = 42;
    }

    if (entry3) {
        std::cout << "Found entry: " << entry3->name << std::endl;
    } else {
        std::cout << "Entry not found." << std::endl;
    }

    if (entry4) {
        std::cout << "Found entry: " << entry4->name << std::endl;
    } else {
        std::cout << "Entry not found." << std::endl;
    }

    table.printSymbolStats();
    return 0;
}
