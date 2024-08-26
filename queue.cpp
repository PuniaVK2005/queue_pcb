#include <functional>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>


enum class E_PCB_STATE
{
    NEW,
    IDLE,
    READY,
    RUNNNG,
    SUSPENDED,

    //----
    INVALID,
};

std::string getPCBStateString(E_PCB_STATE state)
{
    switch(state)
    {
        case E_PCB_STATE::NEW:          return {"NEW"};
        case E_PCB_STATE::IDLE:         return {"IDLE"};
        case E_PCB_STATE::READY:        return {"READY"};
        case E_PCB_STATE::RUNNNG:       return {"RUNNING"};
        case E_PCB_STATE::SUSPENDED:    return {"SUSPENDED"};
        default:                        return {"INVALID"};
    }
}

struct PCB
{
    static std::size_t s_id;
    std::size_t id{s_id++};
    E_PCB_STATE state{E_PCB_STATE::INVALID};
    std::size_t time{};
};

std::size_t PCB::s_id{0};


std::ostream& operator<< (std::ostream& out, PCB& pcb)
{
    out << pcb.id;
    return out;
}

bool operator== (PCB& pcb1, PCB& pcb2)
{
    return pcb1.id == pcb2.id;
}


//---[Node]--------------------------------
template <typename T>
struct Node
{
    Node(
         T data
        , Node<T>* prev = nullptr
        , Node<T>* next = nullptr
    )
        : data(data)
        , prev(prev)
        , next(next)
    {}

    Node<T>* prev{nullptr};
    Node<T>* next{nullptr};
    T data{};
};

//---------------------------------------------------------

//--[ All Algorithms here ]---------------------------------------
template <typename T>
class LinkedListManager
{
    using Node_t = Node<T>;
public:
    LinkedListManager() = default;

    void create(T data)
    {
        if(!mHead)
        {
            mHead = new Node_t(data);
            mTail = mHead;
            return;
        }
        Node_t* newNode{new Node_t(data)};
        mTail->next = newNode;
        mTail = mTail->next;
    }

    void deleteAt(int position)
    {
        if(position < 0) return;

        if(position == 0)
        {
            if(mHead)
            {
                Node_t* next{mHead->next};
                delete mHead;
                mHead = next;
            }
            return;
        }

        Node_t* prev{mHead};
        Node_t* node{mHead};
        while(node && position--)
        {
            prev = node;
            node = node->next;
        }

        if(node == nullptr) return;
        prev->next = node->next;
        delete node;
    }

    T const* search(T needle)
    {
        Node_t* node{mHead};
        int pos{0};
        while(node) 
        {
            if(node->data == needle) 
            {
                return &(node->data);
            }
            pos++;
            node = node->next;
        }
        return nullptr;
    }

    void sort()
    {
        std::vector<int> list{};
        Node_t* node{mHead};

        while(node)
        {
            list.push_back(node->data);
            node = node->next;
        }

        std::sort(list.begin(), list.end());
        
        node = mHead;

        int idx{0};
        while(node)
        {
            node->data = list[idx++];
            node = node->next;
        }
    }

    void printLL()
    {
        Node_t* node{mHead};
        while(node)
        {
            std::cout << node->data << " -> ";
            node = node->next;
        }
        std::cout << " nullptr \n";
    }
    
private:
    Node_t* mHead{nullptr};
    Node_t* mTail{nullptr};
};
//---------------------------------------------------------


//--[Helpers]----------------------------------------------

void printHelp()
{
    std::cout << R"(
+-------------------------------------+
|           Enter an action           |
+-------------------------------------+
| e   -> enqueue new PCB              |
| d   -> dequeue PCB                  |
| i 5 -> print pcb's info given id=5  |
| q -> quit                           |
+-------------------------------------+
    )";
};

void cls()
{
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

//------------------------------------------------------


void enqueueAction(LinkedListManager<PCB>& llm)
{
    PCB pcb{};
    pcb.state = E_PCB_STATE::NEW;
    std::cout << R"(
    ENTER STATE:
        n -> New
        i -> Idle
        d -> Ready
        r -> Running
        s -> Suspended
    )";
    char state{};
    std::cin >> state;

    switch(state)
    {
        case 'n':
            pcb.state = E_PCB_STATE::NEW;
        break;
        case 'i':
            pcb.state = E_PCB_STATE::IDLE;
        break;
        case 'd':
            pcb.state = E_PCB_STATE::READY;
        break;
        case 'r':
            pcb.state = E_PCB_STATE::RUNNNG;
        break;
        case 's':
            pcb.state = E_PCB_STATE::SUSPENDED;
        break;
        default:
            pcb.state = E_PCB_STATE::INVALID;
    }

    pcb.time = std::time(nullptr);
    llm.create(pcb);
}

void dequeueAction(LinkedListManager<PCB>& llm)
{
    llm.deleteAt(0);
}

void infoAction(LinkedListManager<PCB>& llm)
{
    std::size_t id{};
    std::cin >> id;
    auto* pcb{llm.search({.id = id})};
    if(!pcb)
    {
        std::cout << "PCB #" << id << " Not found! \n";
    }
    else
    {
        std::cout 
            << "---- PCB INFO ----\n"
            << "id    : #" <<  pcb->id << '\n'
            << "State : " <<  getPCBStateString(pcb->state) << '\n'
            << "Time  : " << pcb->time << '\n'
            << "------------------------------ \n";
    }

    std::cout << " Press Any Key To Continue...\n";
    std::cin.get();
    std::cin.ignore();

}


//------------------------------------------------------

template <typename T>
using ActionCallback_t = std::function<void(LinkedListManager<T>&)>;

//--[main]----------------------------------------------
int main()
{
    LinkedListManager<PCB> llm{};
    bool isRunning{true};
    std::unordered_map<char, ActionCallback_t<PCB> > actionMap {
        {'e', enqueueAction},
        {'d', dequeueAction},
        {'i', infoAction},
        {'q', [&isRunning](LinkedListManager<PCB>& llm){  isRunning = false;  }},
    };


    while(isRunning)
    {
        cls();
        printHelp();
        std::cout << "Jobs List : ";
        llm.printLL();
        char action{};
        std::cin >> action;

        if(actionMap.find(action) != actionMap.end())
        {
            (actionMap[action])(llm);
        }
        else 
        {
            std::cout << "Invalid Operation\nPress Any Key To Continue...";
            std::cin.get();
            std::cin.ignore();
        }

    }

    std::cout << "Exiting...";

}
