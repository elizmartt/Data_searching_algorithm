# Time Series Similarity Search with Eigenvalues

This project implements a time series similarity search algorithm using eigenvalues and eigenvector similarity. It reads time series data from CSV files, calculates eigenvalues, and identifies similar patterns between a sample time series and a larger dataset.

## Overview

The project consists of the following components:

* **Data Handling:**
    * Reads time series data from CSV files, including dates and values.
    * Parses date strings and converts string values to doubles.
* **Eigenvalue Calculation:**
    * Calculates the eigenvalues of time series data using the Eigen library.
    * Uses a simplified correlation matrix (A^T * A) for eigenvalue calculation on single-column time series data. **Note:** This method results in a single eigenvalue representing the sum of squared values and is a simplified approach for comparison purposes.
* **Eigenvector Similarity:**
    * Calculates the cosine similarity between eigenvalue vectors to measure the similarity between time series patterns.
* **Time Series Matching:**
    * Finds sub-sequences in a larger time series that are similar to a sample time series based on eigenvalue similarity.
    * Sorts the matches by similarity.
* **Output:**
    * Prints the matched time series patterns, including dates, values, and similarity scores.
    * Saves the sample and sub-series eigenvalues to a text file.
* **Alternative C++ Implementation:**
    * Includes a C++ file that performs time series similarity search without using eigenvalues. This provides an alternative method for comparison.
* **Python Implementation:**
    * Contains a Python project that searches for one time series data within another. This provides a Python-based solution for time series similarity search.
* **CSV Data:**
    * Includes example CSV files for testing and demonstration purposes.

## Dependencies

* **C++ Compiler:** A C++17 or later compatible compiler (e.g., g++).
* **CMake:** For building the C++ project.
* **Eigen Library:** For matrix and vector operations in the eigenvalue-based C++ implementation.
* **Standard C++ Libraries:** `<iostream>`, `<fstream>`, `<string>`, `<vector>`, `<algorithm>`, `<cmath>`, `<limits>`, `<iomanip>`, `<ctime>`, `<sstream>`, `<memory>`, `<filesystem>`, `<map>`.
* **Python 3.x:** For running the Python project.
* **Pandas:** For data manipulation in the Python project.
* **Matplotlib:** For plotting data in the Python project.

## How to Build and Run

1.  **Clone the Repository:**

    ```bash
    git clone [repository_url]
    cd [repository_directory]
    ```

2.  **C++ Project (Eigenvalue-based):**
    * **Download and Place Eigen:**
        * Download the Eigen library from [http://eigen.tuxfamily.org/](http://eigen.tuxfamily.org/).
        * Place the Eigen directory into the project directory, or into a subdirectory such as lib.
        * If using CLion, then place the eigen directory in the project root.
    * **Configure CMake:**
        * Create a `CMakeLists.txt` file in the project's root directory.
        * Add the following lines to your `CMakeLists.txt` file, adjusting the path to Eigen as needed:

            ```cmake
            cmake_minimum_required(VERSION 3.30)
            project(TimeSeriesSimilarity)

            set(CMAKE_CXX_STANDARD 20)
            include_directories(eigen) # or include_directories(lib/eigen)
            add_executable(TimeSeriesSimilarity main.cpp)
            ```

        * If you are using CLion, then reload the CMake project.
    * **Build the Project:**
        * Using CMake, generate build files and build the project.
        * In CLion, use the build button.
        * From the command line:

            ```bash
            mkdir build
            cd build
            cmake ..
            make
            ```

    * **Run the Executable:**
        * Run the generated executable.
        * The program will read CSV files from the specified sample and data directories and output the results to the console and to text files in the output directory.

3.  **C++ Project (Without Eigenvalues):**
    * Compile the alternative C++ file using your preferred C++ compiler.
    * Run the generated executable.

4.  **Python Project:**
    * Navigate to the Python project directory.
    * Install the required Python packages:

        ```bash
        pip install pandas matplotlib
        ```

    * Run the Python script:

        ```bash
        python your_python_script.py
        ```

5.  **CSV Files:**
    * Place your CSV files in the appropriate directories as required by the C++ and Python projects.

## CSV File Format

The CSV files should have the following format:

```csv
Date,Value
YYYY-MM-DD,value1
YYYY-MM-DD,value2
...

