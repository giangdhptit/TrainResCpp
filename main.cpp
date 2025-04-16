#include <iostream>
#include <stdexcept>
#include <string>
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/mysql_driver.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/mysql_connection.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/statement.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/resultset.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/prepared_statement.h"
#include <memory> // For std::unique_ptr, std::shared_ptr

using namespace std;

bool verifyPassword(const string& inputPassword, const string& hashFromDB) {
    return inputPassword == hashFromDB;  // SIMULATION ONLY
}

void createUser(sql::Connection* con, const string& username, const string& password, const string& email, const string& role) {
    try {
        string fakeHashedPassword = password;

        // Use unique_ptr to manage pstmt
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO users (username, password_hash, email, u_role) VALUES (?, ?, ?, ?)")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, fakeHashedPassword);
        pstmt->setString(3, email);
        pstmt->setString(4, role);

        pstmt->executeUpdate();
        cout << "✅ User created successfully!" << endl;
    } catch (sql::SQLException& e) {
        cerr << "❌ Failed to create user: " << e.what() << endl;
    }
}

bool login(sql::Connection* con, const string& username, const string& password) {
    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT password_hash, u_role FROM users WHERE username = ? LIMIT 1")
        );
        pstmt->setString(1, username);

        // Use unique_ptr for res
        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            string hashFromDB = res->getString("password_hash");
            string role = res->getString("u_role");

            if (verifyPassword(password, hashFromDB)) {
                cout << "✅ Login successful. Role: " << role << endl;
                return true;
            } else {
                cout << "❌ Incorrect password." << endl;
            }
        } else {
            cout << "❌ User not found." << endl;
        }
    } catch (sql::SQLException& e) {
        cerr << "Authentication error: " << e.what() << endl;
    }
    return false;
}

// Function to handle user login
bool handleUserLogin(sql::Connection* con, string& username, string& password) {
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    return login(con, username, password);  // Assuming login() is already defined
}

// Function to handle user registration
void handleUserRegistration(sql::Connection* con, string& username, string& password, string& email) {
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter email: ";
    cin >> email;
    createUser(con, username, password, email, "user");  // Assuming createUser() is already defined
}

// Function to handle admin login
bool handleAdminLogin(sql::Connection* con, string& username, string& password) {
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    return login(con, username, password);  // Assuming login() is already defined
}

// Function to handle ticket options after login
void handleTicketOptions() {
    int ticket_option;
    cout << "\n1. Reserve Ticket\n2. Cancel Ticket\nChoice: ";
    cin >> ticket_option;

    if (ticket_option == 1) {
        // bookticket();
    } else if (ticket_option == 2) {
        // cancelticket();
    } else {
        cout << "Invalid option.\n";
        pause();  // Assuming pause() is defined elsewhere
    }
}

const std::string DB_HOST = "tcp://localhost:3306";
const std::string DB_USER = "robin";
const std::string DB_PASS = "!";
const std::string DB_NAME = "trainres";

void handleMenuOption(int ch, sql::Connection* con) {
    string username, password, email;
    int ch1;

    switch (ch) {
        case 1: // View train info
            //viewTrainInfo();
            break;

        case 2: // Search trains
            //searchTrains();
            break;

        case 3: // User login or registration
            cout << "Please select: \n";
            cout << "1. I already have an account.\n";
            cout << "2. Register. \n";
            cout << "Choice: ";
            cin >> ch1;

            if (ch1 == 1) {
                // User Login
                if (handleUserLogin(con, username, password)) {
                    handleTicketOptions();
                } else {
                    cout << "Please login to proceed.\n";
                    pause();
                }
            } else {
                // Register new user
                handleUserRegistration(con, username, password, email);
            }
            break;

        case 4: // Admin login
            if (handleAdminLogin(con, username, password)) {
                // Admin functionality
            } else {
                cout << "Admin login failed. Please try again.\n";
                pause();
            }
            break;

        case 5: // Exit
            break;

        default:
            cout << "Invalid choice.\n";
            pause();
            break;
    }
}

int main() {
    int ch;
    std::string username, password, email;
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    try {
        driver = sql::mysql::get_mysql_driver_instance();
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(DB_HOST, DB_USER, DB_PASS);
        if (con->isValid()) {
            cout << "Connection successful!" << endl;
        }
        con->setSchema("train_res_cpp");
        while (1) {
            system("clear");
            cout << "\nRAILWAY RESERVATION SYSTEM\n";
            cout << "1. View Train Info\n"; //everybody
            cout << "2. Search Trains\n"; //everybody - if user choose 1 train -> go to reserve function
            cout << "3. Reserve/ Cancel Ticket"; //user page
            cout << "4. Admin Login\n"; //admin page
            cout << "5. Exit\n";
            cout << "Choice: ";
            cin >> ch;

            handleMenuOption(ch, con);  // Call the function to handle menu options
            if (ch == 5) break;  // Exit the loop when ch is 5
        }
        // No need for manual delete, smart pointers will handle cleanup automatically

        delete con;  // You still need to delete the connection manually if it's not a smart pointer

    } catch (sql::SQLException& e) {
        cerr << "MySQL error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
