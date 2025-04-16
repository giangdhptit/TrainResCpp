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

const std::string DB_HOST = "tcp://localhost:3306";
const std::string DB_USER = "robin";
const std::string DB_PASS = "robin123!";
const std::string DB_NAME = "trainres";

// Declare admin functions
void adminMenu(sql::Connection* con);
void addTrain(sql::Connection* con);
void removeTrain(sql::Connection* con);
void seeTrainList(sql::Connection* con);
void manageMember(sql::Connection* con);

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

void pause_press() {
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void viewTrainInfo() {
    system("clear");
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;
        sql::Statement* stmt;
        sql::ResultSet* res;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(DB_HOST, DB_USER, DB_PASS);
        con->setSchema("trainres");

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM trains ORDER BY date, time");

        printf("\nTrain Info:\n");
        printf("\n——————————————————————————————————————————————————————————————————————————————\n");
        printf("%-4s %-15s %-6s %-6s %-8s %-6s %-8s %-12s\n",
               "ID", "Name", "Start", "Dest", "Price", "Seat", "Time", "Date");
        printf("——————————————————————————————————————————————————————————————————————————————\n");

        while (res->next()) {
            printf("%-4d %-15s %-6s %-6s %-8s %-6d %-8s %-12s\n",
                res->getInt("train_id"),
                res->getString("train_name").c_str(),
                res->getString("start").c_str(),
                res->getString("destination").c_str(),
                res->getString("price").c_str(),
                res->getInt("seat"),
                res->getString("time").c_str(),
                res->getString("date").c_str()
            );
        }
        printf("——————————————————————————————————————————————————————————————————————————————\n");

        delete res;
        delete stmt;
        delete con;

    } catch (sql::SQLException& e) {
        printf("MySQL Error: %s\n", e.what());
    }

    pause_press();
}

void search_trains() {
    system("clear");

    char start[50] = "", destination[50] = "", date[20] = "";
    char time_input[10] = "";
    char min_price_input[10] = "";
    char max_price_input[10] = "";

    bool use_time = false, use_min_price = false, use_max_price = false;

    printf("🔎 Find trains that match your journey!\n");
    printf("(* Departure station, Destination, and Date are required)\n\n");

    // 입력
    clearInputBuffer();
    printf("• Departure Station (e.g. PAR): ");
    fgets(start, sizeof(start), stdin);
    start[strcspn(start, "\n")] = 0;
    
    printf("• Destination Station (e.g. LYO): ");
    fgets(destination, sizeof(destination), stdin);
    destination[strcspn(destination, "\n")] = 0;

    printf("• Date (YYYY-MM-DD): ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = 0;

    printf("• Earliest Time (HH:MM, press enter to skip): ");
    fgets(time_input, sizeof(time_input), stdin);
    if (strcmp(time_input, "\n") != 0 && strlen(time_input) > 1) {
        time_input[strcspn(time_input, "\n")] = '\0';
        use_time = true;
    }

    printf("• Min Price (press enter to skip): ");
    fgets(min_price_input, sizeof(min_price_input), stdin);
    if (strcmp(min_price_input, "\n") != 0 && strlen(min_price_input) > 1) {
        min_price_input[strcspn(min_price_input, "\n")] = '\0';
        use_min_price = true;
    }

    printf("• Max Price (press enter to skip): ");
    fgets(max_price_input, sizeof(max_price_input), stdin);
    if (strcmp(max_price_input, "\n") != 0 && strlen(max_price_input) > 1) {
        max_price_input[strcspn(max_price_input, "\n")] = '\0';
        use_max_price = true;
    }

    // 필수 입력 검증
    if (strlen(start) == 0) {
        printf("\n[Departure Station] is required.\n");
        pause_press();
        return;
    }
    if (strlen(destination) == 0) {
        printf("\n[Destination Station] is required.\n");
        pause_press();
        return;
    }
    if (strlen(date) == 0) {
        printf("\n[Date] is required.\n");
        pause_press();
        return;
    }

    // 입력 요약 출력
    printf("\n Your Search Criteria:\n");
    printf("• Departure: %s\n", start);
    printf("• Destination: %s\n", destination);
    printf("• Date: %s\n", date);
    printf("• Time After: %s\n", use_time ? time_input : "(any)");
    printf("• Min Price: %s\n", use_min_price ? min_price_input : "(any)");
    printf("• Max Price: %s\n", use_max_price ? max_price_input : "(any)\n");

    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(DB_HOST, DB_USER, DB_PASS);
        con->setSchema("trainres");

        std::string query = "SELECT * FROM trains WHERE start = ? AND destination = ? AND date = ?";
        if (use_time) query += " AND time >= ?";
        if (use_min_price) query += " AND CAST(price AS UNSIGNED) >= ?";
        if (use_max_price) query += " AND CAST(price AS UNSIGNED) <= ?";
        query += " ORDER BY time";

        pstmt = con->prepareStatement(query);

        int paramIndex = 1;
        pstmt->setString(paramIndex++, start);
        pstmt->setString(paramIndex++, destination);
        pstmt->setString(paramIndex++, date);
        if (use_time) pstmt->setString(paramIndex++, time_input);
        if (use_min_price) pstmt->setInt(paramIndex++, atoi(min_price_input));
        if (use_max_price) pstmt->setInt(paramIndex++, atoi(max_price_input));

        res = pstmt->executeQuery();

        int count = 0;
        printf("\nSearch Results:\n");
        printf("——————————————————————————————————————————————————————————————————————————————\n");
        printf("%-4s %-15s %-6s %-6s %-8s %-6s %-8s %-12s\n",
               "ID", "Name", "Start", "Dest", "Price", "Seat", "Time", "Date");
        printf("——————————————————————————————————————————————————————————————————————————————\n");

        while (res->next()) {
            printf("%-4d %-15s %-6s %-6s %-8s %-6d %-8s %-12s\n",
                   res->getInt("train_id"),
                   res->getString("train_name").c_str(),
                   res->getString("start").c_str(),
                   res->getString("destination").c_str(),
                   res->getString("price").c_str(),
                   res->getInt("seat"),
                   res->getString("time").c_str(),
                   res->getString("date").c_str());
            count++;
        }

        if (count == 0) {
            printf("\n❗ No trains found for the given criteria.\n");
        }

        delete res;
        delete pstmt;
        delete con;

    } catch (sql::SQLException& e) {
        printf("MySQL Error: %s\n", e.what());
    }

    pause_press();
}

void reserveTicket(sql::Connection* con, const std::string& username) {
    string trainName;
    int seatsRequested;

    cout << "Enter the train name to reserve (e.g., TGV-101): ";
    cin >> trainName;

    cout << "Number of seats to reserve (e.g., 3): ";
    cin >> seatsRequested;
    clearInputBuffer();

    try {
        // Step 1: Check availability by train_name
        unique_ptr<sql::PreparedStatement> checkStmt(
            con->prepareStatement("SELECT train_id, seat, date FROM trains WHERE train_name = ?")
        );
        checkStmt->setString(1, trainName);
        unique_ptr<sql::ResultSet> res(checkStmt->executeQuery());

        if (res->next()) {
            int trainId = res->getInt("train_id");
            int availableSeats = res->getInt("seat");
            string trainDate = res->getString("date");

            if (seatsRequested > availableSeats) {
                cout << "❗ Not enough seats available.\n";
                return;
            }

            // Step 2: Reserve seats (with transaction)
            con->setAutoCommit(false);  

            // 2-1: Insert booking
            unique_ptr<sql::PreparedStatement> bookStmt(
                con->prepareStatement("INSERT INTO bookings (username, train_id, seat_count, date) VALUES (?, ?, ?, ?)")
            );
            bookStmt->setString(1, username);
            bookStmt->setInt(2, trainId);  // insert resolved ID
            bookStmt->setInt(3, seatsRequested);
            bookStmt->setString(4, trainDate);
            bookStmt->executeUpdate();

            // 2-2: Update train seat count
            unique_ptr<sql::PreparedStatement> updateTrain(
                con->prepareStatement("UPDATE trains SET seat = seat - ? WHERE train_id = ?")
            );
            updateTrain->setInt(1, seatsRequested);
            updateTrain->setInt(2, trainId);
            updateTrain->executeUpdate();

            con->commit();
            cout << "✅ Reservation successful!" << endl;

        } else {
            cout << "❌ Train not found.\n";
        }

    } catch (sql::SQLException& e) {
        con->rollback();
        cerr << "❌ Reservation error: " << e.what() << endl;
    }

    pause_press();
}


void cancelTicket(sql::Connection* con, const std::string& username) {
    int bookingId;
    cout << "Enter Booking ID to cancel: ";
    cin >> bookingId;

    try {
        con->setAutoCommit(false);  // transaction begin

        // Step 1: Get booking info
        unique_ptr<sql::PreparedStatement> getStmt(
            con->prepareStatement("SELECT train_id, seat_count FROM bookings WHERE id = ? AND username = ?")
        );
        getStmt->setInt(1, bookingId);
        getStmt->setString(2, username);
        unique_ptr<sql::ResultSet> res(getStmt->executeQuery());

        if (!res->next()) {
            cout << "❌ Booking not found or not your booking.\n";
            return;
        }

        int trainId = res->getInt("train_id");
        int seatCount = res->getInt("seat_count");

        // Step 2: Delete booking
        unique_ptr<sql::PreparedStatement> delStmt(
            con->prepareStatement("DELETE FROM bookings WHERE id = ?")
        );
        delStmt->setInt(1, bookingId);
        delStmt->executeUpdate();

        // Step 3: Restore seats
        unique_ptr<sql::PreparedStatement> updStmt(
            con->prepareStatement("UPDATE trains SET seat = seat + ? WHERE train_id = ?")
        );
        updStmt->setInt(1, seatCount);
        updStmt->setInt(2, trainId);
        updStmt->executeUpdate();

        con->commit();
        cout << "✅ Cancellation successful." << endl;

    } catch (sql::SQLException& e) {
        con->rollback();
        cerr << "❌ Cancellation error: " << e.what() << endl;
    }

    pause_press();
}


// void handleTicketOptions(sql::Connection* con, const std::string& username) {
void handleTicketOptions(sql::Connection* con, const std::string& username) {
    int ticket_option;
    cout << "\n1. Reserve Ticket\n2. Cancel Ticket\nChoice: ";
    cin >> ticket_option;

    if (ticket_option == 1) {
        reserveTicket(con, username);
    } else if (ticket_option == 2) {
        cancelTicket(con, username);
    } else {
        cout << "Invalid option.\n";
        pause_press();
    }
}


void handleMenuOption(int ch, sql::Connection* con) {
    string username, password, email;
    int ch1;

    switch (ch) {
        case 1: // View train info
            viewTrainInfo();
            break;

        case 2: // Search trains
            search_trains();
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
                    handleTicketOptions(con, username);
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
                adminMenu(con);
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

// Function to add a train
void addTrain(sql::Connection* con) {
    string trainName, start, destination, price, time;
    int seat;
    string date;

    // Get train details from admin
    cout << "Enter train name: ";
    cin >> trainName;
    cout << "Enter start station: ";
    cin >> start;
    cout << "Enter destination station: ";
    cin >> destination;
    cout << "Enter price: ";
    cin >> price;
    cout << "Enter available seats: ";
    cin >> seat;
    cout << "Enter train time (HH:MM): ";
    cin >> time;
    cout << "Enter date (YYYY-MM-DD): ";
    cin >> date;

    try {
        // Prepare SQL query to insert new train into the trains table
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO trains (train_name, start, destination, price, seat, time, date) VALUES (?, ?, ?, ?, ?, ?, ?)")
        );
        pstmt->setString(1, trainName);
        pstmt->setString(2, start);
        pstmt->setString(3, destination);
        pstmt->setString(4, price);
        pstmt->setInt(5, seat);
        pstmt->setString(6, time);
        pstmt->setString(7, date);

        pstmt->executeUpdate();
        cout << "Train added successfully!" << endl;
    } catch (sql::SQLException& e) {
        cerr << "Error adding train: " << e.what() << endl;
    }
}

// Function to remove a train
void removeTrain(sql::Connection* con) {
    string trainId;
    cout << "Enter train ID to remove: ";
    cin >> trainId;

    try {
        // Prepare SQL query to delete train from the trains table
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("DELETE FROM trains WHERE train_id = ?")
        );
        pstmt->setString(1, trainId);

        pstmt->executeUpdate();
        cout << "Train removed successfully!" << endl;
    } catch (sql::SQLException& e) {
        cerr << "Error removing train: " << e.what() << endl;
    }
}

// Function to see the reservation list
void seeTrainList(sql::Connection* con) {
    try {
        // SQL query to retrieve all trains
        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT train_id, train_name, start, destination, price, seat, time, date FROM trains")
        );

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        cout << "Train List:" << endl;
        while (res->next()) {
            cout << "Train ID: " << res->getInt("train_id")
                 << ", Train Name: " << res->getString("train_name")
                 << ", Start: " << res->getString("start")
                 << ", Destination: " << res->getString("destination")
                 << ", Price: " << res->getString("price")
                 << ", Seats Available: " << res->getInt("seat")
                 << ", Time: " << res->getString("time")
                 << ", Date: " << res->getString("date") << endl;
        }
    } catch (sql::SQLException& e) {
        cerr << "Error fetching train list: " << e.what() << endl;
    }
}

// Function to manage members (e.g., activate, deactivate)
void manageMember(sql::Connection* con) {
    string username;
    int action;
    cout << "Enter username to manage: ";
    cin >> username;
    cout << "Choose action: \n1. Activate account\n2. Deactivate account\nChoice: ";
    cin >> action;

    try {
        string query;
        if (action == 1) {
            query = "UPDATE users SET status = 'active' WHERE username = ?";
        } else if (action == 2) {
            query = "UPDATE users SET status = 'inactive' WHERE username = ?";
        } else {
            cout << "Invalid choice.\n";
            return;
        }

        unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(query)
        );
        pstmt->setString(1, username);

        pstmt->executeUpdate();
        cout << "Member account status updated successfully!" << endl;
    } catch (sql::SQLException& e) {
        cerr << "Error managing member: " << e.what() << endl;
    }
}

// Admin menu options
void adminMenu(sql::Connection* con) {
    int choice;
    do {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Train\n";
        cout << "2. Remove Train\n";
        cout << "3. See Train List\n";
        cout << "4. Manage Member\n";
        cout << "5. Logout\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addTrain(con);
                break;
            case 2:
                removeTrain(con);
                break;
            case 3:
                seeTrainList(con);
                break;
            case 4:
                manageMember(con);
                break;
            case 5:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 5);
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
        con->setSchema(DB_NAME);
        con->setSchema(DB_NAME);
        while (1) {
            system("clear");
            cout << "\n[ RAILWAY RESERVATION SYSTEM ]\n\n";
            cout << "1. View Train Info\n"; //everybody
            cout << "2. Search Trains\n"; //everybody - if user choose 1 train -> go to reserve function
            // cout << "3. Reserve/ Cancel Ticket\n"; //user page
            cout << "3. Reserve/ Cancel Ticket\n"; //user page
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
