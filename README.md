# Spreadsheet
This project involves the development of a custom spreadsheet application using C++ and ANTLR. The primary goal of this project is to create a flexible and functional spreadsheet tool that can parse, evaluate, and manage mathematical formulas and expressions, as well as text-based content in cells.
# Descripiton
This console-based spreadsheet application is purpose-built for educational use, offering practical hands-on experience in data management and mathematical computations. It features advanced formula parsing and evaluation, robust exception management, and structured data organization. While not intended for real-world applications, this project serves as a valuable educational tool, providing a simplified yet comprehensive platform for individuals interested in deepening their understanding of data management and computational mathematics within an educational context.

**Key Features and Implementation**:

1. **Advanced Formula Handling**: The project excels in parsing and evaluating mathematical formulas. It efficiently manages basic arithmetic operations, operator precedence within parentheses, and variable referencing, offering an educational platform for in-depth exploration of mathematical concepts.

2. **Exception Management**: Robust exception management ensures data integrity in various scenarios, such as syntactical errors and circular dependencies. This educational project offers practical insights into effective error handling.

3. **Structured Data Organization**: Structured table management promotes data organization principles and best practices. This approach encourages logical data structuring, a fundamental aspect of data management.

# Spreadsheet User Guide

### Building the Project

1. **Clone the Repository**: Clone the spreadsheet repository to your local machine if you haven't already:

   ```bash
   git clone https://github.com/jeanedit/cpp-spreadsheet.git
   ```

2. **Place ANTLR Files**:
   Copy the ANTLR JAR file (`antlr-x.xx-complete.jar`) and the `antlr-cpp-runtime-x.x.x-source` directory to the root folder of your spreadsheet project.

4. **Generate C++ Files:**
   Use ANTLR to generate the C++ files from your Formula.g4 grammar file. Open your terminal or command prompt and run the following command:

   ```
   antlr4 -Dlanguage=Cpp -o spreadsheet_directory Formula.g4
   ```
   - Make sure you run this command in the directory where your Formula.g4 file is located.

5. **Configure the project**:

   - Open your terminal and navigate to the project directory.

   - Create a `build` directory for out-of-source builds:

     ```bash
     mkdir build
     cd build
     ```

   - Run CMake to configure the project:

     ```bash
     cmake ..
     ```

6. **Build the Project**:

   - After configuring the project with CMake, build the project using the following command:

     ```bash
     cmake --build .
     ```

   This command will compile the source code and generate the executable for the spreadsheet application.

### Creating a Spreadsheet and Setting Cells:

1. **Create a Spreadsheet Instance**:

   To work with the spreadsheet, you'll need to create an instance of the `SheetInterface` or its implementation, named `Sheet`. You can do this as follows:

   ```cpp
   SheetInterface* sheet = CreateSheet();
   ```

   This creates a new spreadsheet and returns a pointer to the `SheetInterface` for you to work with.

2. **Setting Cell Values**:

   To set the value of a cell, use the `SetCell` method on the spreadsheet. You'll need to provide a `Position` and the cell's value (which can be a string, number, or formula):

   ```cpp
   sheet->SetCell("A1"_pos, "=2");
   sheet->SetCell("B2"_pos, "=A1 * 2");  // You can also set a formula.
   ```

   In this example, we set the value of cell A1 to a number and cell B2 to a formula that multiplies the value in cell A1 by 2.

3. **Getting Cell Values**:

   To retrieve the value of a cell, you can use the `GetCell` method:

   ```cpp
   CellInterface* cellA1 = sheet->GetCell("A1"_pos);
   ```

   This retrieves a pointer to the cell at position A1. You can access the cell's value and other properties through this pointer.

4. **Working with Cell Values**:

   The `CellInterface` can represent different types of cell values. You can retrieve the value as a string, number, or formula result based on what you set:

   ```cpp
   // Get the cell value as a string.
   std::string valueA1 = cellA1->GetText();

   // Get the cell value as a number (if it's a valid number).
   double numberB2 = std::get<double>(sheet->GetCell("B2"_pos)->GetValue());
   ```

   The `GetValue` method returns the cell's value in a variant format, allowing you to work with different types.

5. **Using Formulas**:

   As shown in the example above, you can set cell values to formulas. The code you provided includes tests for formula evaluation, so you can be confident that formulas are handled correctly.

6. **Clearing Cells**:

   To clear the contents of a cell, use the `ClearCell` method:

   ```cpp
   sheet->ClearCell("A1"_pos);
   ```

   This will remove the value from cell A1.

7. **Handling Invalid Positions**:

   The code includes error handling for invalid positions. You can add your own error handling to deal with invalid inputs gracefully.

**Example Spreadsheet Data**:

Suppose you have the following data in your spreadsheet:

|    |  A  |  B  |  C  |
|--- |---  |---  |---  |
|  1 |  42 |     |     |
|  2 |     |  =A1 |     |
|  3 |  10 |  =A1 + B2 |  =B3 |
|  4 |     |     |     |

**Example Output from `PrintTexts`**:

```
   42
        =A1
   10   =A1+B2 =B3

```

In this representation, you can see the cell values and formulas. The row and column headers indicate the cell position, and the contents of the cells are displayed beneath the corresponding headers.

**Example Output from `PrintValues`**:

```
   42
         42
   10    52    52

```

In this representation generated by `PrintValues`, only the calculated values of the formulas are displayed. The cell references have been resolved to their actual values.

These representations are simplified, and you can format them as desired to match your application's needs. The `PrintTexts` and `PrintValues` methods provide a convenient way to visualize the contents of your spreadsheet for debugging or user interface purposes.

### Running the Provided Tests:

Before using the spreadsheet for your specific application, it's a good idea to run the provided unit tests to ensure that the basic functionality is working correctly. The code includes tests for various aspects of the spreadsheet, such as formulas and cell references.

# System requirements
1. **CMake**: CMake is used to build the project. You can download it from [CMake's official website](https://cmake.org/download/).
2. **C++ Compiler**: You need a C++ compiler that supports C++17 or higher. If you're using Linux, you likely have `g++` installed. On Windows, you can use MinGW or Visual Studio's C++ compiler.
3. **ANTLR-4.13**: The project uses ANTLR-4.13 for parsing. You can download ANTLR from [ANTLR's official website](https://www.antlr.org/download/antlr-4.9.2-complete.jar). Make sure you have Java installed to run ANTLR.   
# Development plans:
1. Creating a simple user interface that resembles Excel or Google Spreadsheet.
2. Adding new operations, such as exponentiation and square root extraction.
