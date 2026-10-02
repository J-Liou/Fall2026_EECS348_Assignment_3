/*
Program Name: EECS 348 Assignment 3
Description: C++ program that prioritizes emails for a CEO. Uses a max heap to organize based on priority from people or date(tie-breaker).
Inputs: filename
Output: prints emails in order of priority and what emails need to be read.
Other Sources: ChatGPT
Collaborators: ChatGPT
Author: Jonathan Liou
Created: Sept 29th, 2026
Revised: Oct 1st, 2026
Revisions: Moved print for reusability, removed temp queue for nodes, replaced endl with \n, and made sure dates are valid
*/

#include <iostream> //for input and output
#include <fstream> //for file reading
// #include <sstream> REMOVED FOR CHANGES
#include <string> //for strings
#include <utility> //for swap
#include <stdexcept> //for error

using namespace std; //to get rid of std namespace usages

class Email { //email class init
private: //private members
    string sender; //sender member
    string subject; //subject member
    string date; //date member

    int priority; //priority member
    int month; //month member
    int day; //day member
    int year; //year member

//CHANGE FOR VALID DATES : ChatGPT
    //strict date validation
    static bool validDate(const string& date) { //takes date string
        
        if (date.size() != 10 || date[2] != '-' || date[5] != '-') { //if not in form MM-DD-YYYY
            return false; //return false
        } //end of format check

        //chars must be decimal if not '-'
        for (int i = 0; i < 10; i++) { //loop through date string
            if (i != 2 && i != 5 && (date[i] < '0' || date[i] > '9')) { //check for numbers 0-9 of m/d/y
                return false; //false if not valid
            } //end of decimal check
        } //end of loop of date

        //get individual m,d, and y
        int month = stoi(date.substr(0, 2)); //turn month str into int
        int day = stoi(date.substr(3, 2)); //day to int
        int year = stoi(date.substr(6, 4)); //year to int

        if (year < 1 || month < 1 || month > 12) { //check for valid year and month
            return false; //false if invalid
        } //end of check

        //check for month specific days
        int daysInMonth[] = { //make array of days in month
            31, 28, 31, 30, 31, 30, //first 6
            31, 31, 30, 31, 30, 31  // last 6 months
        }; //end of array

        //check for leap year days
        if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) { //check if leap year
            daysInMonth[1] = 29; //makes feb have 29 days instead
        } //end of leap year check

        return day >= 1 && day <= daysInMonth[month - 1]; //check valid day
    } //end of valid date check


public:     // public members
//CHANGE TO SPACE COMPLEXITY: ChatGPT
    Email(const string& sender, const string& subject, const string& date) //params are pass by reference
        : sender(sender), subject(subject), date(date) { //initializer list

        if (!validDate(date)) { //checks valid date
            throw invalid_argument("Invalid date: " + date); //or throw error
        } // end of valid date check

//END OF CHANGE

        //sender priority check
        if (sender == "Boss") { //if sender is boss
            priority = 5; //prio value of 5
        } //end of boss check
        else if (sender == "Subordinate") { //if sender is subordinate
            priority = 4;  //prio value of 4
        }  //end of subordinate check
        else if (sender == "Peer") {    //if sender is peer
            priority = 3;   //prio value of 3
        } //end of peer check
        else if (sender == "ImportantPerson") { //if sender is important person
            priority = 2;   //prio value of 2
        } //end of important person
        else { //other senders
            priority = 1; //least prio
        } //end of else

        //get m, d, and y
        month = stoi(date.substr(0, 2)); //convert month str into int
        day = stoi(date.substr(3, 2)); //day to int
        year = stoi(date.substr(6, 4)); //year to int
    } //end of email constructor

    //CHANGED GET FUNCTIONS TO PASS BY REF : CHATGPT
    const string& getSender() const { //gets sender
        return sender; //return sender string
    } //end of sender func

    const string& getSubject() const { //gets subject line
        return subject; //return subject str
    } //end of func

    const string& getDate() const { //gets date
        return date; //return date str
    } //end of func

    //return true if prio is higher than other email prio
    bool higherPriorityThan(const Email& other) const { //takes email object

        if (priority != other.priority) {  //compare sender prio
            return priority > other.priority; //returns if prio is higher
        } //end of sender prio check

        //or check recent date
        if (year != other.year) { //compare earlier year
            return year > other.year; //returns if earlier year
        } //end of year check

        // or check recent month
        if (month != other.month) { //compare earlier month
            return month > other.month; //return if month is recent
        } //end of month check
        
        //else check recent day
        return day > other.day; //return if day is more recent
    }
    
        //REMOVED BECAUSE MAIN NOW PRINTS
//    void display() {
//       cout << "Sender: " << sender << endl;
//       cout << "Subject: " << subject << endl;
//       cout << "Date: " << date << endl;
//    }
};

//CHANGE MAXHEAP TO LINKED HEAP : ChatGPT
class MaxHeap { //maxheap class
private: //private members
    struct Node { //node for one email
        Email email; //stores email
        Node* left; //left child
        Node* right; //right child
        Node* parent; //parent node

        Node(const Email& email) //takes email by reference
            : email(email), left(nullptr), right(nullptr), parent(nullptr) {} //init members
    }; //end of node

    Node* root = nullptr; //start with empty heap
    int emailCount = 0; //number of emails

    //gets a node by its position, starting with root at 1
    Node* getNode(int position) const { //takes valid position in heap
        Node* current = root; //start at root
        int bit = 1; //start with lowest binary place
        while (bit <= position / 2) { //find highest binary place
            bit *= 2; //move to next binary place
        } //end of bit loop
        bit /= 2; //skip first bit which represents root
        while (bit > 0) { //follow remaining bits
            if ((position & bit) == 0) { //0 means left
                current = current->left; //go left
            } else { //1 means right
                current = current->right; //go right
            } //end of direction check
            bit /= 2; //move to next bit
        } //end of path loop
        return current; //return node at position
    } //end of getNode

    void deleteNodes(Node* node) { //cleans up heap memory
        if (node == nullptr) return; //stop if no node
        deleteNodes(node->left); //delete left side
        deleteNodes(node->right); //delete right side
        delete node; //delete current node
    } //end of cleanup

public: //public members

    ~MaxHeap() { //destructor runs when heap is done being used
        deleteNodes(root); //free remaining nodes
    } //end of destructor

    void add(const Email& email) { //add email to heap
        Node* node = new Node(email); //make email node
        emailCount++; //increase count
        if (root == nullptr) { //if first email
            root = node; //make it root
            return; //done adding
        } //end of empty check

        Node* parent = getNode(emailCount / 2); //get new nodes parent
        node->parent = parent; //set parent pointer
        if (emailCount % 2 == 0) { //even position is left child
            parent->left = node; //attach on left
        } else { //odd position is right child
            parent->right = node; //attach on right
        } //end of child check

        //upheap code
        while (node->parent != nullptr &&
               node->email.higherPriorityThan(node->parent->email)) { //if higher prio than parent
            swap(node->email, node->parent->email); //swap emails
            node = node->parent; //move up
        } //end of upheap
    } //end of add

    //CHANGE FOR PRINTING IN MAIN
    const Email* next() const { //gets next email without removing it
        if (root == nullptr) return nullptr; //if empty
        return &root->email; //return root email
    } //end of next

    void read() { //remove highest prio email
        if (root == nullptr) return; //if empty do nothing
        if (emailCount == 1) { //if only one email
            delete root; //delete only node
            root = nullptr; //heap is empty
            emailCount = 0; //reset count
            return; //done removing
        } //end of single email check

        Node* last = getNode(emailCount); //get last node
        root->email = last->email; //replace root email
        if (last->parent->left == last) { //if last is left child
            last->parent->left = nullptr; //remove left link
        } else { //if last is right child
            last->parent->right = nullptr; //remove right link
        } //end of link check
        delete last; //free last node
        emailCount--; //decrease count

        Node* node = root; //start at root
        while (true) { //downheap code
            Node* largest = node; //assume current is highest
            if (node->left != nullptr &&
                node->left->email.higherPriorityThan(largest->email)) { //check left prio
                largest = node->left; //left is higher
            } //end of left check
            if (node->right != nullptr &&
                node->right->email.higherPriorityThan(largest->email)) { //check right prio
                largest = node->right; //right is higher
            } //end of right check
            if (largest == node) break; //stop if already in order
            swap(node->email, largest->email); //swap emails
            node = largest; //move down
        } //end of downheap
    } //end of read

    int count() const { //gets unread count
        return emailCount; //return count for main to print
    } //end of count
}; //end of maxheap

int main(int argc, char* argv[]) { //main function

    if (argc < 2) { //check for file name
        cout << "Please provide an input file." << '\n'; //CHANGED endl to newline
        return 1; //stop program
    } //end of file check

    ifstream inputFile(argv[1]); //open file from argument

    if (!inputFile.is_open()) { //check if file opened
        cout << "Unable to open file." << '\n'; //CHANGED endl to newline
        return 1; //stop program
    } //end of file error

    MaxHeap inbox; //make inbox

    string line; //stores current command

    while (getline(inputFile, line)) { //read commands from file

        if (line.substr(0, 6) == "EMAIL ") { //if email command

            // Remove "EMAIL "
            string emailInfo = line.substr(6); //remove EMAIL prefix

            // Find the commas
            int firstComma = emailInfo.find(','); //find comma after sender
            int secondComma = emailInfo.find(',', firstComma + 1); //find comma after subject

            string sender =
                emailInfo.substr(0, firstComma); //get sender

            string subject =
                emailInfo.substr(firstComma + 1,
                                 secondComma - firstComma - 1); //get subject

            string date =
                emailInfo.substr(secondComma + 1); //get date

            //CHANGE FOR INVALID DATES : ChatGPT
            try { //try to make valid email
                Email email(sender, subject, date); //make email and check date
                inbox.add(email); //add email to heap
            } catch (const invalid_argument& error) { //if invalid date
                cerr << error.what() << "; email skipped.\n"; //show date error
            } //end of catch
        } //end ofemail command

        else if (line == "NEXT") { //if next command
            //CHANGE FOR PRINTING IN MAIN : ChatGPT
            const Email* email = inbox.next(); //get next email
            if (email == nullptr) { //if empty
                cout << "There are no emails to read.\n\n"; //empty message
            } else { //if email isnt empty, print email
                cout << "Next email:\n"; //print label
                cout << "\tSender: " << email->getSender() << '\n'; //print sender ADDED \t
                cout << "\tSubject: " << email->getSubject() << '\n'; //print subject ADDED \t
                cout << "\tDate: " << email->getDate() << "\n\n"; //print date ADDED \t
            } //end of email print
        } //end of next

        else if (line == "READ") { //if read command
            inbox.read(); //remove highest prio email
        } //end of read

        else if (line == "COUNT") { //if count command
            //CHANGE FOR PRINTING IN MAIN : ChatGPT
            cout << "There are " << inbox.count() << " emails to read.\n\n"; //print count
        } //end of count
    } //end of commands

    inputFile.close(); //close file

    return 0; //end program
} //end of program