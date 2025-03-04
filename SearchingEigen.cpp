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
#include <Eigen/Dense>

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
            << std::setw(4) << year << "-"
            << std::setw(2) << month << "-"
            << std::setw(2) << day;
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
    double similarity;
    std::vector<Date> dates;
    Eigen::VectorXd sampleEigenvalues;
    Eigen::VectorXd subSeriesEigenvalues;
};

Eigen::VectorXd calculateEigenvalues(const std::vector<double>& values) {
    Eigen::MatrixXd matrix(values.size(), 1);
    for (size_t i = 0; i < values.size(); ++i) {
        matrix(i, 0) = values[i];
    }

    Eigen::MatrixXd correlationMatrix = matrix.transpose() * matrix;
    Eigen::EigenSolver<Eigen::MatrixXd> es(correlationMatrix);
    return es.eigenvalues().real();
}

double eigenvectorSimilarity(const Eigen::VectorXd& vec1, const Eigen::VectorXd& vec2) {
    if (vec1.size() != vec2.size()) {
        return 0.0;
    }
    return vec1.dot(vec2) / (vec1.norm() * vec2.norm());
}

std::vector<Match> findMatches(const TimeSeries& data, const TimeSeries& sample) {
    std::vector<Match> matches;
    Eigen::VectorXd sampleEigenvalues = calculateEigenvalues(sample.values);

    for (size_t i = 0; i <= data.values.size() - sample.values.size(); ++i) {
        std::vector<double> subSeries(data.values.begin() + i, data.values.begin() + i + sample.values.size());
        Eigen::VectorXd subSeriesEigenvalues = calculateEigenvalues(subSeries);

        double similarity = eigenvectorSimilarity(sampleEigenvalues, subSeriesEigenvalues);
        if (std::isnan(similarity)) similarity = -1;

        if (similarity > 0.9) {
            std::vector<Date> matchDates(data.dates.begin() + i, data.dates.begin() + i + sample.values.size());
            matches.push_back({i, subSeries, similarity, matchDates, sampleEigenvalues, subSeriesEigenvalues});
        }
    }

    std::sort(matches.begin(), matches.end(),
              [](const Match& a, const Match& b) { return a.similarity > b.similarity; });

    return matches;
}

void saveEigenvaluesToFile(const Match& match, const std::string& sampleFilename, const std::string& dataFilename, size_t matchNumber) {
    std::string outputDirectory = "C:/Users/Eliza/Desktop/dev/sm/Data_searhing_testing";
    std::filesystem::create_directories(outputDirectory);

    std::stringstream filenameStream;
    filenameStream << outputDirectory << "Eigenvalues_" << sampleFilename << "_" << dataFilename << "_Match" << matchNumber + 1 << ".txt";
    std::string filename = filenameStream.str();

    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error creating file " << filename << std::endl;
        return;
    }

    outFile << "Sample Eigenvalues:\n";
    for (int i = 0; i < match.sampleEigenvalues.size(); ++i) {
        outFile << match.sampleEigenvalues(i) << "\n";
    }

    outFile << "\nSub-Series Eigenvalues:\n";
    for (int i = 0; i < match.subSeriesEigenvalues.size(); ++i) {
        outFile << match.subSeriesEigenvalues(i) << "\n";
    }

    outFile.close();
    std::cout << "Eigenvalues saved to " << filename << std::endl;
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
    std::cout << "\nSimilarity: " << match.similarity << "\n\n";
}

int main() {
    try {
        const std::string sampleDirectory = "C:/Users/Eliza/CLionProjects/Data_searhing_testing/Sample";
        const std::string dataDirectory = "C:/Users/Eliza/CLionProjects/Data_searhing_testing/Close";
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

                        auto matches = findMatches(closeData, sampleData);

                        if (matches.empty()) {
                            std::cout << "No matches found\n";
                            continue;
                        }

                        size_t numMatchesToPrint = std::min(maxMatches, matches.size());
                        std::cout << "Top " << numMatchesToPrint << " Matches \n";

                        for (size_t i = 0; i < numMatchesToPrint; ++i) {
                            printMatch(matches[i], i);
                            saveEigenvaluesToFile(matches[i], sampleEntry.path().filename().string(), dataEntry.path().filename().string(), i);
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing data file "


                        << dataEntry.path()
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
