#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <memory>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

class Date {
public:
    int year, month, day;

    Date(const std::string& dateStr) {
        std::stringstream ss(dateStr);
        char dash;
        ss >> year >> dash >> month >> dash >> day;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid date format " + dateStr);
        }
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(2) << year << "-"
            << std::setw(2) << month << "-"
            << std::setw(4) << day;
        return oss.str();
    }
};

struct TimeSeries {
    std::vector<Date> dates;
    std::vector<double> values;
};

double stringToDouble(const std::string& str) {
    std::string cleanedStr = str;
    cleanedStr.erase(std::remove(cleanedStr.begin(), cleanedStr.end(), ','), cleanedStr.end());
    try {
        return std::stod(cleanedStr);
    } catch (const std::invalid_argument& e) {
        throw std::invalid_argument("Error converting to double " + str);
    }
}

TimeSeries readCSVFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open file " + filename);
    }

    TimeSeries ts;
    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string dateStr, valueStr;

        if (std::getline(ss, dateStr, ',') && std::getline(ss, valueStr)) {
            try {
                ts.dates.push_back(Date(dateStr));
                ts.values.push_back(stringToDouble(valueStr));
            } catch (const std::exception& e) {
                std::cerr << "Error for lines " << line << "\n" << e.what() << std::endl;
                continue;
            }
        }
    }
    return ts;
}

struct Match {
    size_t startIndex;
    std::vector<double> values;
    double totalDifference;
    std::vector<Date> dates;
};

std::vector<Match> findMatches(const TimeSeries& data, const TimeSeries& sample, double tolerance) {
    std::vector<Match> matches;

    for (size_t i = 0; i <= data.values.size() - sample.values.size(); ++i) {
        double totalDifference = 0.0;
        bool isMatch = true;
        std::vector<double> subSeries;

        for (size_t j = 0; j < sample.values.size(); ++j) {
            double diff = std::abs(data.values[i + j] - sample.values[j]);
            if (diff > tolerance) {
                isMatch = false;
                break;
            }
            totalDifference += diff;
            subSeries.push_back(data.values[i + j]);
        }

       // efvrygylgly
        if (isMatch) {
            std::vector<Date> matchDates;
            for (size_t j = 0; j < sample.values.size(); ++j) {
                matchDates.push_back(data.dates[i + j]);
            }
            matches.push_back({i, subSeries, totalDifference, matchDates});
        }
    }


    std::sort(matches.begin(), matches.end(),
              [](const Match& a, const Match& b) { return a.totalDifference < b.totalDifference; });

    return matches;
}

void printMatch(const Match& match, size_t matchNumber) {
    std::cout << "Match " << matchNumber + 1 << "\n";
    std::cout << "Dates \n";
    for (const auto& date : match.dates) {
        std::cout << date.toString() << " ";
    }
    std::cout << "\nClose Values \n ";
    for (const auto& value : match.values) {
        std::cout << std::fixed << std::setprecision(2) << value << " ";
    }
    std::cout << "\nTotal Difference: " << match.totalDifference << "\n\n";
}

int main() {
    try {
        const std::string sampleDirectory = "/Data_searhing_testing/Sample";
        const std::string dataDirectory = "/Data_searhing_testing/Close";
        const double tolerance = 5000.0;
        const size_t maxMatches = 10;


        for (const auto& sampleEntry : fs::directory_iterator(sampleDirectory)) {
            if (!sampleEntry.is_regular_file() || sampleEntry.path().extension() != ".csv") {
                continue;
            }

            try {
                TimeSeries sampleData = readCSVFile(sampleEntry.path().string());


                for (const auto& dataEntry : fs::directory_iterator(dataDirectory)) {
                    if (!dataEntry.is_regular_file() || dataEntry.path().extension() != ".csv") {
                        continue;
                    }

                    try {
                        TimeSeries closeData = readCSVFile(dataEntry.path().string());

                        std::cout << "\nAnalyzing matches between\n"
                                 << "Sample: " << sampleEntry.path().filename() << "\n"
                                 << "Data: " << dataEntry.path().filename() << "\n";

                        auto matches = findMatches(closeData, sampleData, tolerance);

                        if (matches.empty()) {
                            std::cout << "No matches found\n";
                            continue;
                        }

                        size_t numMatchesToPrint = std::min(maxMatches, matches.size());
                        std::cout << "Top " << numMatchesToPrint << " Matches \n";

                        for (size_t i = 0; i < numMatchesToPrint; ++i) {
                            printMatch(matches[i], i);
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing data file " << dataEntry.path()
                                 << ": " << e.what() << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing sample file " << sampleEntry.path()
                         << ": " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << " error " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
