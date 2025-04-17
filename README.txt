
# Railway Reservation System (C++)

This project is a simple C++ application for a railway reservation system. It interacts with a MySQL database to manage train reservations. This README provides instructions on how to compile, build, and run the project on a macOS system.

---

## **Steps to Compile and Run the Project**

### 1. **Clean the Output Directory**
Before compiling the project, ensure that the output directory is clean. You can remove any previously compiled files using the following command:

```bash
rm -rf /Users/giangdinh/Downloads/TrainRes/output/*
```

This command will remove all files in the `output` directory to ensure that the next compilation starts fresh.

### 2. **Compile the C++ Code**
To compile the C++ project, use the following command:

```bash
g++ -std=c++11 -Wall -Wextra -g3 /Users/giangdinh/Downloads/TrainRes/main.cpp -o /Users/giangdinh/Downloads/TrainRes/output/main -I/usr/local/mysql-connector-c++-8.0.20/include -L/usr/local/mysql-connector-c++-8.0.20/lib64 -L/usr/local/lib -lmysqlcppconn -lboost_system -lboost_filesystem -Wl,-rpath,/usr/local/lib
```

### **Explanation of the Compilation Command:**
- **`g++ -std=c++11`**: This tells the compiler to use C++11 standards.
- **`-Wall -Wextra`**: These flags enable additional warnings to help identify potential issues in your code.
- **`-g3`**: This flag includes debugging information to help in debugging the application using tools like `gdb`.
- **`/Users/giangdinh/Downloads/TrainRes/main.cpp`**: This specifies the path to the main C++ source code file to compile.
- **`-o /Users/giangdinh/Downloads/TrainRes/output/main`**: This specifies the output file for the compiled executable.
- **`-I/usr/local/mysql-connector-c++-8.0.20/include`**: This adds the include directory for MySQL connector to the project.
- **`-L/usr/local/mysql-connector-c++-8.0.20/lib64`**: This adds the library path for MySQL connector.
- **`-L/usr/local/lib`**: This adds another library path to ensure the libraries are correctly linked.
- **`-lmysqlcppconn`**: This links the MySQL C++ connector library.
- **`-lboost_system -lboost_filesystem`**: These link the Boost system and filesystem libraries.
- **`-Wl,-rpath,/usr/local/lib`**: This specifies the runtime library path.

### 3. **Set the DYLD Library Path (macOS specific)**
If you are using macOS, you need to set the `DYLD_LIBRARY_PATH` to ensure the system can find the required MySQL libraries at runtime. Run the following command:

```bash
export DYLD_LIBRARY_PATH=/usr/local/mysql-connector-c++-8.0.20/lib64:$DYLD_LIBRARY_PATH
```

This command adds the path of MySQL libraries to the dynamic linker’s search path, ensuring that the compiled program can access the MySQL connector libraries during execution.

### 4. **Run the Application**
After compiling the project and setting the library path, you can run the application using the following command:

```bash
/Users/giangdinh/Downloads/TrainRes/output/main
```

This will execute the compiled program, which starts the Railway Reservation System. You should now be able to interact with the application based on the menu options provided in the code.

---

## **Summary of Commands**

### **Clean Output Directory**
```bash
rm -rf /Users/giangdinh/Downloads/TrainRes/output/*
```
- This removes all compiled files in the output directory.

### **Compile the C++ Code**
```bash
g++ -std=c++11 -Wall -Wextra -g3 /Users/giangdinh/Downloads/TrainRes/main.cpp -o /Users/giangdinh/Downloads/TrainRes/output/main -I/usr/local/mysql-connector-c++-8.0.20/include -L/usr/local/mysql-connector-c++-8.0.20/lib64 -L/usr/local/lib -lmysqlcppconn -lboost_system -lboost_filesystem -Wl,-rpath,/usr/local/lib
```
- This command compiles the C++ code with necessary flags for debugging, warnings, and library paths.

### **Set DYLD Library Path (macOS)**
```bash
export DYLD_LIBRARY_PATH=/usr/local/mysql-connector-c++-8.0.20/lib64:$DYLD_LIBRARY_PATH
```
- Sets the library path to include MySQL C++ connector on macOS systems.

### **Run the Application**
```bash
/Users/giangdinh/Downloads/TrainRes/output/main
```
- Executes the compiled C++ program.

---

## **Additional Notes**
- Ensure that you have MySQL and Boost libraries installed on your system before attempting to compile the project.
- The project assumes the MySQL connector is installed at `/usr/local/mysql-connector-c++-8.0.20/`. If it’s located elsewhere, adjust the paths accordingly in the compilation and runtime commands.

---
