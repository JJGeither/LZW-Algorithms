#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <sstream>

std::string removeExtension(std::string input) {
    int pos = input.find('.');
    if (pos != std::string::npos) {
        input = input.substr(0, pos);
    }
    return input;
}

bool compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream fs1(file1);
    std::ifstream fs2(file2);

    if (!fs1.is_open() || !fs2.is_open()) {
        std::cerr << "Error opening files for comparison\n";
        return false;
    }

    std::string line1, line2;
    while (std::getline(fs1, line1) && std::getline(fs2, line2)) {
        if (line1 != line2) {
            fs1.close(); // Close files before exiting function
            fs2.close();
            return false;
        }
    }

    // Check if both files have reached EOF
    if (fs1.eof() && fs2.eof()) {
        fs1.close(); // Close files before exiting function
        fs2.close();
        return true;
    }
    else {
        fs1.close(); // Close files before exiting function
        fs2.close();
        return false;
    }
}

std::string readFile(const std::string& fileName) {
    std::string fileContents;

    std::ifstream file(fileName);

    if (file.is_open()) {
        //Read the contents
        std::string line;
        while (getline(file, line)) {
            fileContents += line + '\n';
        }
        file.close();
    }
    else {
        return "Unable to open file";
    }
    return fileContents;
}

std::vector<int> readNumbers(std::string filename) {
    std::ifstream infile(filename);
    std::vector<int> numbers;

    if (!infile) {
        std::cout << "Error opening " + filename << std::endl;
        return numbers;
    }

    int num;
    while (infile >> num) {
        numbers.push_back(num);
    }

    infile.close();
    return numbers;
}


template<typename T>
void writeToVector(const std::string& filepath, const  std::vector<T>& data)
{

    std::ofstream filestream(filepath);
    std::copy(data.begin(), data.end(), std::ostream_iterator<T>(filestream, " "));
    filestream.close();

}

void writeToString(const std::string& filename, const std::string& contents) {
    // Create an output file stream object and open the file for writing
    std::ofstream outfile(filename, std::ofstream::trunc);

    // Use std::ostringstream to store the contents in a string
    std::ostringstream ss;
    ss << contents;

    // Remove the newline character from the end of the string if it exists
    std::string output = ss.str();
    if (output.back() == '\n') {
        output.pop_back();
    }

    // Write the contents to the file
    outfile << output;

    // Close the file
    outfile.close();
}

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(const std::string& uncompressed, Iterator result) {
    // Build the dictionary.
    int dictSize = 256;
    int codeLength = 9;
    std::map<std::string, int> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[std::string(1, i)] = i;

    std::string w;
    for (std::string::const_iterator it = uncompressed.begin();
        it != uncompressed.end(); ++it) {
        char c = *it;
        std::string wc = w + c;
        if (dictionary.count(wc))
            w = wc;
        else {
            *result++ = dictionary[w];
            // Add wc to the dictionary.
            dictionary[wc] = dictSize++;
            if (dictSize >= (1 << codeLength) && codeLength < 16) {
                // Increase code length if dictionary is full and we're not already at the maximum length.
                codeLength++;
            }
            w = std::string(1, c);
        }
    }

    // Output the code for w.
    if (!w.empty())
        *result++ = dictionary[w];
    return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
std::string decompress(Iterator begin, Iterator end) {
    // Build the dictionary.
    int dictSize = 256;
    int codeLength = 9;
    std::map<int, std::string> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[i] = std::string(1, i);

    std::string w(1, *begin++);
    std::string result = w;
    std::string entry;
    for (; begin != end; begin++) {
        int k = *begin;
        std::string entry;
        if (dictionary.count(k))
            entry = dictionary[k];
        else if (k == dictSize)
            entry = w + w[0];
        else
            throw "Bad compressed k";

        result += entry;

        // Add w+entry[0] to the dictionary.
        dictionary[dictSize++] = w + entry[0];

        if (dictSize >= (1 << codeLength) && codeLength < 16) {
            // Increase code length if dictionary is full and we're not already at the maximum length.
            codeLength++;
        }

        w = entry;
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: : lzw435 c filename to compress and lzw435 e filename.lzw to expand.";
        return -1;
    }

    std::string operation = argv[1];
    std::string filename = argv[2];
    std::string name = removeExtension(filename);


    std::vector<int> compressed;
    if (operation == "c") {
        std::string fileContents = readFile(filename);
        compress(fileContents, std::back_inserter(compressed));


        std::cout << "File:";
        copy(compressed.begin(), compressed.end(), std::ostream_iterator<int>(std::cout, ", "));

        std::vector<int> to_vector;
        writeToVector(name + ".lzw", compressed);

    }

    if (operation == "e") {

        std::vector<int> fileNumbers = readNumbers(filename);
        std::string decompressed = decompress(fileNumbers.begin(), fileNumbers.end());
        writeToString(name + "2.txt", decompressed);

        if (compareFiles(name + "2.txt", name + ".txt")) {
            std::cout << "Identical" << std::endl;
        }
        else
            std::cout << "Different" << std::endl;
        return 0;
    }

    if (operation == "i") {

        if (compareFiles(name + "2.txt", name + ".txt")) {
            std::cout << "Identical" << std::endl;
        }
        else
            std::cout << "Different" << std::endl;
        return 0;
    }

    return 0;
}


/*
Part II: Modified LZW to allow the length of LZW codes to increase gradually.
• Your algorithm will increase the length of the code words from 9 to 16 bits.
• Do nothing when the length reaches 16 bits.
Name your modified LZW program “lzw435M.cpp”. Once we compile the program, we should be able to run it using:
“lzw435M.exe c filename” to compress the file “filename”. You program should save the compressed file as:
“filename.lzw2”. And we should be able to run it using: “lzw435M.exe e filename.lzw2” to expand the compressed
file “filename.lzw2”. You program should save the expanded file as: “filename.2M”. You know your file named
“filename.2M” should be identical to that of name “filename”.
Pay special attention to synchronize the compression and expansion to make sure they work correctly. Test your
program thoroughly
*/