// TrainRes.cpp - macOS (M1 포함) 호환 전체 구현
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <iostream>
#include <stdexcept>
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/mysql_driver.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/mysql_connection.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/statement.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/resultset.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/prepared_statement.h"
#include <memory> // For std::unique_ptr, std::shared_ptr

// 구조체 정의
struct adddata {
    char si[10];
    char train_number[10];
    char train_name[20];
    char start[10];
    char destination[10];
    char price[10];
    int seat;
    char time[10];
} add[1000];

struct bookticket {
    char train_number[20];
    char name[20];
    char phone[20];
    char date[20];
    int seat;
} book[1000];

const std::string DB_HOST = "tcp://localhost:3306";
const std::string DB_USER = "root";
const std::string DB_PASS = "pwd"; 
const std::string DB_NAME = "trainres";

int k = 0, u = 0;

// 함수 선언
void viewinfo();
void bookticket();
void cancelticket();
void admin();
void password();
void viewpassenger();
void addtrain();
void dlttrain();
void awrite();
void aread();
void bookticket_write();
void viewpassengers_read();

void pause_press() {
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

void awrite() {
    FILE* f = fopen("train_details.txt", "w");
    fwrite(&add, sizeof(add), 1, f);
    fclose(f);
    f = fopen("train_report.txt", "w");
    fwrite(&k, sizeof(k), 1, f);
    fclose(f);
}

void aread() {
    FILE* f = fopen("train_details.txt", "r");
    if (f) {
        fread(&add, sizeof(add), 1, f);
        fclose(f);
    }
    f = fopen("train_report.txt", "r");
    if (f) {
        fread(&k, sizeof(k), 1, f);
        fclose(f);
    }
}

void bookticket_write() {
    FILE* f = fopen("booklist.txt", "w");
    fwrite(&book, sizeof(book), 1, f);
    fclose(f);
    f = fopen("booklistreport.txt", "w");
    fwrite(&u, sizeof(u), 1, f);
    fclose(f);
}

void viewpassengers_read() {
    FILE* f = fopen("booklist.txt", "r");
    if (f) {
        fread(&book, sizeof(book), 1, f);
        fclose(f);
    }
    f = fopen("booklistreport.txt", "r");
    if (f) {
        fread(&u, sizeof(u), 1, f);
        fclose(f);
    }
}

void viewinfo() {
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
        printf("%-4s %-6s %-15s %-6s %-6s %-8s %-6s %-8s %-12s",
                "SI", "Train#", "Name", "Start", "Dest", "Price", "Seat", "Time", "Date");
        printf("\n——————————————————————————————————————————————————————————————————————————————\n");

        while (res->next()) {
            printf("%-4s %-6s %-15s %-6s %-6s %-8s %-6d %-8s %-12s\n",
                res->getString("si").c_str(),
                res->getString("train_number").c_str(),
                res->getString("train_name").c_str(),
                res->getString("start").c_str(),
                res->getString("destination").c_str(),
                res->getString("price").c_str(),
                res->getInt("seat"),
                res->getString("time").c_str(),
                res->getString("date").c_str()
            );
        }
        printf("\n——————————————————————————————————————————————————————————————————————————————\n");

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

    char start[10] = "", destination[10] = "", date[20] = "";
    char time_input[10] = "";
    char min_price_input[10] = "";
    char max_price_input[10] = "";

    bool use_time = false, use_min_price = false, use_max_price = false;

    printf("🔎 Find trains that match your journey!\n");
    printf("(* Departure station, Destination, and Date are required)\n\n");

    // 입력
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
        if (use_min_price) query += " AND price >= ?";
        if (use_max_price) query += " AND price <= ?";
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
        printf("%-4s %-6s %-15s %-6s %-6s %-8s %-6s %-8s %-12s\n",
               "SI", "Train#", "Name", "From", "To", "Price", "Seat", "Time", "Date");

        while (res->next()) {
            printf("%-4s %-6s %-15s %-6s %-6s %-8d %-6d %-8s %-12s\n",
                   res->getString("si").c_str(),
                   res->getString("train_number").c_str(),
                   res->getString("train_name").c_str(),
                   res->getString("start").c_str(),
                   res->getString("destination").c_str(),
                   res->getInt("price"),
                   res->getInt("seat"),
                   res->getString("time").c_str(),
                   res->getString("date").c_str());
            count++;
        }

        if (count == 0) {
            printf("\nNo trains found for the given criteria.\n");
        }

        delete res;
        delete pstmt;
        delete con;

    } catch (sql::SQLException& e) {
        printf("MySQL Error: %s\n", e.what());
    }

    pause_press();
}

void bookticket() {
    system("clear");
    int n, i, j, found;
    aread();
    printf("How many tickets? ");
    scanf("%d", &n); getchar();
    for (j = u; j < u + n; j++) {
        found = -1;
        printf("\nEnter Train#: "); scanf("%s", book[j].train_number); getchar();
        for (i = 0; i < k; i++) {
            if (strcmp(book[j].train_number, add[i].train_number) == 0 && add[i].seat > 0) {
                found = i;
                printf("Date: "); scanf("%s", book[j].date);
                printf("Name: "); scanf("%s", book[j].name);
                printf("Phone: "); scanf("%s", book[j].phone);
                book[j].seat = add[i].seat--;
                bookticket_write();
                awrite();
                break;
            }
        }
        if (found == -1) {
            printf("Train not found or no seats available.\n");
            pause(); return;
        }
    }
    u = j;
    pause();
}

void cancelticket() {
    viewpassengers_read();
    char phone[20];
    int found = -1;
    printf("Enter phone#: "); scanf("%s", phone); getchar();
    for (int i = 0; i < u; i++) {
        if (strcmp(book[i].phone, phone) == 0) {
            for (int j = i; j < u - 1; j++) {
                book[j] = book[j + 1];
            }
            u--;
            bookticket_write();
            printf("Cancelled successfully.\n");
            found = 1;
            break;
        }
    }
    if (found == -1) printf("Not found.\n");
    pause();
}

void password() {
    int pass;
    printf("Enter password: ");
    scanf("%d", &pass); getchar();
    if (pass == 1234567) {
        printf("Access granted.\n");
        pause();
        admin();
    } else {
        printf("Wrong password.\n");
        pause();
    }
}

void admin() {
    int ch;
    while (1) {
        system("clear");
        printf("\nADMIN MENU\n1. View Passengers\n2. Add Train\n3. Delete Train\n4. Back\nChoice: ");
        scanf("%d", &ch); getchar();
        if (ch == 1) viewpassenger();
        else if (ch == 2) addtrain();
        else if (ch == 3) dlttrain();
        else if (ch == 4) break;
        else { printf("Invalid.\n"); pause(); }
    }
}

void viewpassenger() {
    viewpassengers_read();
    printf("\nPassengers:\nTrain#\tName\tPhone\tDate\tSeat\n");
    for (int i = 0; i < u; i++) {
        printf("%s\t%s\t%s\t%s\t%d\n",
               book[i].train_number, book[i].name, book[i].phone, book[i].date, book[i].seat);
    }
    pause();
}

void addtrain() {
    int n;
    aread();
    printf("How many trains to add? ");
    scanf("%d", &n); getchar();
    for (int i = k; i < k + n; i++) {
        printf("Serial: "); scanf("%s", add[i].si);
        printf("Train#: "); scanf("%s", add[i].train_number);
        printf("Name: "); scanf("%s", add[i].train_name);
        printf("Start: "); scanf("%s", add[i].start);
        printf("Dest: "); scanf("%s", add[i].destination);
        printf("Price: "); scanf("%s", add[i].price);
        printf("Seats: "); scanf("%d", &add[i].seat);
        printf("Time: "); scanf("%s", add[i].time);
    }
    k += n;
    awrite();
    printf("Added successfully.\n");
    pause();
}

void dlttrain() {
    char num[10];
    int found = -1;
    aread();
    printf("Enter train# to delete: ");
    scanf("%s", num); getchar();
    for (int i = 0; i < k; i++) {
        if (strcmp(add[i].train_number, num) == 0) {
            for (int j = i; j < k - 1; j++) {
                add[j] = add[j + 1];
            }
            k--;
            awrite();
            printf("Deleted.\n");
            found = 1;
            break;
        }
    }
    if (found == -1) printf("Not found.\n");
    pause();
}

int main() {
    aread(); viewpassengers_read();
    int ch;
    while (1) {
        system("clear");
        printf("\nRAILWAY RESERVATION SYSTEM\n");
        printf("1. View Info\n2. Search trains\n3. Book Ticket\n4. Cancel Ticket\n5. Admin\n6. Exit\nChoice: ");
        scanf("%d", &ch); getchar();
        if (ch == 1) viewinfo();
        else if (ch == 2) search_trains();
        else if (ch == 3) bookticket();
        else if (ch == 4) cancelticket();
        else if (ch == 5) password();
        else if (ch == 6) break;
        else { printf("Invalid.\n"); pause(); }
    }
    return 0;
}
