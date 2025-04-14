#include <iostream>
#include <stdexcept>
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/mysql_driver.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/mysql_connection.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/statement.h"
#include "/usr/local/mysql-connector-c++-8.0.20/include/jdbc/cppconn/resultset.h"
#include <memory> // For std::unique_ptr, std::shared_ptr
int main() {
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;
        sql::Statement* stmt;
        sql::ResultSet* res;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "123456a@");
        if (con->isValid()) {
            std::cout << "Connection successful!" << std::endl;
        }
        con->setSchema("sms");

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM sms.contacts WHERE contact_email = 'vinouye@aol.com'");

        while (res->next()) {
            std::cout << res << std::endl;
        }

        delete res;
        delete stmt;
        delete con;

    } catch (sql::SQLException& e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
