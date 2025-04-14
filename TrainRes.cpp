// TrainRes.cpp - macOS (M1 포함) 호환 전체 구현
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void pause() {
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
    aread();
    printf("\nTrain Info:\nSI\tTrain#\tName\tStart\tDest\tPrice\tSeat\tTime\n");
    for (int i = 0; i < k; i++) {
        printf("%s\t%s\t%s\t%s\t%s\t%s\t%d\t%s\n",
               add[i].si, add[i].train_number, add[i].train_name, add[i].start, add[i].destination, add[i].price, add[i].seat, add[i].time);
    }
    pause();
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
        printf("1. View Info\n2. Book Ticket\n3. Cancel Ticket\n4. Admin\n5. Exit\nChoice: ");
        scanf("%d", &ch); getchar();
        if (ch == 1) viewinfo();
        else if (ch == 2) bookticket();
        else if (ch == 3) cancelticket();
        else if (ch == 4) password();
        else if (ch == 5) break;
        else { printf("Invalid.\n"); pause(); }
    }
    return 0;
}
