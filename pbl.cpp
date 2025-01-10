#include <iostream>
#include <string>
#include <stack>

using namespace std;

// Structure for Task
struct Task {
    string name;
    int priority;         // 1: High, 2: Medium, 3: Low
    string deadline;      // Format: YYYY-MM-DD
    bool completed;       // true if completed
    Task* next;

    Task(string n, int p, string d)
        : name(n), priority(p), deadline(d), completed(false), next(nullptr) {}
};

// Structure for Action
struct Action {
    string type; // "ADD" or "DELETE"
    Task* task;

    Action(const string& t, Task* tk) : type(t), task(tk) {}
};


// Linked list for task storage
class TaskManager {
private:
    Task* head;
    stack<Action> undoStack; // Stack for undo operations
    stack<Action> redoStack; // Stack for redo operations

    // Helper: Deep copy a task
    Task* deepCopyTask(Task* task) {
        if (!task) return nullptr;
        Task* copy = new Task(task->name, task->priority, task->deadline);
        copy->completed = task->completed;
        return copy;
    }

public:
    TaskManager() : head(nullptr) {}
     
    // Add a new task
    void addTask(const string& name, int priority, const string& deadline) {
        Task* newTask = new Task(name, priority, deadline);
        newTask->next = head;
        head = newTask;

        undoStack.push(Action("ADD", deepCopyTask(newTask))); // Push an "ADD" action
        while (!redoStack.empty()) redoStack.pop(); // Clear redo stack
        cout << "Task '" << name << "' added successfully!\n";
    }

    // Delete a task
    void deleteTask(const string& name) {
        Task *current = head, *prev = nullptr;
        while (current) {
            if (current->name == name) {
                undoStack.push(Action("DELETE", deepCopyTask(current))); // Save a "DELETE" action
                if (prev) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
                delete current;
                while (!redoStack.empty()) redoStack.pop(); // Clear redo stack
                cout << "Task '" << name << "' deleted successfully!\n";
                return;
            }
            prev = current;
            current = current->next;
        }
        cout << "Task '" << name << "' not found!\n";
    }

// Undo the last operation
void undo() {
    if (undoStack.empty()) {
        cout << "No actions to undo!\n";
        return;
    }

    // Retrieve the last action
    Action lastAction = undoStack.top();
    undoStack.pop();

    if (lastAction.type == "ADD") {
        // Undo an add by removing the task from the list
        Task* current = head;
        Task* prev = nullptr;

        while (current) {
            if (current->name == lastAction.task->name) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
                delete current; // Free the memory for the removed task
                break;
            }
            prev = current;
            current = current->next;
        }

        // Save the "ADD" action to the redo stack
        redoStack.push(Action("ADD", deepCopyTask(lastAction.task)));

        cout << "Undo: Task '" << lastAction.task->name << "' removed.\n";
    } 
    else if (lastAction.type == "DELETE") {
        // Undo a delete by re-adding the task
        Task* newTask = new Task(lastAction.task->name, lastAction.task->priority, lastAction.task->deadline);
        newTask->completed = lastAction.task->completed;
        newTask->next = head;
        head = newTask;

        // Save the "DELETE" action to the redo stack
        redoStack.push(Action("DELETE", deepCopyTask(lastAction.task)));

        cout << "Undo: Task '" << lastAction.task->name << "' restored.\n";
    }

    // Free memory for the copied task in the lastAction
    delete lastAction.task;
}


    // Redo the last undone operation
    void redo() {
        if (redoStack.empty()) {
            cout << "No actions to redo!\n";
            return;
        }

        Action lastUndo = redoStack.top();
        redoStack.pop();

        if (lastUndo.type == "ADD") {
            addTask(lastUndo.task->name, lastUndo.task->priority, lastUndo.task->deadline);
        } else if (lastUndo.type == "DELETE") {
            deleteTask(lastUndo.task->name);
        }

        delete lastUndo.task; // Free the copied task
    }

    // Mark a task as completed
    void markCompleted(const string& name) {
        Task* current = head;
        while (current) {
            if (current->name == name) {
                current->completed = true;
                cout << "Task '" << name << "' marked as completed!\n";
                return;
            }
            current = current->next;
        }
        cout << "Task '" << name << "' not found!\n";
    }

    // Display all tasks
    void displayTasks() {
        if (!head) {
            cout << "No tasks available.\n";
            return;
        }
        Task* current = head;
        cout << "\nAll Tasks:\n";
        while (current) {
            cout << "Task: " << current->name << ", Priority: " << current->priority
                 << ", Deadline: " << current->deadline
                 << ", Completed: " << (current->completed ? "Yes" : "No") << endl;
            current = current->next;
        }
    }

    // Destructor to free memory
    ~TaskManager() {
        while (head) {
            Task* temp = head;
            head = head->next;
            delete temp;
        }
        while (!undoStack.empty()) {
            delete undoStack.top().task;
            undoStack.pop();
        }
        while (!redoStack.empty()) {
            delete redoStack.top().task;
            redoStack.pop();
        }
    }
};

int main() {
    TaskManager manager;
    int choice;

    do {
        cout << "\nTask Manager Menu:\n";
        cout << "1. Add Task\n2. Delete Task\n3. Mark Task as Completed\n4. Undo\n5. Redo\n6. Display Tasks\n7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        string name, deadline;
        int priority;

        switch (choice) {
            case 1:
                cin.ignore(); // Clear newline from input buffer
                cout << "Enter task name: ";
                getline(cin, name);
                cout << "Enter priority (1-High, 2-Medium, 3-Low): ";
                cin >> priority;
                cout << "Enter deadline (YYYY-MM-DD): ";
                cin >> deadline;
                manager.addTask(name, priority, deadline);
                break;
            case 2:
                cin.ignore(); // Clear newline from input buffer
                cout << "Enter task name to delete: ";
                getline(cin, name);
                manager.deleteTask(name);
                break;
            case 3:
                cin.ignore(); // Clear newline from input buffer
                cout << "Enter task name to mark as completed: ";
                getline(cin, name);
                manager.markCompleted(name);
                break;
            case 4:
                manager.undo();
                break;
            case 5:
                manager.redo();
                break;
            case 6:
                manager.displayTasks();
                break;
            case 7:
                cout << "Exiting Task Manager. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 7);

    return 0;
}
